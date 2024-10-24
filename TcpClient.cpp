/**
 * For commercial, educational or non-profit use please contact 
 * syed.raihan@gmail.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */

#include <cassert>
#include <climits>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "OfpHeader.h"
#include "TcpClient.hpp"
#include "OfDpLogicalSw.h"

//  Default constructor.
//  Initialize variables.
TcpClient::TcpClient()
{
}

//  Default destructor.
//  Free any allocated momory.
TcpClient::~TcpClient(){
    if ( TcpClient::IsOpen() )
        close (mpParent->GetSocketFd());
}

//  Sends data (string) to the %server.
//  \param  length of packet
//  \param  packet to send
//  \return success or failure
bool TcpClient::Send(const char * packet, const unsigned length)
{
    if ( TcpClient::IsOpen() && packet && length > 0 /*&& IsOpened()*/ ) {
        mLastErrorNo = write (mpParent->GetSocketFd(), packet, length);
        return(mLastErrorNo == (int)length);
    }
    mLastErrorNo = -ENOTCONN;
    return false;
}

//  Sends data (string) to the %server, waits for response from this %server and then recives it.
//  \param  packet to read from port
//  \return "true" if everything is OK or "false" if there is an error
bool TcpClient::Receive(const uint8_t* pBuffer, const unsigned length) {
    if ( TcpClient::IsOpen() && pBuffer && length > 0 ) {
        mLastErrorNo = read (mpParent->GetSocketFd(), (void*)pBuffer, length);
        // mLastErrorNo holds no of bytes read
        return(mLastErrorNo > 0);
    }
    mLastErrorNo = -ENOTCONN;
    return false;
}

//It opens a connection using msHost and msPort object
bool TcpClient::Open(AbstractConn& lParent)
{
    if ( !TcpClient::IsOpen() ) {
        lParent.GetSocketFd() = ConnectSock(lParent.GetHost(), lParent.GetPort(), msType) ;
        if ( lParent.GetSocketFd() > 0 ) {
            lParent.ResetHelloStatus();
            mpParent = &lParent;
        }
    }
    return TcpClient::IsOpen();
}

// Closes socket for listening for TCP connections from clients.
// Closes if and only if msHost has been set else we dont own fd
// to reconnect
void TcpClient::Close(AbstractConn& lParent)
{
    if ( TcpClient::IsOpen() ) {
        shutdown(lParent.GetSocketFd(),2);
        close (lParent.GetSocketFd());
        lParent.GetSocketFd() = 0;
        lParent.ResetHelloStatus();
        mpParent = 0;
    }
}

int TcpClient::GetSocketType() const {
//    assert(msType == "tcp");
    return SOCK_STREAM;
}

// It is for trusted use that buffer would be non null and have at least 6 bytes
// Returns first MAC Address for device that is not loop back
bool TcpClient::GetMacAddress(char* mac_address) const
{
    int sock = 0;
    if ( mac_address ) {
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
        if ( sock == -1 ) {
            return false;
        };

        struct ifreq ifr;
        strcpy(ifr.ifr_name, "eth1");
        if ( ioctl(sock, SIOCGIFFLAGS, &ifr) == 0 ) {
            if ( ! (ifr.ifr_flags & IFF_LOOPBACK) ) { // don't count loopback
                if ( ioctl(sock, SIOCGIFHWADDR, &ifr) == 0 ) {
                    memcpy(mac_address, ifr.ifr_hwaddr.sa_data, 6);
                    close(sock);
                    return true;
                }
            }
        }
    }
    close(sock);
    return false;
}

bool TcpClient::HandleSocketCLoseError(){
    if (mpParent) {
        Close(*mpParent);
        return true;
    }
    return false;
}
