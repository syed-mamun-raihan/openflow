/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
// For logging
#include <cstdio>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <boost/range/adaptor/map.hpp>
#include <netinet/in.h>

#include "OfConfig.hpp"
#include "OfDpDefine.hpp"
#include "ofDebug.h"
#include "OfDpLogicalSw.h"
#include "OfConnManager.hpp"
#include "OfDpFlowTable.h"
#include "OfDpConfigApi.h"
#include "OfDataPlaneApi.h"
uint32_t gDbgMask = DBG_ERR_OF; //


extern "C"
{
/**
 * from debugger we can check current level
 * 
 * @author sraihan ()
 * 
 * @return uint32_t 
 */
uint32_t get_dbgmask() {
    std::cout << " cofa debugMask: " << std::hex << gDbgMask << std::dec << std::endl;
    return gDbgMask;
}

/**
 * change Cofa logging level and module
 * 
 * @author sraihan ()
 * 
 * @param val 
 */
void SetFlowDebug() {
    gDbgMask = (DBG_ERR_OF | DBG_DP | DBG_FLOW_ENTRY | DBG_FLOW_TABLE);
}

void ResetFlowDebug() {
    gDbgMask &= ~(DBG_DP | DBG_FLOW_ENTRY | DBG_FLOW_TABLE);
}

/**
 * Enabling OFParserLogging will disable other gDbgMask based logging
 * This logging kind of invasive not suitable for high volume or continuous logging
 */
void EnableOFParserDebug(void) { gDbgMask = DBG_ERR_OF|DBG_PARSER_PARSE;}
void DisableOFParserDebug(void) { gDbgMask &= ~(DBG_PARSER_PARSE); }
void SetCofaDebugMask(uint32_t val) { gDbgMask = val|DBG_ERR_OF;}
void ReseCofatDebugMask() { gDbgMask = DBG_ERR_OF; }

}

OfDpLogicalSw::OfDpLogicalSw(OfDataPlaneApi *pDPIntf,
                             const OfDpLogicalSwT &config) :
    GenericObservableConfig<OfDpLogicalSwT>(config)
    , _table(new OfTableTypeSingle(), pDPIntf, static_cast<OfDpLogicalSwT*>(this))
    , _groupTable(pDPIntf, this)
    , mOfConn(*this)
    , mpDataPlaneApi(pDPIntf) {
    DBG_TRACE_ENTER;
    /// Start Connection thread using lambda expression
    std::thread LSThread([this]() {this->mOfConn(); }); /// self managed
    LSThread.detach();
    std::cout << " LS Thread started for " << GetConfigObject().name << "\n";
    DBG_TRACE_EXIT;
}

OfDpLogicalSw::~OfDpLogicalSw() {
    DBG_TRACE_ENTER;
    mbIsDeleted = true;

    // Clean it all up
    for (const auto& i : _pktInMatch | boost::adaptors::map_values ) {
        delete i;
    }

    while (!mOfConn.mbThreadDied) sleep(1); /// not available in 4.7.0 - std::this_thread::yield();

    DBG_TRACE_EXIT;
}



/**
 * @brief
 * Add Controller to Logical Switch
 *
 * @param controller
 *
 * @return OfDpReturn
 */
OfDpReturn OfDpLogicalSw::AddController(const OfDpControllerT &controller,
                                        OfDpControllerAttachmentT &attachment) {
    DBG_TRACE_ENTER;

    // See if there are any free entries.
    if (GetOfConnManager().size() >= OFDP_MAX_CONTROLLER) {
        DBG_TRACE_RET(OFDP_ERR_NO_FREE_ENTRY);
    }

    const OfDpReturn ret = AttachController(controller, attachment);
    DBG_TRACE_RET(ret);
}


/**
 * @brief
 * Updates only IP and port and TLS params
 *
 * @param controller
 *
 * @return OfDpReturn
 */
