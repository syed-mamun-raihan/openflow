#ifndef _CIENAOFCLIENT_H
#define _CIENAOFCLIENT_H
/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include <iostream>
#include <string>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <cassert>
#include <cstdint>
#include <climits>


#include "OfConfig.hpp"
#include "OfDpDefine.hpp"
#include "ofDebug.h"
#include "GenericObservableConfig.hpp"

class AbstractConn;
struct ofp_message;
class OfDpLogicalSw;
/**
 *@class OfConnManager
 *@author Syed M Raihan
 *@date 
 *@brief It contains controller attachments (@class AbstractConn)
 */
class OfConnManager final {
public:
    OfDpLogicalSw& mLogicalSw;/// Reference to LogicalSwitch
    volatile bool mbThreadDied = false;
    uint64_t mMaxGenerationId = 0;/// Controller provides for Role change
    /**
     * CTOR Incoming/Outgoing controller message Translator
     *
     * @param msngr
     */
    OfConnManager(OfDpLogicalSw& msngr);

    /**
     * DTOR Incoming/Outgoing controller message Translator
     *
     * @param msngr
     */
    ~OfConnManager();

    /**
     * @brief Run the state machine for all attached connections
     * thread entry points
     * @param none
     * @return void
     **/
    void operator()(void);

    /**
     * @version 1.0+
     *
     * @param info
     *
     * @return uint32_t
     * number of controllers
     */
    uint32_t PacketIn(const OfDpPacketInT & info);

    /**
     * @version 1.0+
     *
     * @param info
     *
     * @return uint32_t
     * number of controllers
     */
    uint32_t  PortStatus(const OfDpPortStatusT & info);

    /**
     * @version 1.0+
     *
     * @param info
     *
     * @return uint32_t
     * number of controllers
     */
    uint32_t  FlowRemoved(const OfDpFlowRemovedT & info);

    bool GetPacketStat(const std::string& host, const std::string& port, uint32_t& rcvdPkts, uint32_t& sendPkts) const;

    /**
     * @version 1.0+
     *
     * @param fd
     * @param xid
     *
     * @return bool
     */
    virtual bool SendBarrierReply( const int fd, const uint32_t xid );

    /**
     * @version 1.0+
     *
     * @param info
     * @param response
     *
     * @return bool
     */
    virtual bool GetFeatureReply(const OfDpSwitchFeaturesT& info, const OfDpResponseT& response);

    /**
     * @version 1.0+
     *
     * @param info
     * @param response
     *
     * @return bool
     */
    virtual bool GetConfigReply(const OfDpSwitchConfigT& info, const OfDpResponseT& response);

    /**
     * @version 1.0+
     *
     * @param response
     *
     * @return bool
     */
    virtual bool SetConfigReply(const OfDpResponseT& response);

    /**
     * @version 1.0+
     *
     * @param response
     *
     * @return bool
     */
    virtual bool PacketOutReply(const OfDpResponseT& response);

    /**
     * @version 1.0+
     *
     * @param info
     * @param response
     *
     * @return bool
     */
    virtual bool ResourceModReply(const OfDpFlowT & info, const OfDpResponseT& response);

    /**
     * @version 1.2+
     *
     * @param info
     * @param response
     *
     * @return bool
     */
    virtual bool ResourceModReply(const OfDpGroupT & info, const OfDpResponseT& response);

    /**
     * @version 1.0+
     *
     * @param info
     * @param response
     *
     * @return bool
     */
    virtual bool ResourceModReply(const OfDpPortInfoT & info, const OfDpResponseT& response);

    /**
     * @version 1.0+
     *
     * @param stats
     * @param response
     *
     * @return bool
     */
    virtual bool GetStatReply( const OfDpDescStatsT &stats, const OfDpResponseT& response);

    /**
     * @version 1.0+
     *
     * @param stats
     * @param response
     *
     * @return bool
     */
    virtual bool GetStatReply( const OfDpFlowStatsT &stats, const OfDpResponseT& response);

    /**
     * @version 1.0+
     *
     * @param stats
     * @param response
     *
     * @return bool
     */
    virtual bool GetStatReply( const OfDpAggregateStatsT &stats, const OfDpResponseT& response);

    /**
     * @version 1.0+
     *
     * @param stats
     * @param response
     *
     * @return bool
     */
    virtual bool GetStatReply( const OfDpTableStatsT &stats, const OfDpResponseT& response);

