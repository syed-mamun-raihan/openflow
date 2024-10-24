#ifndef _IPCLIENT_H
#define _IPCLIENT_H
/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include <iostream>
#include <string>
#include <cstdint>
#include <climits>

//Provides Ip connectivity functionality
class IpClient {
public:
    //  Default constructor.
    //  Does nothing special.
    IpClient();

    //  Default destructor.
    //  Does nothing special.
    virtual ~IpClient();
    int GetSocketError(int& value, const int sock ) const;
    bool CheckDataOut(const int fd, int& error_code);
    bool CheckDataIn(const int fd, int& error_code);

protected:
    virtual bool HandleSocketCLoseError() = 0;
    //  Opens socket for communicating with TCP or UDP %server.
    //  \param  host hostname or IP address of the %server
    //  \param  service port number of the %server
    //  \param  protocol "TCP" or "UDP"
    //  \return network socket descriptor
    int ConnectSock(const std::string & host, const std::string & service, const std::string & protocol);

    //Sets 'fd' to non-blocking mode.  Returns 0 if successful, otherwise a
    //positive errno value.
    int SetNonBlocking(const int fd) const;
    virtual int GetSocketType() const = 0;

    int mLastErrorNo;
};

#endif
