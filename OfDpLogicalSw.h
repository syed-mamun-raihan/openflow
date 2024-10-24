#ifndef OfDpLogicalSw_h_
#define OfDpLogicalSw_h_
/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include <vector>
#include <list>
#include <set>
#include <map>
#include <iostream>
#include <mutex>
#include <memory>
#include <utility>

#include "OfConfig.hpp"
#include "OfDpDefine.hpp"
#include "ofDebug.h"
#include "GenericObservableConfig.hpp"
#include "OfConnManager.hpp"
#include "OfDpController.h"
#include "OfDpGroupTable.h"
#include "OfDpFlowTable.h"

typedef struct OfDpPktInMatch {
    uint16_t priority = 0;
    uint32_t port = 0;
    OfDpPktType pktType = 0;
    bool drop = false;
} OfDpPktInMatch;

/**
 * @class OfDpLogicalSw
 * @author Syed Raihan
 * @date 
 * @file OfDpLogicalSw.h
 * @brief final from GenericObservableConfig<OfDpLogicalSwT>
 * This allows real time update of important
 * notifications like Role, SocketFd on channels without a need for callback
 * from connection manager. Further refinement may be needed for distributed
 * computing
 */
class OfDpLogicalSw final : public GenericObservableConfig<OfDpLogicalSwT> {
public:
    volatile bool mbIsDeleted = false;
    friend class OfDpConfigApi;

    virtual ~OfDpLogicalSw();

    /// Get name Logical Switch
    inline const std::string& getName() const { return (GetConfigObject().name);}

    /// Get protocol version of Logical Switch
    inline OfDpVersion getVersion() const { return (GetConfigObject().version);}

    /// Get protocol version of Logical Switch
    inline OfDpVersion getVersionNegotiated() const { return (GetConfigObject().negotiatedVersion);}

    /// Get datapath ID of Logical Switch
    inline uint64_t getDataPathId() const { return *(uint64_t *)(GetConfigObject().dpId.data());}

    /// Get attached controllers
    const GenericConfigObserver<OfDpControllerAttachmentT>&  GetAttachments() const;

    /// Get port
    inline const GenericConfigObserver<OfDpPortAttachmentT>& GetPorts() const { return mPorts;}

    /// Non constant version must be protected
    inline GenericConfigObserver<OfDpPortAttachmentT>& GetPorts() { return mPorts;}

    /// Add Controller to Logical Switch
    OfDpReturn AddController(const OfDpControllerT&, OfDpControllerAttachmentT&);

    /// Update Controller to Logical Switch
    /// With new IP/Port/TLS info
    OfDpReturn UpdateController(const OfDpControllerT&);

    // Delete Controller from Logical Switch
    OfDpReturn DeleteController(const std::string &name);

    /*
     * @brief
     * We only allow activation data that will not require - reopening socket for that
     * explicitly detach and attach again
     */
    OfDpReturn UpdateAttachment(OfDpControllerAttachmentT &attachment);
    const OfDpControllerAttachmentT* GetAttachmentInfo(const std::string &controllerName) const;

    // Update Group in Logical Switch
    OfDpReturn updateGroup(OfDpGroupT &group, OfDpResponseT &resp);

    // Receive packet
    OfDpReturn receivePkt(uint32_t portIdx,
                          uint32_t phyPortIdx,
                          uint8_t  *pkt,
                          uint32_t pktLen);

    /// Send flow rem msg
    OfDpReturn sendFlowRem(const OfDpFlowRemovedT &info);

    /// Output
    friend std::ostream& operator<<(std::ostream &out, const OfDpLogicalSw &instance);

    /// Update Flow in Logical Switch
    OfDpReturn updateFlow(const OfDpFlowT &flow, OfDpResponseT &response);

    /// Start of Functions for Connection Manager
    /// Get switch config
    bool GetConfig(OfDpSwitchConfigT &info, const OfDpResponseT &response); /// Used by ConnMgr

    /// Get queue config.  Result is allocated by OfDp, since it is variable length.
    bool GetConfig(const uint32_t portIdx,
                   OfDpQueueConfigT *&info, const OfDpResponseT &response);

