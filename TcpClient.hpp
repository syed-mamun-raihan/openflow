#ifndef _TCPCLIENT_H
#define _TCPCLIENT_H

/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */

#include <iostream>
#include <string>
#include <cstdint>
#include <ctime>
#include "OfpHeader.h"
#include "IpClient.hpp"
#include "AbstractConn.hpp"

//  Class for sending/reciving data from/to %server.
//  Implements network %client.
//  At this point DPManager sets mSock instead of opening the socket by itself and then read it in run_Active
class TcpClient : public IpClient {
public:
    //  Default constructor.
    //  Initialize variables.
    TcpClient();

    //  Default destructor.
    //  Free any allocated momory.
    ~TcpClient();

    //  Sends data (string) to the %server.
    //  \param  host hostname or IP address of the %server
    //  \param  service port number of the %server
    //  \param  packet a string to send
    bool Send(const char * packet, const unsigned length);

    //  Sends data (string) to the %server, waits for response from this %server and then recives it.
    //  \param  packet a string to send
    //  \return "true" if everything is OK or "false" if there is an error
    bool Receive(const uint8_t* pBuffer, const unsigned length);

    //It opens a connection using msHost and msPort object
    bool Open(AbstractConn& lParent);

    //Closes socket for listening for TCP connections from clients.
    void Close(AbstractConn& lParent);

    inline int32_t GetSocketFd() const{
        return((mpParent && mpParent->GetSocketFd() > 0) ?
               mpParent->GetSocketFd() : 0);
    }
    inline bool IsOpen() const { return(GetSocketFd() > 0);}
    inline int& GetLastErrorNo() { return mLastErrorNo;}
    inline const int& GetLastErrorNo() const { return mLastErrorNo;}
    virtual int GetSocketType() const;
    bool GetMacAddress(char* mac_address) const;


protected:
    const std::string msType = "tcp";
    AbstractConn* mpParent = 0;
    virtual bool HandleSocketCLoseError();

private:
    TcpClient(const TcpClient& );
    TcpClient& operator = (const TcpClient& );
};
#endif