OfDpReturn OfDpLogicalSw::UpdateController(const OfDpControllerT &controller) {
    DBG_TRACE_ENTER;

    // Find entry
    const OfDpControllerAttachmentT *activation = GetOfConnManager().Get(controller.name);
    // Entry doesn't exist
    if (!activation) {
        DBG_TRACE_RET(OFDP_ENTRY_NOT_EXIST);
    }
    const OfDpControllerAttachmentT save_data = *activation;
    // Delete from OfCiena.
    if (!mOfConn.DelController(activation->name)) {

        return (OFDP_ERROR_UNKNOWN);
    }


    // Then, add to OFSwitch, get FD back.
    if (strlen(controller.cafile.c_str()) &&
        strlen(controller.certfile.c_str()) &&
        strlen(controller.privfile.c_str()) &&
        mOfConn.AddSSLController(save_data,
                                 controller)) {
        DBG_TRACE_RET(OFDP_OK);

    } else if (mOfConn.AddController(save_data,
                                     controller)) {
        DBG_TRACE_RET(OFDP_OK);
    }
    return (OFDP_ERROR_UNKNOWN);
}

/**
 * Delete Controller from Logical Switch
 *
 * @param controllerName
 *
 * @return OfDpReturn
 */
OfDpReturn OfDpLogicalSw::DeleteController(const std::string &controllerName) {
    DBG_TRACE_ENTER;

    // Find entry
    const OfDpControllerAttachmentT *curData = GetOfConnManager().Get(controllerName);
    // Entry doesn't exist
    if (!curData) {
        DBG_TRACE_RET(OFDP_ENTRY_NOT_EXIST);
    }

    // Delete from OfCiena.
    if (!mOfConn.DelController(controllerName)) {

        DBG_TRACE_RET(OFDP_ERROR_UNKNOWN);
    }
    DBG_TRACE_RET(OFDP_OK);
}

/**
 * @brief
 * Attach Controller from OfDpConfigApi to this Logical Switch
 *
 * @param controllerName
 *
 * @return OfDpReturn
 */
OfDpReturn OfDpLogicalSw::AttachController(const OfDpControllerT &curController, OfDpControllerAttachmentT &curData) {
    DBG_TRACE_ENTER;
    bool ret = true;

    if (strlen(curController.privfile.c_str()) &&
        strlen(curController.cafile.c_str()) &&
        strlen(curController.certfile.c_str())) {
        // TLS params are default from system
        curData.isTlsSet = true;
        ret = mOfConn.AddSSLController(curData, curController);
    } else {
        curData.isTlsSet = false;
        // Add to OFSwitch, get FD back.
        ret = mOfConn.AddController(curData, curController);
    }

    DBG_TRACE_RET(ret ? OFDP_OK : OFDP_ERROR_UNKNOWN);
}

/// Update Flow in Logical Switch
OfDpReturn OfDpLogicalSw::updateFlow(const OfDpFlowT &flow, OfDpResponseT &response) {
    DBG_TRACE_ENTER;
    OfDpReturn ret;

    // Lookup and send to proper table.
    if (flow.table_id == 0) {
        ret = getTable().updateFlow(flow, response);
        DBG_TRACE_RET(ret);
    }

    DBG_TRACE_RET(OFDP_ERR_TABLE_ID);
}


// Update Group in Logical Switch
OfDpReturn OfDpLogicalSw::updateGroup(OfDpGroupT &group, OfDpResponseT &response) {
    DBG_TRACE_ENTER;
    OfDpReturn ret = _groupTable.updateGroup(group, response);
    DBG_TRACE_RET(ret);
}

void OfDpLogicalSw::addPktInMatch(OfDpPktInMatch &info) {
    DBG_TRACE_ENTER;

    OfDpPktInMatch *newPktIn = new OfDpPktInMatch();
    memcpy(newPktIn, &info, sizeof(OfDpPktInMatch));
    //TODO: Fix it
    //    _pktInMatch[info.priority] = newPktIn;

    DBG_TRACE_EXIT;
}

