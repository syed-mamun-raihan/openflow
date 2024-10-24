#ifndef _SSLCLIENT_H
#define _SSLCLIENT_H
/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */

#include <iostream>
#include <string>
#include <ctime>
#include <cstdint>
#include <string.h>
extern "C" {
#include <openssl/ssl.h>
#include <openssl/err.h>
}
#include "TcpClient.hpp"
class AbstractConn;

template<typename ConnClient = TcpClient>
class SslClient {
public:
    // CTOR
    SslClient();

    //DTOR
    ~SslClient();

    //  Sends data (string) to the %server.
    //  \param  host hostname or IP address of the %server
    //  \param  service port number of the %server
    //  \param  packet a string to send
    bool Send(const char * packet, const uint32_t length);

    //  Receive data (string) from the %server
    //  \param  packet a string to send
    //  \return "true" if everything is OK or "false" if there is an error
    bool Receive(const uint8_t* pBuffer, const unsigned length);

    // we dont open SSL socket but initiate negotiation
    bool Open(AbstractConn& lParent);

    //Closes socket.
    void Close(AbstractConn& lParent);

    //\Returns the error condition associated with SSL* 'mpSSL'
    int GetSocketError(const int error_code) const;
    inline bool IsOpen() const { return(mConnObj.IsOpen() && mbSslConnected) ;}
    inline int& GetLastErrorNo() { return mConnObj.GetLastErrorNo();}
    inline const int& GetLastErrorNo() const { return mConnObj.GetLastErrorNo();}
    inline bool CheckDataIn(const int fd, int& error_code) { return mConnObj.CheckDataIn(fd, error_code);}
    inline bool CheckDataOut(const int fd, int& error_code) { return mConnObj.CheckDataOut(fd, error_code);}
    // temp patch it will work for only single client system
    inline int GetSocketFd() const { return mConnObj.GetSocketFd();}
    inline bool GetMacAddress(char* mac_address) const { return mConnObj.GetMacAddress(mac_address);}

    // public data
    std::string msClientCert;
    std::string msClientKey;
    std::string msCaCert;
    ConnClient mConnObj;

protected: // data
    const SSL_METHOD* mpMethod = 0;
    SSL_CTX*    mpCtx = 0;
    SSL* mpSSl = 0;
    bool mbSslConnected = false;
    bool mbPrivateKeyLoaded = false;
    bool mbClientCertLoaded = false;
    bool mbCaCertLoaded = false;
protected: // functions
    // Called as part of Open attempt
    bool SetupSSL(std::ostream& out);
    // Called as part of Open attempt
    bool SetupTCP(AbstractConn & lParent, std::ostream& out);

private: // following functions are deleted
    SslClient(const SslClient& );
    SslClient& operator = (const SslClient& );
};
extern template class SslClient < TcpClient >;

#endif
