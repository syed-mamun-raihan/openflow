/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include <climits>
#include <limits>
#include <typeinfo>
#include <ctime>
#include <ostream>
#include <unistd.h>
#include <errno.h>
#include "GenericConn.hpp"
#include "ofDebug.h"
#include "OfDpDefine.hpp"
#include "OfpHeader.h"
#include "AbstractParser.hpp"
#include "OfConnStateManager.hpp"

template<typename ConnClient>
GenericConn<ConnClient>::GenericConn(OfConnManager& switchP, const OfDpControllerAttachmentT& obj, const time_t now):
AbstractConn(switchP, obj, now)
,mLogStr(mLogBuffer, sizeof(mLogBuffer))
,mLogOut(mLogStr)
{
}

template<typename ConnClient>
GenericConn<ConnClient>::~GenericConn()
{
    while ( this->mSendRetryList.size() ) {
        typename std::list<const SendRetry*>::const_reference retry = this->mSendRetryList.front();
        delete retry;
        this->mSendRetryList.pop_front();
    }
}

template<typename ConnClient>
bool GenericConn<ConnClient>::Open() { return mConnObj.Open(*static_cast<AbstractConn*>(this));}

template<typename ConnClient>
size_t GenericConn<ConnClient>::GetMaxReadBufferLen() const { return sizeof(mpcReadBuffer);}

template<typename ConnClient>
void GenericConn<ConnClient>::SendQueue() {
    const unsigned maxAttempt = 16;
    const time_t now = time(0);
    unsigned i = 0;
    int error_ret = 0;
    std::lock_guard<std::mutex> lock(mMutexSendRetryList);
    while ( this->mSendRetryList.size() && i++ < maxAttempt ) {
        typename std::list<const SendRetry*>::const_reference retry =
        this->mSendRetryList.front();
        if ( mConnObj.CheckDataOut(mConnObj.GetSocketFd(), error_ret) &&
             mConnObj.Send(retry->buffer, retry->len_buffer) ) {
            if ( mPacketsSent == std::numeric_limits<typeof (mPacketsSent)>::max() )
                mPacketsSent = 0;
            this->mLastActivityTime = now;
            ++mPacketsSent;
            if ( retry->partialMsg && mPartialMsgSentCount >= 1 ) --mPartialMsgSentCount;
            delete retry;
            this->mSendRetryList.pop_front();
        } else {
            break;
        }
    }
}

template<typename ConnClient>
bool GenericConn<ConnClient>::Send(const ofp_message & data, const bool mustSend)
{
    SendQueue();
    bool bRetVal = false;
    int error_ret = 0;
    std::lock_guard<std::mutex> lock(mMutexSendRetryList);
    if ( this->mSendRetryList.size() && mustSend && IsOpen() ) {// need to maintain order just in case
        const SendRetry *new_retry = new SendRetry(reinterpret_cast<const char*>(&data), data.get_length());
        if ( (data.type == this->mpParser->get_echo_type() ||
              data.type == this->mpParser->get_echo_reply_type()) &&
             mPartialMsgSentCount == 0 )
            this->mSendRetryList.push_front(new_retry);
        else
            this->mSendRetryList.push_back(new_retry);
        bRetVal = true;
    } else if ( mConnObj.CheckDataOut(mConnObj.GetSocketFd(), error_ret) &&
                IsOpen() &&
                mConnObj.Send((const char*)&data, data.get_length()) ) {
        if ( mPacketsSent == std::numeric_limits<typeof(mPacketsSent)>::max() ) mPacketsSent = 0;
        ++mPacketsSent;
        this->mLastActivityTime = (data.type != this->mpParser->get_echo_type())?
                                  time(0) : this->mLastActivityTime;
        bRetVal = true;
    } else if ( mConnObj.GetLastErrorNo() > 0 &&
                mConnObj.GetLastErrorNo() < data.get_length() &&
                IsOpen() ) {
        ++mPartialMsgSentCount;
        const size_t buf_len = data.get_length()-mConnObj.GetLastErrorNo();
        const SendRetry *new_retry = new SendRetry(reinterpret_cast<const char*>(&data)+mConnObj.GetLastErrorNo(), buf_len, true);
        this->mSendRetryList.push_back(new_retry);
        bRetVal = true;
    } else if ( mustSend && IsOpen() ) {
        const SendRetry *new_retry = new SendRetry(reinterpret_cast<const char*>(&data), data.get_length());
        if ( (data.type == this->mpParser->get_echo_type() ||
              data.type == this->mpParser->get_echo_reply_type()) &&
             mPartialMsgSentCount == 0 )
            this->mSendRetryList.push_front(new_retry);
        else
            this->mSendRetryList.push_back(new_retry);
        bRetVal = true;
    }
    return bRetVal;
}