void OfDpLogicalSw::deletePktInMatch(OfDpPktInMatch &info) {
    DBG_TRACE_ENTER;
    std::multimap<uint16_t, OfDpPktInMatch *>::iterator pktInMatchIt;
    OfDpPktInMatch *remEntry = NULL;

    pktInMatchIt = _pktInMatch.begin();

    while (pktInMatchIt != _pktInMatch.end()) {
        if (memcmp(pktInMatchIt->second, &info, sizeof(OfDpPktInMatch)) == 0) {
            remEntry = pktInMatchIt->second;
            _pktInMatch.erase(pktInMatchIt);
            delete remEntry;
            DBG_TRACE_EXIT;
            return;
        }
        pktInMatchIt++;
    }

    DBG_TRACE_EXIT;
}

// Receive packet
OfDpReturn OfDpLogicalSw::receivePkt(uint32_t portIdx,
                                     uint32_t phyPortIdx,
                                     uint8_t  *pkt,
                                     uint32_t pktLen) {
    DBG_TRACE_ENTER;

    OfDpReturn ret = OFDP_OK;
    OfDpPktType pktType = OFDP_PKT_UNKNOWN;
    OfDpPacketInT pktIn;
    bool foundMatch = false;
    std::multimap<uint16_t, OfDpPktInMatch *>::iterator pktInMatchIt;

    // Identify pkt type
    const uint16_t *ptr16 = (uint16_t *)(pkt + 12);
    const uint16_t *ptr32_16 = (uint16_t *)(pkt + 16); // ether type in case of 802.1q frame
    if (ntohs(*ptr16) == OFDP_8021_ETYPE) { // 802.1q
        ptr16 = ptr32_16; // EtherType for 802.1q frames
        DBG(DBG_DETAIL | DBG_LOGICAL_SW, ("vlan tagged pkt, pgid=%u\n",(unsigned int)portIdx));
    }
    switch (ntohs(*ptr16)) {
    case OFDP_LLDP_ETYPE:
        if (OFDP_IS_LLDP(pkt)) {
            pktType = OFDP_PKT_LLDP;
        } else if (OFDP_IS_OFDP(pkt)) {
            pktType = OFDP_PKT_OFDP;
        }
        break;
    case OFDP_BDDP_ETYPE:
        pktType = OFDP_PKT_BDDP;
        break;
    case 0: //Junk
        DBG(DBG_DETAIL | DBG_LOGICAL_SW, ("junk pkt, pgid=%u, ethertype=0x%04x, pktType=0x%x - not sending ptkin.\n",
                                          (unsigned int)portIdx,(unsigned int)ntohs(*ptr16),
                                          (unsigned int)pktType));

        DBG_TRACE_RET(ret);
        break;
    default:
        pktType = OFDP_PKT_UNKNOWN;
        break;
    }

    DBG(DBG_DETAIL | DBG_LOGICAL_SW, ("recv pkt, pgid=%u, ethertype=0x%04x, pktType=0x%x\n",
                                      (unsigned int)portIdx,(unsigned int)ntohs(*ptr16),
                                      (unsigned int)pktType));

    if (_pktInMatch.end() != _pktInMatch.begin()) {
        pktInMatchIt = _pktInMatch.end();
        pktInMatchIt--;

        while (pktInMatchIt != _pktInMatch.begin()) {
            if ((pktInMatchIt->second->pktType & pktType) &&
                ((pktInMatchIt->second->port == OFDP_OFPP_ALL) ||
                 (pktInMatchIt->second->port == portIdx))) {
                foundMatch = true;
                break;
            }

            pktInMatchIt--;
        }

        if (!foundMatch && (pktInMatchIt == _pktInMatch.begin())) {
            if ((pktInMatchIt->second->pktType & pktType) &&
                ((pktInMatchIt->second->port == OFDP_OFPP_ALL) ||
                 (pktInMatchIt->second->port == portIdx))) {
                foundMatch = true;
            }
        }
    }

    if (pktType &&
        (!foundMatch ||
         (!pktInMatchIt->second->drop))) {
        DBG(DBG_DETAIL | DBG_LOGICAL_SW, ("send pkt to controller, pgid=%u, ethertype=0x%04x, pktType=0x%x\n",
                                          (unsigned int)portIdx,(unsigned int)ntohs(*ptr16),
                                          (unsigned int)pktType));
        // Copy pkt and other info.
        pktIn.buffer_id = -1;
        //        pktIn.total_len = (pktLen > miss_send_len) ? miss_send_len : pktLen;
        pktIn.total_len = pktLen;
        pktIn.in_port = portIdx;
        if (!foundMatch) {
            pktIn.reason = OFDP_OFPR_NO_MATCH;
        } else {
            pktIn.reason = OFDP_OFPR_ACTION;
        }
        memcpy(pktIn.message, pkt, pktIn.total_len);

        // Send to controller.
        mOfConn.PacketIn(pktIn);
    } else if (foundMatch &&
               pktInMatchIt->second->drop) {
        DBG(DBG_DETAIL | DBG_LOGICAL_SW, ("dropping control pkt due to flow, pgid=%u, ethertype=0x%04x, pktType=0x%x\n",
                                          (unsigned int)portIdx,(unsigned int)ntohs(*ptr16),
                                          (unsigned int)pktType));
    }

    DBG_TRACE_RET(ret);
}


