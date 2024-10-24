/**
 * For commercial, educational or non-profit use please contact 
 * syed.raihan@gmail.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */

#include <errno.h>
#include "TcpClient.hpp"
#include "SslClient.hpp"
#include "OfpHeader.h"
#include "AbstractConn.hpp"
#include "OfDpLogicalSw.h"

#define STREAM_BUFFER() out.str().c_str()
template<typename ConnType>
SslClient<ConnType>::SslClient():mpMethod(TLSv1_client_method())
{
    SSL_library_init();/* load encryption & hash algorithms for SSL */
    SSL_load_error_strings(); /* load the error strings for good error reporting */
    if ( mpMethod && !mpCtx) {
        mpCtx = SSL_CTX_new(const_cast<SSL_METHOD*>(mpMethod));
        if (mpCtx) {
            SSL_CTX_set_mode(mpCtx, 0);
        }
    }
}

template<typename ConnType>
SslClient<ConnType>::~SslClient()
{
    if ( mpSSl ) {
        SSL_shutdown(mpSSl);
        SSL_free(mpSSl);
        mpSSl = 0;
    }
    if ( mpCtx ) {
        SSL_CTX_free(mpCtx);
        mpCtx = 0;
    }
}

template<typename ConnType>
bool SslClient<ConnType>::SetupSSL(std::ostream& )
{
    long error_ret=0;
    if ( !mpSSl && mpCtx ) {
        /*----- Load a client certificate into the SSL_CTX structure -----*/
        if ( !mbPrivateKeyLoaded )
            error_ret = SSL_CTX_use_PrivateKey_file(mpCtx,msClientKey.data(), SSL_FILETYPE_PEM);
        else
            error_ret = 1; // fallback to next step
        if ( error_ret == 1 ) {
            mbPrivateKeyLoaded = true;
            /*----- Load a private-key into the SSL_CTX structure -----*/
            if ( !mbClientCertLoaded )
                error_ret = SSL_CTX_use_certificate_file(mpCtx,msClientCert.data(), SSL_FILETYPE_PEM);
            else
                error_ret = 1; // fall back to next step
            if ( error_ret == 1 &&
                 SSL_CTX_check_private_key(mpCtx) ) {
                mbClientCertLoaded = true;
                /* Load trusted CA. */
                if ( !mbCaCertLoaded )
                    error_ret = SSL_CTX_load_verify_locations(mpCtx,msCaCert.data(),NULL);
                else
                    error_ret = 1; // fall back to next step
                if ( error_ret == 1 ) {
                    mbCaCertLoaded = true;
                    // Create SSL_new
                    mpSSl = SSL_new(mpCtx);
                    DBG( DBG_PARSER_SOCKET, ( (mpSSl? " SSL* is initialized. " : " SSL* failed. ")));
                } else {
                    DBG( DBG_PARSER_SOCKET, ( "\n Error: SSL_CTX_load_verify_locations %s \n", msCaCert.data()));
                }
            } else {
                DBG( DBG_PARSER_SOCKET, ( "\n Error: SSL_CTX_check_private_key or SSL_CTX_use_certificate_file %s \n", msClientCert.data()));
            }
        } else {
            DBG( DBG_PARSER_SOCKET, ( "\n Error: SSL_CTX_use_PrivateKey_file %s\n", msClientKey.data()));
        }
    }// SSL* is already been created
    return(mpSSl);
}

// It will make an attempt to SSL_connect
template<typename ConnType>
bool SslClient<ConnType>::SetupTCP(AbstractConn & lParent, std::ostream& )
{
    int error_ret=0;
    if ( mpSSl && !mConnObj.IsOpen() ) {
        mConnObj.Open(lParent);
        DBG( DBG_PARSER_SOCKET, ( "\nSSL socket set to %i \n",mConnObj.GetSocketFd()));
    }
    if ( mConnObj.IsOpen() && !mbSslConnected && mpSSl &&
         CheckDataOut(mConnObj.GetSocketFd(), error_ret) ) {
        SSL_set_fd(mpSSl, mConnObj.GetSocketFd());
        do {
            error_ret = SSL_connect(mpSSl);
            GetLastErrorNo() = GetSocketError(error_ret);
        }while (GetLastErrorNo() == SSL_ERROR_WANT_READ ||
                GetLastErrorNo() == SSL_ERROR_WANT_WRITE ||
                GetLastErrorNo() == SSL_ERROR_WANT_CONNECT);
        mbSslConnected = ( error_ret == 1);
        char buf[1024];
        buf[0]='\0';
        if ( !mbSslConnected ) {
            ERR_error_string_n(error_ret, buf, 1024);
            DBG( DBG_PARSER_SOCKET, (" SSL_Error_code: %i, %s \n",GetLastErrorNo() , buf));
        }
        DBG( DBG_PARSER_SOCKET, ("\nSSL_connect: %s\n",(mbSslConnected?"Success":"Failed")));
    }
    return(mbSslConnected);
}