    /// Set switch config.  PD class should override if fragmentation config supported.
    bool SetConfig(const OfDpSwitchConfigT &info, OfDpResponseT &response);

    /// packet out rcvd to send out
    bool PacketOut(const OfDpPacketOutT &info, OfDpResponseT &response);

    /// Port mod
    bool ResourceMod(const OfDpPortInfoT &info, OfDpResponseT &response);

    /// Flow mod
    bool ResourceMod(OfDpFlowT &info, OfDpResponseT &response);

    /// Group mod
    bool ResourceMod(OfDpGroupT &info, OfDpResponseT &response);

    /// Get features of logical switch.  Result is allocated by OfDp, since it is variable length.
    bool GetFeature(OfDpSwitchFeaturesT&, const OfDpResponseT &response);

    /// Get description stats
    bool GetStat(OfDpDescStatsT&, const OfDpResponseT &response);

    /// Get flow stats
    bool GetStat(const OfDpFlowStatsReqT &statsReq, const OfDpResponseT &response);

    /// Get aggregate flow stats
    bool GetStat(const OfDpAggregateStatsReqT &req, OfDpAggregateStatsT&, const OfDpResponseT &response);

    /// Get table stats
    bool GetStat(OfDpTableStatsT&, const OfDpResponseT &response);

    /// Get port stats
    bool GetStat(const OfDpPortStatsReqT&, OfDpPortStatsT&, const OfDpResponseT &response);

    /// Get queue stats
    bool GetStat(OfDpQueueStatsT&, const OfDpResponseT &response);

    /// Get stats OfDpGroupDescStatsT
    bool GetStat(OfDpGroupDescStatsT&, const OfDpResponseT &response);

    // Get stats OfDpGroupFeatureStatsT
    bool GetStat(OfDpGroupFeatureStatsT&, const OfDpResponseT &response);

    /// Get stats OfDpGroupStatsT
    bool GetStat(OfDpGroupStatsT&, const OfDpResponseT &response);

    /// Get stats OfDpTableFeatureStatsT
    bool GetStat(OfDpTableFeatureStatsT&, const OfDpResponseT &response);

    /// Get Stat OfDpPortListT
    bool GetStat(OfDpPortListT&, const OfDpResponseT &response);

    /// To propagate barrier request
    bool BarrierRequest(const int incoming_fd, const uint32_t incoming_xid);

    /// Notification for negotiated version
    bool VersionNegotiated(const int negotiatedVersion);
    /// end of OfConnMgr Callbacks

    /// LS methods
    void addPktInMatch(OfDpPktInMatch &info);

    void deletePktInMatch(OfDpPktInMatch &info);

    /// return connection manage
    OfConnManager& GetOfConnManager() { return mOfConn;};

    /// return connection manager
    const OfConnManager& GetOfConnManager() const { return mOfConn;};

    /// for what???
    void Init();

    void SetIndexTableRoleToPrimary();

    /// returns table Manager
    inline OfDpFlowTable& getTable() { return _table; }

    /// returns table manager
    inline const OfDpFlowTable& getTable() const { return _table;}

protected:
    /// CTOR is protected - Multi-ton pattern
    OfDpLogicalSw(OfDataPlaneApi *pDPIntf,
                  const OfDpLogicalSwT &config);

    // Attach Controller from Logical Switch
    OfDpReturn AttachController(const OfDpControllerT &curController,
                                OfDpControllerAttachmentT &curData);

    OfDpFlowTable _table;

    OfDpGroupTable _groupTable;

    GenericConfigObserver<OfDpPortAttachmentT> mPorts; /// Saves attached ports

    OfConnManager mOfConn; /// OpenFlow Connection manager

    OfDataPlaneApi *mpDataPlaneApi = 0;

    // pktIn tracker
    std::multimap<uint16_t, OfDpPktInMatch *> _pktInMatch;

    OfDpLogicalSw(const OfDpLogicalSw&) = delete;
    OfDpLogicalSw(OfDpLogicalSw&&) = delete;
    
}; // end of OfDpLogicalSw


#endif