// Send flow rem msg
OfDpReturn OfDpLogicalSw::sendFlowRem(const OfDpFlowRemovedT &info) {
    DBG_TRACE_ENTER;
    uint32_t ret;

    ret = mOfConn.FlowRemoved(info);

    DBG_TRACE_RET(ret ? OFDP_OK : OFDP_ERROR_UNKNOWN);
}

/**
 * Pulls Port info from Dataplane Api
 * 
 * @author sraihan (7/31/2014)
 * 
 * @param response 
 * 
 * @return bool 
 */
bool OfDpLogicalSw::GetStat(OfDpPortListT&, const OfDpResponseT &response) {
    DBG_TRACE_ENTER;

    // This is only applicable if we're running version 1.3+
    if (GET_VERSION(response.msg) < OFDP_VERSION_V4) {
        DBG_TRACE_RET_SYNC(OFDP_UNSUPPORTED_VERSION, const_cast<OfDpResponseT&>(response).return_code);
    }

    OfDpPortListT info;

    OfDpResponseT reply(response);

    unsigned num_ports = this->GetPorts().size();

    std::lock_guard<std::recursive_mutex> lock(this->GetPorts().mutex());

    for ( const auto& i : GetPorts()| boost::adaptors::map_keys ) {
        info.ports[0].port_no = i;
        OfDpConfigApi::getInstance()->getDataPlaneApiPtr()->GetPort(info.ports[0]);
        info.flags = (--num_ports >= 1) ? 1 : 0;
        if (!this->mOfConn.GetStatReply(info, reply)) break;
    }

    // return false for async stats call.
    return false;
}


bool OfDpLogicalSw::BarrierRequest(const int incoming_fd, const uint32_t incoming_xid) {
    DBG(DBG_DETAIL | DBG_LOGICAL_SW, (" stub code - %i, %u\n", incoming_fd, incoming_xid));
    return true;
}

/// Get OfDpSwitchConfigT
bool OfDpLogicalSw::GetConfig(OfDpSwitchConfigT &info, const OfDpResponseT &response) {
    DBG_TRACE_ENTER;

    info.flags = GetConfigObject().flags;
    info.miss_send_len = GetConfigObject().miss_send_len;

    DBG_TRACE_RET_SYNC(OFDP_OK, const_cast<OfDpResponseT&>(response).return_code);
}

/// Set OfDpSwitchConfigT
bool OfDpLogicalSw::SetConfig(const OfDpSwitchConfigT &info,
                              OfDpResponseT &response) {
    DBG_TRACE_ENTER;

    // We only currently support normal operations for frag, so
    // we just ignore changes to flags.  But, save miss_send_len
    GetConfigObject().miss_send_len = info.miss_send_len;

    DBG_TRACE_RET_SYNC(OFDP_OK, response.return_code);
}

