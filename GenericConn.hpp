#ifndef _IVCONNTCPIMPL_H
#define _IVCONNTCPIMPL_H
/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
#include <string.h>
#include <list>
#include <cstdint>
#include <mutex>
#include "SslClient.hpp"

class AbstractConn;
class OfConnManager;
struct ofp_message;

// Desc:
// Uses Decorator Pattern - It is the concrete class for Connection Manager
//
// It is also a surrogate class for TcpClient or SslClient (see: proxy pattern)
// See: SslClient & TcpClient
// Both SslClient and TcpClient must have the same signature for Open(), Close(), Send() & Receive()
template<typename ConnClient = TcpClient>
class GenericConn final : public AbstractConn {
public:
    /**
     * 
     */
    GenericConn(OfConnManager& , const OfDpControllerAttachmentT&, const time_t now);

    /**
     * 
     * 
     * @author sraihan (7/21/2014)
     */
    virtual ~GenericConn();

    /**
     * Open connections
     * 
     * @author sraihan (7/21/2014)
     * 
     * @return bool 
     */
    virtual bool Open();

    /**
     * send an openflow message to a controller
     * 
     * @author sraihan (7/21/2014)
     * 
     * @param data 
     * @param mustSend 
     * 
     * @return bool 
     */
    virtual bool Send(const ofp_message & data, const bool mustSend = true);

    /**
     * Receive a openflow message
     * 
     * @author sraihan (7/21/2014)
     * 
     * @param now 
     * 
     * @return bool 
     */
    virtual bool Receive(const time_t now);

    /**
     * Used by state machine primarily
     * 
     * @author sraihan (7/21/2014)
     */
    virtual void SendQueue();

    /**
     * Close the connections
     * 
     * @author sraihan (7/21/2014)
     */
    virtual void Close();

    /**
     * Test if connection is opened
     * 
     * @author sraihan (7/21/2014)
     * 
     * @return bool 
     */
    virtual bool IsOpen() const;

    /**
     * Get Max read buffer length
     * 
     * @author sraihan (7/21/2014)
     * 
     * @return size_t 
     */
    virtual size_t GetMaxReadBufferLen() const ;

    /**
     * Get Mac address from HW
     * 
     * @author sraihan (7/21/2014)
     * 
     * @param mac_address 
     * 
     * @return bool 
     */
    virtual bool GetMacAddress(char* mac_address) const { return mConnObj.GetMacAddress(mac_address);}


    ConnClient mConnObj;

protected:
    uint8_t mpcReadBuffer[_OF_JUMBO_PACKET_SIZE];
    size_t mpPartialMessageLength = 0;
    // This buffer is used to log for 1 message at a time
    char  mLogBuffer[_OF_JUMBO_PACKET_SIZE*8];
    const std::string mLogStr;
    std::ostringstream mLogOut;
    /**
     * Private struct to manage send retry list 
     * for packets 
     * 
     * @author sraihan (7/21/2014)
     */
    struct SendRetry {
        const char* buffer;
        const size_t len_buffer;
        const bool partialMsg;
        explicit SendRetry(const char* buf, const size_t len, const bool part = false):
        buffer(new char[len]),
        len_buffer(len),
        partialMsg(part)
        {
            memcpy(const_cast<void*>(reinterpret_cast<const void*>(buffer)), buf , len_buffer);
        }
        ~SendRetry(){
            if ( buffer ) delete[] buffer;
        }
    private:
        SendRetry(const SendRetry&);
        SendRetry& operator =(const SendRetry&);
    };

    std::list<const SendRetry*> mSendRetryList;
    unsigned long mPartialMsgSentCount = 0;
    std::mutex mMutexSendRetryList;
};

// To prevent automatic instantiation of following classes
extern template class GenericConn<TcpClient>;
extern template class GenericConn<SslClient<TcpClient> >;

#endif
