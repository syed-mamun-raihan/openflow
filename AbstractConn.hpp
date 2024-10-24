#ifndef _IVCONN_H
#define _IVCONN_H
/**
 * For commercial, educational or non-profit use please contact 
 * syed.raihan@gmail.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */

#include "OfConfig.hpp"
#include "GenericObservableConfig.hpp"
class OfConnManager;
class AbstractParser;
struct ofp_message;
class OfConnStateManager;

/**
 *@class OfConnManager
 *@author Syed M Raihan
 *@date 
 *@brief It contains controller attachments (@class AbstractConn)
 *@brief Uses Decorator Pattern and uses AbstractParser for parsing
 *@brief Incoming and Outgoing messages
 */
class AbstractConn : public GenericObservableConfig<OfDpControllerAttachmentT> {
public:
    /**
     * default CTOR for multi-ton creational patterns
     * 
     * @author sraihan ()
     * 
     * @param now 
     * 
     * @return explicit 
     */
    explicit AbstractConn(OfConnManager& ,const OfDpControllerAttachmentT& , const time_t now);

    /**
     * default DTOR
     * 
     * @author sraihan ()
     */
    virtual ~AbstractConn();

    /** 
     * It calls run function from StateManager. Which 
     * checks current state and transitions to next 
     * state if necessary, Reads a packet, calls process_of_msg() 
     *  
     * @author sraihan ()
     * 
     * @param now 
     * 
     * @return bool 
     */
    bool Run(const time_t now);

    /**
     * It opens a channel using a decorated surrogate class
     * 
     * @author sraihan ()
     * 
     * @return bool 
     */
    virtual bool Open() = 0;

    /**
     * It sends a packet using a decorated surrogate class
     * 
     * @author sraihan ()
     * 
     * @param data 
     * @param mustSend 
     * 
     * @return bool 
     */
    virtual bool Send(const ofp_message & data, const bool mustSend = true) = 0;

    /**
     * It tries to get a packet using a decorated surrogate class
     * 
     * @author sraihan ()
     * 
     * @param now 
     * 
     * @return bool 
     */
    virtual bool Receive(const time_t now) = 0;

    /**
     * It sends 5 packets saved before if any,
     * 
     * @author sraihan ()
     */
    virtual void SendQueue() = 0;

    /**
     * It closes this channel using a decorated surrogate class
     * 
     * @author sraihan ()
     */
    virtual void Close() = 0;

    /**
     * Utility functions
     * 
     * @author sraihan ()
     * 
     * @return size_t 
     */
    virtual size_t GetMaxReadBufferLen() const = 0;

    /**
     * Utility functions
     * 
     * @author sraihan ()
     * 
     * @return bool 
     */
    virtual bool IsOpen() const = 0;

    /**
     * Utility functions
     * 
     * @author sraihan ()
     * 
     * @return bool 
     */
    virtual bool GetMacAddress(char* mac_address) const = 0;

    /**
     * Utility functions
     * 
     * @author sraihan ()
     * 
     * @return int 
     */
    inline int GetSocketFd() const { return mClientSocket;}
    inline volatile int& GetSocketFd() { return mClientSocket;}

    /**
     * It returns true if one of the given param is a match
     * 
     * @author sraihan ()
     * 
     * @param fd 
     * @param host 
     * @param port 
     * 
     * @return bool 
     */
    inline bool Match(const int32_t fd,
                      const std::string& host,
                      const std::string& port ) const
    {
        return(( fd && fd == GetSocketFd() ) ||
               ( GetHost().compare(host) == 0 &&
                 GetPort().compare(port) == 0 ) );
    }
    inline std::string& GetHost() { return msHost;}
    inline const std::string& GetHost() const{ return msHost;}
    inline std::string& GetPort() { return msPort;}
    inline const std::string& GetPort() const{ return msPort;}

    /**
     * It may disconnects and then questions connectivity
     * 
     * @author sraihan ()
     * 
     * @param now 
     * 
     * @return bool 
     */
    bool BackOff(const time_t now);

    /**
     * @author sraihan ()
     *@param    current timestamp
     *@Desc     if active state is TimedOut then we switch to idle
     *          Idle state will send echo then every BackOff() period
     */
    bool TimedOut(const time_t now) const;

    ///\brief   return true if passive channel
    inline bool IsPassive() const { return GetCurrentRole() == OFDP_CHANNEL_ROLE_SLAVE || GetCurrentRole() == OFDP_CHANNEL_AUX_SLAVE;}

    /**
     * All ofp versions has hello
     * TODO: extended hello need to be implement when we have 
     * 1.3,1.4 e.t.c 
     * @author sraihan ()
     * 
     * @param xid 
     * 
     * @return bool 
     */
    bool send_hello(const uint32_t xid = 0);

    /**
     * In what state it is in
     * 
     * @author sraihan ()
     * 
     * @return const std::string& 
     */
    const std::string& GetCurrentStateName() const;

    /**
     * Used in time out management
     * 
     * @author sraihan ()
     * 
     * @return time_t 
     */
    inline time_t GetBackOff() const { return GetProbeInterval();}

    /**
     * Used in time out management
     * 
     * @author sraihan ()
     */
    inline void ResetHelloStatus() {
        mbHelloSend = false;
        mbHelloRecieved = false;
    }

    /**
     * Used in time out management
     * 
     * @author sraihan ()
     * 
     * @param now 
     */
    inline void ResetBackoff(const time_t now) {
        mLastQuestionedTime = now; // reset to prevent socket closing in next Run()
        mLastBackOffTime = now ; // reset to prevent timeout in next Run()
    }