/// OfDpPacketOutT
bool OfDpLogicalSw::PacketOut(const OfDpPacketOutT &info, OfDpResponseT &response) {
    DBG_TRACE_ENTER;
    OfDpReturn ret = OFDP_OK;


    DBG_TRACE_RET_SYNC(ret, response.return_code);
}

/// modify OfDpPortInfoT
bool OfDpLogicalSw::ResourceMod(const OfDpPortInfoT &info, OfDpResponseT &response) {
    DBG_TRACE_ENTER;
    OfDpReturn ret = OFDP_OK;

    DBG_TRACE_RET_SYNC(ret, response.return_code);
}

/// Flow mod - OfDpFlowT
bool OfDpLogicalSw::ResourceMod(OfDpFlowT &info, OfDpResponseT &response) {
    DBG_TRACE_ENTER;
    OfDpReturn ret = OFDP_OK;
    /// get in_port_name and out_port_name
    {
        std::lock_guard<std::recursive_mutex> lock(GetPorts().mutex());
        const OfDpPortAttachmentT *out_p = GetPorts()[info.out_port];
        if (out_p) info.out_port_name = out_p->name;
        const OfDpPortAttachmentT *in_p = GetPorts()[info.match.in_port];
        if (in_p) info.in_port_name = in_p->name;
    }
    ret = updateFlow(info, response);

    DBG_TRACE_RET_SYNC(ret, response.return_code);
}

/// Group mod - OfDpGroupT
bool OfDpLogicalSw::ResourceMod(OfDpGroupT &info, OfDpResponseT &response) {
    DBG_TRACE_ENTER;
    OfDpReturn ret = OFDP_OK;

//    ret = updateGroup( info , response);

    DBG_TRACE_RET_SYNC(ret, response.return_code);
}

/// Multi - version Feature return we may need our own Feature defination and
/// Get Translated in parser
bool OfDpLogicalSw::GetFeature(OfDpSwitchFeaturesT&, const OfDpResponseT &response) {
    DBG_TRACE_ENTER;
    OfDpSwitchFeaturesT info;
    // Fill in the values.
    info.datapath_id = getDataPathId();
    info.n_buffers = 0; /// We do not provide local buffers yet
    info.n_tables = 1;

    // OFDP_FLOW_STATS, OFDP_TABLE_STATS, OFDP_PORT_STATS, OFDP_STP,
    // OFDP_RESERVED, OFDP_IP_REASM, OFDP_QUEUE_STATS, OFDP_ARP_MATCH_IP
    // OFDP_VPORT_TABLE (1.0/ext), OFDP_PORT_BLOCKED (1.3)
    // First, cover capabilities for all versions

    info.capabilities = OFDP_FLOW_STATS | OFDP_PORT_STATS | OFDP_QUEUE_STATS | OFDP_TABLE_STATS
        | OFDP_PORT_BLOCKED;

    this->mOfConn.GetFeatureReply(info, response);

    // return false for async stats call.
    return false;
}

/// Get OfDpDescStatsT
bool OfDpLogicalSw::GetStat(OfDpDescStatsT &stats, const OfDpResponseT &response) {
    DBG_TRACE_ENTER;
    std::string hwDesc, swDesc, serialNum;

    strncpy(stats.mfr_desc, OFDP_MFR_DESC, OFDP_DESC_STR_LEN - 1);
    strncpy(stats.dp_desc, this->getName().c_str(), OFDP_DESC_STR_LEN - 1);

    DBG_TRACE_RET_SYNC(OFDP_OK, const_cast<OfDpResponseT&>(response).return_code);
}

/// Get OfDpFlowStatsReqT
bool OfDpLogicalSw::GetStat(const OfDpFlowStatsReqT &statsReq, const OfDpResponseT &response) {
    DBG_TRACE_ENTER;
    if ((statsReq.table_id > 1) && (statsReq.table_id != OFDP_TABLE_ALL)) {
        DBG_TRACE_RET_SYNC(OFDP_ERR_TABLE_ID, const_cast<OfDpResponseT&>(response).return_code);
    }

    /// TODO: Flow Table need mutex
    this->getTable().getFlowStats(statsReq,
                                  this->mOfConn,
                                  const_cast<OfDpResponseT&>(response));

    // return false for async stats call.
    return false;
}