    /**
     * @version 1.0+
     *
     * @param stats
     * @param response
     *
     * @return bool
     */
    virtual bool GetStatReply( const OfDpPortStatsT &stats, const OfDpResponseT& response);

    /**
     * @version 1.0+
     *
     * @param stats
     * @param response
     *
     * @return bool
     */
    virtual bool GetStatReply(const OfDpQueueStatsT &stats, const OfDpResponseT& response);

    /**
     *  @version 1.2+
     *
     * @param info
     * @param response
     *
     * @return bool
     */
    virtual bool GetConfigReply( const OfDpQueueConfigT& info, const OfDpResponseT& response);

    /**
     * @version 1.2+
     *
     * @param info
     * @param response
     *
     * @return bool
     */
    virtual bool GetStatReply(const OfDpGroupDescStatsT &info, const OfDpResponseT& response);

    /**
     * @version 1.2+
     *
     * @param info
     * @param response
     *
     * @return bool
     */
    virtual bool GetStatReply(const OfDpGroupFeatureStatsT &info, const OfDpResponseT& response);

    /**
     * @version 1.2+
     *
     * @param info
     * @param response
     *
     * @return bool
     */
    virtual bool GetStatReply(const OfDpGroupStatsT &info,  const OfDpResponseT& response);

    /**
     * @version 1.3+
     *
     * @param stats
     * @param response
     *
     * @return bool
     */
    virtual bool GetStatReply( const OfDpTableFeatureStatsT &stats, const OfDpResponseT& response);

    /**
     * @version 1.3+
     *
     * @param stats
     * @param response
     *
     * @return bool
     */
    virtual bool GetStatReply( const OfDpPortListT &stats, const OfDpResponseT& response);

    /**
     * true for success else false
     *
     * @param attachment
     * @param curController
     * @param now
     *
     * @return bool
     */
    bool AddSSLController(const OfDpControllerAttachmentT& attachment,
                          const OfDpControllerT& curController ,
                          const time_t now = time(0));

    /**
     * true for success else false
     *
     * @param attachment
     * @param curController
     * @param now
     *
     * @return bool
     */
    bool AddController(const OfDpControllerAttachmentT& attachment,
                       const OfDpControllerT& curController ,
                       const time_t now = time(0));
    /**
     * deletes a connection
     * true for success else false
     *
     * @param controller name
     *
     * @return bool
     */
    bool DelController(const std::string& name);
    /**
     * \brief print attachment informations
     */
    friend std::ostream& operator << (std::ostream& out, const OfConnManager& obj);

    /**
     * Call this function when Role Request is made by one controller
     * It does not validate any pre/post-requisites
     */
    void ResetMastersToSlave();
    /**
     * Find the connection and Update's statemachine parameters
     *
     * @param config
     * @param fd
     *
     * @return bool
     */
    bool UpdateAttachment(OfDpControllerAttachmentT& config);
    /**
     * Find the connections size
     *
     * @param none
     *
     * @return size of attachments
     */
    inline size_t size() const { return mpVconn.size();}

    /**
     * Get the connection by name
     *
     * @param none
     *
     * @return index
     */
    inline const OfDpControllerAttachmentT* Get(const std::string& controllerName) const
    { return static_cast<const OfDpControllerAttachmentT*>(mpVconn[controllerName]);}

    /**
      * Get the connection by name
      *
      * @param none
      *
      * @return index
      */
    inline const GenericConfigObserver<OfDpControllerAttachmentT>& GetAttachments() const { return mpVconn;}

protected:

    GenericConfigObserver<OfDpControllerAttachmentT> mpVconn;/// attachments of controllers

    /**
     * Get the connection object
     *
     * @param Attachment&
     *
     * @return AbstractConn&
     */
    AbstractConn& GetVconn(OfDpControllerAttachmentT&) ;

    /**
     * Get the connection object
     *
     * @param Attachment&
     *
     * @return AbstractConn&
     */
    const AbstractConn& GetVconn(const OfDpControllerAttachmentT&) const;


private:
    OfConnManager(const OfConnManager&) = delete;
    OfConnManager(OfConnManager&&) = delete;
    OfConnManager& operator =(const OfConnManager&) = delete;
    OfConnManager& operator =(OfConnManager&&) = delete;
};
#endif