template<typename ConnType>
bool SslClient<ConnType>::Open(AbstractConn& lParent) 
{
    const char* func = " SslClient::Open- ";
    if ( SslClient::IsOpen() ) {
        DBG( DBG_PARSER_SOCKET, (" %s - Success\n", func) );
        return mbSslConnected;
    } else if ( !(mpMethod && mpCtx && msClientCert.size() && msClientKey.size() && msCaCert.size()) ) {
        DBG( DBG_PARSER_SOCKET, (" %s - SslMethod/CTX/Key/Certificates not set yet\n", func) );
        return false;
    }

    std::ostream out(0);
    if (SetupSSL(out)) SetupTCP(lParent, out);
    if (GetLastErrorNo() == SSL_ERROR_SYSCALL) {
        lParent.Close();
    }
    DBG( DBG_PARSER_SOCKET, (" %s - %s\n", func,(mbSslConnected?"Success":"Failed")) );
    return mbSslConnected;
}

template<typename ConnType>
bool SslClient<ConnType>::Receive(const uint8_t* pBuffer, const unsigned length) {
    int error_code = -1;
    if ( mbSslConnected && mpSSl && pBuffer && length > 0 ) {
        error_code = SSL_read(mpSSl,(void *)pBuffer,length);
        if ( !(error_code > 0) ) {
            switch ( GetSocketError(error_code) ) {
            case SSL_ERROR_ZERO_RETURN:
                break;
            case SSL_ERROR_WANT_READ:
                do {
                    error_code = SSL_read(mpSSl,(void *)pBuffer,length);
                }while ( error_code <= 0 && GetSocketError(error_code) == SSL_ERROR_WANT_READ );
                break;
            case SSL_ERROR_SYSCALL:
            case SSL_ERROR_SSL:
            default:
                DBG( DBG_PARSER_SOCKET, (" SslClient::Receive - IO error or SSL Library error\n"));
                break;
            };
        }
    }
    GetLastErrorNo() = error_code;
    return(error_code > 0);
}

template<typename ConnType>
bool SslClient<ConnType>::Send(const char * packet, const uint32_t length) 
{
    int error_code = -1;
    if ( mbSslConnected && mpSSl && packet && length > 0 ) {
        error_code = SSL_write(mpSSl, packet, length);
        if ( !(error_code == (int)length) ) {
            switch ( GetSocketError(error_code) ) {
            case SSL_ERROR_ZERO_RETURN:
                break;
            case SSL_ERROR_WANT_WRITE:
                do {
                    error_code = SSL_write(mpSSl, packet, length);
                }while ( error_code <= 0 && GetSocketError(error_code) == SSL_ERROR_WANT_WRITE );
                break;
            case SSL_ERROR_SYSCALL:
            case SSL_ERROR_SSL:
            default:
                DBG( DBG_PARSER_SOCKET, (" SslClient::Send - IO error or SSL Library error\n"));
                break;
            };
        }
    }
    GetLastErrorNo() = error_code;
    return(error_code == (int)length);
}

// we do not close here
// @see DTOR
template<typename ConnType>
void SslClient<ConnType>::Close(AbstractConn& lParent) 
{
    if ( mbSslConnected )mbSslConnected = false;
    if ( mpSSl ) {
        SSL_shutdown(mpSSl);
        SSL_free(mpSSl);
        mpSSl = 0;
    }
    mConnObj.Close(lParent);
}

//@return the error condition associated with socket 'mSock' and resets the
//socket's error status.
template<typename ConnType>
int SslClient<ConnType>::GetSocketError(const int error_code) const  {
    return SSL_get_error(mpSSl, error_code);
}

template class SslClient < TcpClient >;