// gets ofp_message from controller
template<typename ConnClient>
bool GenericConn<ConnClient>::Receive(const time_t now)
{
    SendQueue();
    bool retVal = false;
    int error_ret = 0;
    if (mConnObj.CheckDataIn(mConnObj.GetSocketFd(), error_ret) &&
        mConnObj.Receive(mpcReadBuffer+mpPartialMessageLength, GetMaxReadBufferLen()-mpPartialMessageLength) ) {
        this->mLastActivityTime = now;
        size_t bytes_processed = 0;
        const size_t bytes_read = mConnObj.GetLastErrorNo() + mpPartialMessageLength;
        mpPartialMessageLength = 0;
        DBG( DBG_PARSER_PARSE|DBG_DETAIL, (" Time@  = %ul, State@ = %s\n", (uint32_t)now, this->GetCurrentStateName().c_str() ) );

        ofp_message* msg = reinterpret_cast<ofp_message*>(mpcReadBuffer+bytes_processed);

        while ( bytes_read > bytes_processed ) {
            if ( msg->get_length() == 0 || msg->get_length() > GetMaxReadBufferLen() ) {
                DBG_ERROR( (" error: message length =  %i\n", msg->get_length()) );
                break;
            }
            mLogOut.seekp(0, std::ios_base::beg);
            if ( msg->get_length() <= bytes_read &&
                 msg->get_length() >= sizeof(ofp_message) &&
                 AbstractConn::process_of_msg(*msg,now, mLogOut) ) {
                if ( mPacketsReceived == std::numeric_limits<typeof(mPacketsReceived)>::max() )
                    mPacketsReceived = 0;
                ++mPacketsReceived;
                retVal = true;
            }
            mLogOut << std::ends;
            DBG( DBG_PARSER_PARSE, (" %s\n", mLogOut.str().c_str()) );
            // usually for a known type with length error may be considered as packet corruptions
            if ( !retVal ) { // something terrible happens parser wants us to close sockets
                DBG_ERROR((" irrecoverable errors due to client or controller failure, *** closing sockets ***\n") );
                Close(); // we can not trust current or any left over packets
                break;
            }
            bytes_processed += msg->get_length();
            msg = reinterpret_cast<ofp_message*>(mpcReadBuffer+bytes_processed);
            if ( bytes_processed >= bytes_read ||   // no more bytes to work with
                 (((bytes_read - bytes_processed) >= 4) && // check if corrupt message was sent
                  (msg->get_length() == 0 || msg->get_length() > GetMaxReadBufferLen()) ) ) {
            	DBG_DETAILS((" error in message length or it is done processing bytes\n") );
                break;
            }
            if ( (bytes_read - bytes_processed) < 4  // not enough bytes to check length
                 || (msg->get_length() + bytes_processed) > bytes_read ) { // not enough bytes to continue process
                mpPartialMessageLength = (bytes_read - bytes_processed);
                if ( mpPartialMessageLength && mpPartialMessageLength < GetMaxReadBufferLen() ) {
                    memmove(mpcReadBuffer, msg, mpPartialMessageLength); // we have partial message
                    DBG_ERROR((" partial bytes %lu \n",(long unsigned)mpPartialMessageLength ) );
                } else {
                    DBG_ERROR((" corrupt partial bytes length %lu \n", (long unsigned)mpPartialMessageLength ) );
                    mpPartialMessageLength = 0; // something went bad
                }
                break;//left over bytes or nothing else is left
            }

        }// end while bytes_read > bytes_processed

        DBG_DETAILS((" rcvd %lu, send %lu, bytes(read %lu, processed %lu)\n", (long unsigned)mPacketsReceived, (long unsigned)mPacketsSent, (long unsigned)bytes_read, (long unsigned)bytes_processed));
    } // read
    else if ( mConnObj.GetLastErrorNo() == 0 ) {
        mpStateMgr->TransitionTo(OfConnStateManager::S_VOID, now);
    }
    return(retVal);
}

template<typename ConnClient>
void GenericConn<ConnClient>::Close()
{
    mConnObj.Close(*static_cast<AbstractConn*>(this));
    if ( mpPartialMessageLength ) {
        DBG_ERROR((" warn: socket closed, but partial message is not processed\n"));
        mpPartialMessageLength = 0;// cleanup after it is closed
    }
    if ( this->mSendRetryList.size() > 1 )
        DBG_ERROR((" warn: socket closed, send retry list size %lu\n", (long unsigned)(this->mSendRetryList.size())));
    while ( this->mSendRetryList.size() ) {
        typename std::list<const SendRetry*>::const_reference retry = this->mSendRetryList.front();
        delete retry;
        this->mSendRetryList.pop_front();
    }
}

// test for if socket is ready
template<typename ConnClient>
bool GenericConn<ConnClient>::IsOpen() const { return mConnObj.IsOpen();}

// To explicitly instantiate following classes here
template class GenericConn<TcpClient>;
template class GenericConn<SslClient<TcpClient> >;

