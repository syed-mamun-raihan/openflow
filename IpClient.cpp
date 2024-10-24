/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include "OfDpDefine.hpp"
#include "IpClient.hpp"

//  Default constructor.
//  Does nothing special.
IpClient::IpClient():mLastErrorNo(-ENOTCONN)
{
    signal(SIGPIPE, SIG_IGN);
}

//  Default destructor.
//  Does nothing special.
IpClient::~IpClient(){
}

//  Opens socket for communicating with TCP or UDP %server.
//  \param  host hostname or IP address of the %server
//  \param  service port number of the %server
//  \param  protocol "TCP" or "UDP"
//  \return network socket descriptor
int IpClient::ConnectSock(const std::string & host, 
                          const std::string & service, 
                          const std::string & protocol) {
    struct sockaddr_in sin;
    const char* host_str = host.data();
    const char* service_str = service.data();
    const char* protocol_str = protocol.data();
    mLastErrorNo = -ENOTCONN;
    memset ((void*) &sin, 0, sizeof (sin));
    sin.sin_family = AF_INET;
    const struct servent *pse = getservbyname (service_str, protocol_str);
    if ( pse )
        sin.sin_port = pse->s_port;
    else if ( (sin.sin_port = htons ((ushort) atoi (service_str))) == 0 ) {
        sin.sin_port = htons(OFP_TCP_PORT);
    }
    const struct hostent *phe = gethostbyname (host_str);
    if ( phe )
        memcpy ((char*) &sin.sin_addr, phe->h_addr, phe->h_length);
    else if ( (sin.sin_addr.s_addr = inet_addr (host_str)) == INADDR_NONE ) {
        mLastErrorNo = errno;
        return 0;
    }
    const struct protoent *ppe = getprotobyname (protocol_str);
    if ( !ppe ) {
        mLastErrorNo = errno;
        return 0;
    }
    const int type=GetSocketType();
    const int sock_fd = socket (AF_INET, type, ppe->p_proto);
    mLastErrorNo = (sock_fd > 0)? SetNonBlocking(sock_fd) : errno;
    if ( mLastErrorNo ) {
        close(sock_fd);
    }
    if ( sock_fd > 0 &&
         connect (sock_fd, (struct sockaddr *) &sin, sizeof (sin)) < 0 &&
         errno != EINPROGRESS ) {
        mLastErrorNo = errno;
        close(sock_fd);
    }// we have a screaming socket
    int set = 1;
    setsockopt(sock_fd, SOL_SOCKET, MSG_NOSIGNAL, (void *)&set, sizeof(int));
    return sock_fd;
}

//Sets 'fd' to non-blocking mode.  Returns 0 if successful, otherwise a
//positive errno value.
int IpClient::SetNonBlocking(const int fd) const {
    const int flags = fcntl(fd, F_GETFL, 0);
    if ( flags != -1 ) {
        if ( fcntl(fd, F_SETFL, flags | O_NONBLOCK) != -1 ) {
            return 0;
        }
    }
    return errno;
}

bool IpClient::CheckDataOut(const int fd, int& error_code)
{
    if ( !(fd > 0) ) return false;
    unsigned n_attempts = 0;
    const unsigned max_attempts = 1;
    const int timeout_msecs = 1;
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLOUT;
    int retVal = 0;
    do {
        retVal = poll(&pfd, 1, timeout_msecs);
    } while ( retVal < 0 && errno == EINTR && ++n_attempts < max_attempts );
    error_code = errno;
    if ( retVal == 1 ) {
        if ( pfd.revents & POLLERR ) {
            const ssize_t n = send(fd, "", 1, MSG_DONTWAIT);
            if ( n < 0 ) {
                error_code = errno;
            } else {
                std::cout << "Error: poll return POLLERR but send succeeded\n";
                error_code = EPROTO;
            }
        }
        const int retVal = GetSocketError(error_code, fd);
        if (error_code == EPIPE ||
            error_code == ENOTCONN ||
            error_code == ECONNRESET) {
            HandleSocketCLoseError();
        }
        return(retVal == 0 && error_code == 0 && !(pfd.revents & POLLERR));
    } else {
        if (error_code == EPIPE ||
            error_code == ENOTCONN ||
            error_code == ECONNRESET) {
            HandleSocketCLoseError();
        }
    }
    return false;
}

bool IpClient::CheckDataIn(const int fd, int& error_code)
{
    if ( !(fd > 0) ) return false;
    unsigned n_attempts = 0;
    const unsigned max_attempts = 1;
    const int timeout_msecs = 1;
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    int retVal = 0;
    do {
        retVal = poll(&pfd, 1, timeout_msecs);
    } while ( retVal < 0 && errno == EINTR && ++n_attempts < max_attempts );
    error_code = errno;
    if ( retVal == 1 ) {
        const int retVal = GetSocketError(error_code, fd);
        if (error_code == EPIPE ||
            error_code == ENOTCONN ||
            error_code == ECONNRESET) {
            HandleSocketCLoseError();
        }
        return(retVal == 0 && error_code == 0 && !(pfd.revents & POLLERR));
    } else {
        if (error_code == EPIPE ||
            error_code == ENOTCONN ||
            error_code == ECONNRESET) {
            HandleSocketCLoseError();
        }
    }
    return false;
}

int IpClient::GetSocketError(int& error_val, const int sock) const{
    socklen_t len= sizeof (int);
    const int error_ret = getsockopt(sock, SOL_SOCKET, SO_ERROR, &error_val, &len);
    error_val = error_ret ? errno : error_val; // see if we can trust error_val
    // now reset socket error
    int reset_val = 0;
    getsockopt(sock, SOL_SOCKET, SO_ERROR, &reset_val, &len);
    return error_ret;
}