    /**
     * For debugging - stats of ofp packets
     * 
     * @author sraihan ()
     * 
     * @return uint32_t 
     */
    inline uint32_t GetPacketsReceived() const { return mPacketsReceived;}

    /**
     * For debugging - stats of ofp packets
     * 
     * @author sraihan ()
     * 
     * @return uint32_t 
     */
    inline uint32_t GetPacketsSent() const { return mPacketsSent;}
    /**
     * Reconnect
     * 
     * @author sraihan ()
     * 
     * @param now 
     * 
     * @return bool 
     */
    bool Reconnect(const time_t now);

    /**
     * Just current state of connection - not actual connection 
     * state 
     * 
     * @author sraihan ()
     * 
     * @return bool 
     */
    bool IsConnected() const;

    /**
     * Dumps packet
     * 
     * @author sraihan ()
     * 
     * @param pPktBuffer 
     * @param length 
     * @param type_name 
     * @param out 
     */
    void DumpPacket(const uint8_t* pPktBuffer, const uint16_t length, const char* type_name, std::ostream& out);

    /**
     * refers to base configuration for connection attachment
     * 
     * @author sraihan ()
     * 
     * @return OfDpControllerChannelType 
     */
    inline OfDpControllerChannelType GetCurrentRole() const { return GetConfigObject().role;}

    /**
     * refers to base configuration for connection attachment
     * 
     * @author sraihan ()
     * 
     * @return OfDpControllerChannelType& 
     */
    inline OfDpControllerChannelType& GetCurrentRole() { return GetConfigObject().role;}

    /**
     * refers to base configuration for connection attachment
     * 
     * @author sraihan ()
     * 
     * @return int 
     */
    int GetVersion() const ;
    /**
     * refers to base configuration for connection attachment
     * 
     * @author sraihan ()
     * 
     * @return uint32_t 
     */
    inline uint32_t GetMaxRetries() const { return GetConfigObject().maxRetries;}
    /**
     * refers to base configuration for connection attachment
     * 
     * @author sraihan ()
     * 
     * @return time_t 
     */
    inline time_t GetProbeInterval() const { return GetConfigObject().probeInterval;}
    /**
     * refers to base configuration for connection attachment
     * 
     * @author sraihan ()
     * 
     * @return time_t 
     */
    inline time_t GetTimeoutInterval() const { return GetConfigObject().timeoutInterval;}
    /**
     * refers to base configuration for connection attachment
     * 
     * @author sraihan ()
     * 
     * @return uint32_t& 
     */
    inline uint32_t& GetMaxRetries() { return GetConfigObject().maxRetries;}
    /**
     * refers to base configuration for connection attachment
     * 
     * @author sraihan ()
     * 
     * @return time_t& 
     */
    inline time_t& GetProbeInterval() { return GetConfigObject().probeInterval;}
    /**
     * refers to base configuration for connection attachment
     * 
     * @author sraihan ()
     * 
     * @return time_t& 
     */
    inline time_t& GetTimeoutInterval() { return GetConfigObject().timeoutInterval;}

protected:
    /**
     * Each new parser implements a decorated version of 
     * process_of_msg for each openflow versions 
     * Allows Connection Managers concurrently multiple openflow 
     * versions 
     * 
     * @author sraihan ()
     * 
     * @param msg 
     * @param out 
     * 
     * @return Used in receive_message bool 
     */
    bool SelectNewParser(const ofp_message & msg, std::ostream & out);

    /**
     * This class implements base line functionality for processing 
     * of messages - and excpects concrete decorated implementations 
     * of AbstractParsers' process_of_msg will do the rest 
     * 
     * @author sraihan ()
     * 
     * @param msg 
     * @param now 
     * @param out 
     * 
     * @return bool 
     */
    bool process_of_msg(const ofp_message & msg, const time_t now, std::ostream& out);

    std::string msHost; /// this need to set separtely
    
    std::string msPort;/// this need to set separtely
    
    volatile int mClientSocket = 0; /// client socket - TCP/SSL
    
    const time_t mCreationTime; /// when a connection was attached

    time_t mLastBackOffTime = 0; /// when backoff was increased

    time_t mLastQuestionedTime = 0;/// when timeoutInterval examined

    time_t mLastActivityTime = 0;/// last a packet was sent or recieved

    uint32_t mPacketsSent = 0;/// managed by a decorated surrogate class derived from this

    uint32_t mPacketsReceived = 0;/// managed by a decorated surrogate class derived from this

    bool mbHelloSend = false; /// we have sent hello
    bool mbHelloRecieved = false;/// we got hello
    uint32_t mCurRetries = 0;/// current retires in start state

public:
    /**
     * memory owned
     * 
     * @author sraihan ()
     */
    OfConnStateManager* mpStateMgr = nullptr;/// Implements state pattern

    /**
     * memory owned
     * 
     * @author sraihan ()
     */
    AbstractParser * mpParser = nullptr;/// implements decorated process_of_msg

    /**
     * As per strongly connected components inline with RAII parent 
     * class reference 
     * @author sraihan ()
     */
    OfConnManager& mConnMgr;/// Facade pattern for all connections

private:
    /// disable copy CTOR, assignment ops and comparators
    AbstractConn(const AbstractConn& ) = delete;
    AbstractConn& operator = (const AbstractConn& ) =delete;
    AbstractConn(AbstractConn&& ) = delete;
    AbstractConn& operator = (AbstractConn&& ) =delete;
    // use virtual Match func instead
    bool operator == (const AbstractConn& ) = delete;
    bool operator != (const AbstractConn& ) = delete;
};

#endif