/// Get OfDpAggregateStatsT
bool OfDpLogicalSw::GetStat(const OfDpAggregateStatsReqT &statsReq, OfDpAggregateStatsT&, const OfDpResponseT &response) {
    DBG_TRACE_ENTER;

    if ((statsReq.table_id > 1) && (statsReq.table_id != OFDP_TABLE_ALL)) {
        DBG(DBG_ERR_OF | DBG_LOGICAL_SW, ("invalid table for stats request, switch=%s, table_id=%u\n",
                                          this->getName().c_str(),(unsigned int)statsReq.table_id));
        DBG_TRACE_RET_SYNC(OFDP_ERR_TABLE_ID, const_cast<OfDpResponseT&>(response).return_code);
    }

    OfDpAggregateStatsT stats;
    const_cast<OfDpResponseT&>(response).return_code = this->getTable().getAggregateStats(statsReq,
                                                                                          stats,
                                                                                          this->mOfConn,
                                                                                          const_cast<OfDpResponseT&>(response));
    // return false for async stats call.
    return false;
}

/// Get OfDpTableStatsT
bool OfDpLogicalSw::GetStat(OfDpTableStatsT &stats, const OfDpResponseT &response) {
    DBG_TRACE_ENTER;

    const uint8_t tableIdx = stats.table_id;

    if ((tableIdx > 0) && (tableIdx != OFDP_TABLE_ALL)) {
        DBG_TRACE_RET_SYNC(OFDP_ERR_TABLE_ID, const_cast<OfDpResponseT&>(response).return_code);
    }

    stats.table_id = tableIdx;
    const_cast<OfDpResponseT&>(response).return_code = this->getTable().getTableStats(stats,
                                                                                      this->mOfConn,
                                                                                      const_cast<OfDpResponseT&>(response));
    // return false for async stats call.
    return false;
}

/// Get OfDpTableFeatureStatsT
bool OfDpLogicalSw::GetStat(OfDpTableFeatureStatsT &stats, const OfDpResponseT &response) {
    DBG_TRACE_ENTER;
    const uint8_t tableIdx = stats.feature.table_id;

    if ((tableIdx > 0) && (tableIdx != OFDP_TABLE_ALL)) {
        DBG_TRACE_RET_SYNC(OFDP_ERR_TABLE_ID, const_cast<OfDpResponseT&>(response).return_code);
    }
    const_cast<OfDpResponseT&>(response).return_code = this->getTable().getTableFeature(stats.feature);
    // Start filling in stats info.  Name is filled in by Logical Switch.
    stats.flags = 0;
    this->mOfConn.GetStatReply(stats, response); // last reply
                                                 // return false for async stats call.
    return false;
}

/// Get OfDpPortStatsT
bool OfDpLogicalSw::GetStat(const OfDpPortStatsReqT &statsReq, OfDpPortStatsT&, const OfDpResponseT &response) {
    DBG_TRACE_ENTER;

    OfDpPortStatsT stats;
    if (statsReq.port_no == OFDP_OFPP_ALL ||
        statsReq.port_no == OFDP_OFPP_ANY) {

        unsigned num_ports = this->GetPorts().size();
        std::lock_guard<std::recursive_mutex> lock(this->GetPorts().mutex());
        for ( const auto& pPort : GetPorts()| boost::adaptors::map_values ) {
            stats.port_no = pPort->port_no; // Port number
            stats.flags = (--num_ports >= 1) ? 1 : 0;
//            OfDpConfigApi::getInstance()->getDataPlaneApiPtr()->GetPortStat(stats.)
            if (!this->mOfConn.GetStatReply(stats, response)) break;
        }
    } else {
        stats.port_no = statsReq.port_no; // Port number
        stats.flags = 0;
        this->mOfConn.GetStatReply(stats, response);
    }
//        this->_resourceTable.getPortStats( statsReq, stats, this->mOfConn, response);
// return false for async stats call.
    return false;
}

/// Get OfDpQueueStatsT
bool OfDpLogicalSw::GetStat(OfDpQueueStatsT&, const OfDpResponseT&) {
    DBG_TRACE_ENTER;
#if 0
    const OfDpReturn ret = _resourceTable.getQueueStats(stats, mOfSwitch, response);
#endif
    // return false for async stats call.
    return false;
}

/// get OfDpGroupDescStatsT
bool OfDpLogicalSw::GetStat(OfDpGroupDescStatsT&, const OfDpResponseT&) {
    DBG_TRACE_ENTER;
#if 0
    _groupTable.getStat(info, mOfConn, response);
    // return false for asynchronous reply
#endif
    return false;
}

/// get OfDpGroupFeatureStatsT
bool OfDpLogicalSw::GetStat(OfDpGroupFeatureStatsT&, const OfDpResponseT&) {
    DBG_TRACE_ENTER;
#if 0
    OfDpReturn ret = _groupTable.getFeatures(info.feature);
#endif
    // return false for asynchronous reply
    return false;
}

// get stat
bool OfDpLogicalSw::GetStat(OfDpGroupStatsT&, const OfDpResponseT&) {
    DBG_TRACE_ENTER;
#if 0
    _groupTable.getStat(info, mOfConn, response);
#endif
    // return false for asynchronous reply
    return false;
}

// Get queue config.  Result is allocated by OfDp, since it is variable length.
bool OfDpLogicalSw::GetConfig(const uint32_t /*portIdx*/,
                              OfDpQueueConfigT *&, const OfDpResponseT&) {
    DBG_TRACE_ENTER;

//    const OfDpReturn ret = _resourceTable.getQueueConfig( portIdx, info, mOfSwitch, response );

    // return false for asynchronous reply
    return false;
}


// Output
std::ostream& operator<<(std::ostream &out, const OfDpLogicalSw &instance) {
    // Dump logical switch info
    out << "\n************* LogicalSwitch (" <<  instance.getName() << ")************\n"
        << "Version=" << to_string(instance.getVersion())
        << ", DPID = " << std::hex << *(uint64_t *)(instance.dpId.data()) << std::dec
        << ", Connections := \n" << instance.mOfConn
        << std::endl;

    out << "Ports:\n";
    for (const auto& i : instance.GetPorts() | boost::adaptors::map_values) {
        out << " port: " << i->GetConfigObject().name << " (" << i->GetConfigObject().port_no << ")\n";
    }
    // Dump tables
    out << " Flow Table: " << instance.getTable();
/*    out << "\n Packet-In Match Table: " << std::endl;

    for ( const auto &pktInMatchIt : instance._pktInMatch | boost::adaptors::map_values){
        out << " Priority=" << pktInMatchIt->priority << ", Port=" <<
        pktInMatchIt->port << ", ethType=" <<
        pktInMatchIt->pktType << ", Drop=" <<
        pktInMatchIt->drop << std::endl;

    }
*/
    return (out);
}

OfDpReturn OfDpLogicalSw::UpdateAttachment(OfDpControllerAttachmentT &attachment) {
    if (GetOfConnManager().UpdateAttachment(attachment)) {
        return OFDP_OK;
    }
    return OFDP_ENTRY_NOT_EXIST;
}

const OfDpControllerAttachmentT* OfDpLogicalSw::GetAttachmentInfo(const std::string &controllerName) const {
    return GetOfConnManager().Get(controllerName);
}

bool OfDpLogicalSw::VersionNegotiated(const int negotiatedVersion) {
    this->negotiatedVersion = static_cast<OfDpVersion>(negotiatedVersion);
    return true;
}


void OfDpLogicalSw::SetIndexTableRoleToPrimary() {
    getTable().SetIndexTableRoleToPrimary();
}

// Get attached controllers
const GenericConfigObserver<OfDpControllerAttachmentT>&  OfDpLogicalSw::GetAttachments() const { return mOfConn.GetAttachments();}

