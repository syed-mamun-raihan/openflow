/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
// For logging
#include <cstdio>
#include <string.h>
#include <sstream>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/assign.hpp>

#include "OfDpFlowTable.h"
#include "OfDataPlaneApi.h"
#include "ofDebug.h"
#include "OfConnManager.hpp"


using namespace boost::assign;
using namespace boost::adaptors;
#define BOOST_RESULT_OF_USE_DECLTYPE

/*
 * Flow number 1 to 1000 are reserved for internal flows.
 */
#define startFlowNum 1001
OfTableTypeSingle::OfTableTypeSingle(const uint8_t id, const uint32_t maxEntries) {
    assert(id == 0); // it is not a multitable
    setMaxEntries(maxEntries);

    _tableFeature.actions = ((1 << OFDP_OUTPUT) |
                                 (1 << OFDP_SET_VLAN_VID) |
                                 (1 << OFDP_SET_VLAN_PCP) |
                                 (1 << OFDP_POP_VLAN) |
                                 (1 << OFDP_PUSH_VLAN_VID) |
                                 (1 << OFDP_SET_MPLS_LABEL) |
                                 (1 << OFDP_POP_MPLS) |
                                 (1 << OFDP_PUSH_MPLS) |
                                 (1 << OFDP_SET_MPLS_TTL)
                            );

    //1.3 staffs
    _tableFeature.write_actions = ((1 << OFDP_OUTPUT) |
                                       (1 << OFDP_POP_VLAN) |
                                       (1 << OFDP_PUSH_VLAN_VID) |
                                       (1 << OFDP_POP_MPLS) |
                                       (1 << OFDP_PUSH_MPLS) |
                                       (1 << OFDP_DEC_MPLS_TTL) |
                                       (1 << OFDP_GROUP)
                                  );


    _tableFeature.write_set_fields = ((1 << OFDP_SET_VLAN_VID) |
                                          (1 << OFDP_SET_VLAN_PCP) |
                                          (1 << OFDP_SET_DL_SRC) |
                                          (1 << OFDP_SET_DL_DST) |
                                          (1 << OFDP_SET_MPLS_TTL) |
                                          (1 << OFDP_SET_MPLS_LABEL));


    _tableFeature.match_fields = (OFDP_IN_PORT | OFDP_DL_VLAN | OFDP_DL_TYPE |
                                      OFDP_NW_SRC | OFDP_NW_DST | OFDP_DL_VLAN_PCP |
                                      OFDP_MPLS_L1);

    _tableFeature.wildcard_fields = (OFDP_IN_PORT | OFDP_DL_VLAN | OFDP_DL_TYPE |
                                         OFDP_NW_SRC | OFDP_NW_DST | OFDP_DL_VLAN_PCP |
                                         OFDP_MPLS_L1);

    _tableFeature.apply_set_fields = _tableFeature.write_set_fields;
    _tableFeature.apply_actions = _tableFeature.write_actions;
    _tableFeature.instructions = (1 << OFDP_WRITE_INSTRUCTION);
}

OfDpFlowTable::OfDpFlowTable(OfTableTypeProfileBase *tableProfile,
                             OfDataPlaneApi *const pDPIntrf,
                             const OfDpLogicalSwT *ls) :
    _ttpObj(tableProfile),
    _mpDPInterface(pDPIntrf),
    mpCofaLS(const_cast<OfDpLogicalSwT *>(ls)),
    _flowCount(0) {
    for (uint32_t i = startFlowNum; i < maxFlowNum; ++i) {
        _freeFlowIdQueue.push(i);
    }
}


OfDpFlowTable::~OfDpFlowTable() {
    DBG_TRACE_ENTER;
    // to avoid any variable shadowing with globals
    if (this->_ttpObj) delete this->_ttpObj;
    DBG_TRACE_EXIT;
}

// Add flow
OfDpReturn OfDpFlowTable::_addFlow(const OfDpFlowT &flow,
                                   OfDpResponseT &response) {
    DBG_TRACE(DBG_FLOW_TABLE);
    OfDpReturn ret = OFDP_OK;

    // check for exact match, if found the stats are reset.
    // or delete the old and add the new flow
    // Currently just ignore it.

    std::set<uint32_t> newSet;
    std::set<OfDpFlowT *> rsltSet;
    {
        this->find_exact_with_priority(&flow, rsltSet);

        // if exact search is not empty return ok.
        if (rsltSet.empty() == false) {
            if (flow.flags & OFDP_OFPFF_CHECK_OVERLAP) {
                // Entry exists, return error.
                DBG_TRACE_RET(OFDP_ERR_FLOW_OVERLAP);
            }
            DBG_TRACE_RET(OFDP_OK);
        }

        if (flow.flags & OFDP_OFPFF_CHECK_OVERLAP) {
            this->find_overlap(&flow, rsltSet);

            if (rsltSet.empty() == false) {
                DBG_TRACE_RET(OFDP_ERR_FLOW_OVERLAP);
            }
        }

        // add the flow into the table.
        if (!this->insert(&flow, flow.flow_id)) {
            DBG_TRACE_RET(OFDP_OK);
        }

    }


    if (_mpDPInterface) {
        ret = _mpDPInterface->AddToDb(flow, response);
    }

    DBG_TRACE_RET(ret);
}

// Modify flow (exact)
OfDpReturn OfDpFlowTable::_modifyFlow(const OfDpFlowT &flow,
                                      OfDpResponseT &response) {
    DBG_TRACE(DBG_FLOW_TABLE);
    OfDpReturn ret = OFDP_OK;



    // check for exact match, if found the stats are reset.
    // or delete the old and add the new.
    // Currently just ignore it.

    std::set<OfDpFlowStatsT *> rsltSet;
    {
        this->find_loose(&flow.match, OFDP_OFPP_NONE, OFDP_OFPP_NONE, flow.cookie, flow.cookie_mask, rsltSet);


        // if exact search is not empty return ok.
        if (rsltSet.empty() == false) {
            // add the flow into the table.
            if (_mpDPInterface) {
                ret =  _mpDPInterface->UpdateDb(rsltSet, flow, response);
            }
        } else {
            // add the flow into the table.

            if (this->insert(&flow, flow.flow_id) && _mpDPInterface) {
                ret = _mpDPInterface->AddToDb(flow, response);
            }
        }
    }
    DBG_TRACE_RET(ret);
}

// Modify flow (strict)
OfDpReturn OfDpFlowTable::_modifyStrictFlow(const OfDpFlowT &flow,
                                            OfDpResponseT &response) {

    DBG_TRACE(DBG_FLOW_TABLE);
    OfDpReturn ret = OFDP_OK;

    // check for exact match, if found the stats are reset.
    // or delete the old and add the new.
    // Currently just ignore it.

    std::set<OfDpFlowStatsT *> rsltSet;
    {
        this->find_exact_with_priority(&flow, rsltSet);


        // if exact search is not empty return ok.
        if (rsltSet.empty() == false) {
            // add the flow into the table.
            if (_mpDPInterface) {
                ret = _mpDPInterface->UpdateDb(rsltSet, flow, response);
            }
        } else {
            // add the flow into the table.
            if (this->insert(&flow, flow.flow_id) && _mpDPInterface) {
                ret = _mpDPInterface->AddToDb(flow, response);
            }
        }
    }
    DBG_TRACE_RET(ret);
}

// Delete flow (exact)
OfDpReturn OfDpFlowTable::_deleteFlow(const OfDpFlowT &flow,
                                      OfDpResponseT &response) {
    DBG_TRACE(DBG_FLOW_TABLE);

    OfDpReturn ret = OFDP_OK;

    // check for exact match, if found the stats are reset.
    // or delete the old and add the new.
    // Currently just ignore it.
    std::set<OfDpFlowT *> rsltSet;
    {
        this->find_loose(&flow.match, OFDP_OFPP_NONE, OFDP_OFPP_NONE, flow.cookie, flow.cookie_mask, rsltSet);

        // if exact search is not empty return ok.
        if (rsltSet.empty() == false) {
            std::set<OfDpFlowT *> inSet;
            for(auto i : rsltSet) inSet.insert(i);
            if ((ret = _mpDPInterface->DeleteFlowsFromDb(rsltSet, flow, response)) == OFDP_OK) {
                std::set<uint32_t> outSet;
                this->remove_set(inSet, outSet);
            }
        }
    }
    DBG_TRACE_RET(ret);
}

// Delete flow (strict)
OfDpReturn OfDpFlowTable::_deleteStrictFlow(const OfDpFlowT &flow,
                                            OfDpFlowStatsT &stats,
                                            OfDpResponseT &response,
                                            OfDpFlowRemovedReasonT reason) {
    DBG_TRACE(DBG_FLOW_TABLE);

    // check for exact match, if found the stats are reset.
    // or delete the old and add the new.
    // Currently just ignore it.
    OfDpReturn ret = OFDP_OK;
    std::set<OfDpFlowT *> rsltSet;

    {
        this->find_exact_with_priority(&flow, rsltSet);
        // if exact search is not empty return ok.
        if (rsltSet.empty() == false) {
            std::set<OfDpFlowT *> inSet;
            for(auto i : rsltSet) inSet.insert(i);
            if ((ret = _mpDPInterface->DeleteFlowsFromDb(rsltSet, flow, response)) == OFDP_OK) {
                std::set<uint32_t> outSet;
                this->remove_set(inSet, outSet);
            }
        }
    }
    DBG_TRACE_RET(ret);
}

// Update Flow.
OfDpReturn OfDpFlowTable::updateFlow(const OfDpFlowT &flow,
                                     OfDpResponseT &response) {
    DBG_TRACE(DBG_FLOW_TABLE);

    OfDpReturn ret = OFDP_OK;
    OfDpFlowStatsT stats;
    stats.ls_idx = flow.ls_idx;
    switch (flow.command) {
    case OFDP_FLOW_ADD:
        ret = _addFlow(flow, response);
        DBG_TRACE_RET(ret);
        break;
    case OFDP_FLOW_MODIFY:
        ret = _modifyFlow(flow, response);
        DBG_TRACE_RET(ret);
        break;
    case OFDP_FLOW_MODIFY_STRICT:
        ret = _modifyStrictFlow(flow, response);
        DBG_TRACE_RET(ret);
        break;
    case OFDP_FLOW_DELETE:
        ret = _deleteFlow(flow, response);
        DBG_TRACE_RET(ret);
        break;
    case OFDP_FLOW_DELETE_STRICT:
        ret = _deleteStrictFlow(flow, stats, response);
        DBG_TRACE_RET(ret);
        break;
    default:
        DBG_ERROR(("unknown updateFlow command=%u\n",
                   (unsigned int)flow.command));
        break;
    }

    DBG_TRACE_RET(OFDP_ERR_UNKNOWN_COMMAND);
}

// Get flow stats
OfDpReturn OfDpFlowTable::getFlowStats(const OfDpFlowStatsReqT &statsReq,
                                       OfConnManager &ofSwitch,
                                       OfDpResponseT &response) const {
    DBG_TRACE(DBG_FLOW_TABLE);

    std::set<OfDpFlowStatsT *> rsltSet;
    {
        this->find_loose(&statsReq.match, statsReq.out_port, statsReq.out_group, statsReq.cookie, statsReq.cookie_mask, rsltSet);
    }


    if (_mpDPInterface) {
        _mpDPInterface->GetRecordsFromDb(rsltSet, ofSwitch, response);
        size_t counts = rsltSet.size();
        for (const auto& i : rsltSet ) {
            i->stats_flags = (--counts ? 1 : 0);
            /// TODO: make a call to OfModule
            ofSwitch.GetStatReply(*i, response);
        } // end for
    }

    // All errors are handled through ofSwitch.
    DBG_TRACE_RET(OFDP_OK);
}

// Get aggregate stats
OfDpReturn OfDpFlowTable::getAggregateStats(const OfDpAggregateStatsReqT &statsReq,
                                            OfDpAggregateStatsT &stats,
                                            OfConnManager &ofSwitch,
                                            OfDpResponseT &response) const {
    DBG_TRACE(DBG_FLOW_TABLE);

    std::set<OfDpFlowStatsT *> statSet;
    this->find_loose(&statsReq.match, statsReq.out_port, statsReq.out_group, statsReq.cookie, statsReq.cookie_mask, statSet);
    stats.flow_count = statSet.size();
    if (_mpDPInterface) {
        _mpDPInterface->GetRecordsFromDb(statSet, ofSwitch, response);
    }
    stats.flow_count = statSet.size();
    for (const auto& i : statSet ) {
        stats.byte_count += i->byte_count;
        stats.packet_count += i->packet_count;
    } // end for
    ofSwitch.GetStatReply(stats, response);
    DBG_TRACE_RET(OFDP_OK);
}

// Get table stats
OfDpReturn OfDpFlowTable::getTableStats(OfDpTableStatsT &stats,
                                        OfConnManager &ofSwitch,
                                        OfDpResponseT &response) const {
    DBG_TRACE(DBG_FLOW_TABLE);
    stats.active_count = this->getFlowCount();
    stats.max_entries = _ttpObj->getMaxEntries();
    stats.lookup_count = 0;
    stats.matched_count = 0;
    stats.flags = 0;
    strncpy(stats.name, _ttpObj->getName(), sizeof(stats.name));
    ofSwitch.GetStatReply(stats, response);
    DBG_TRACE_RET(OFDP_OK);
}

// Get table stats
OfDpReturn OfDpFlowTable::getTableFeature(OfDpTableFeatureT &info) const {
    DBG_TRACE_ENTER;
    std::cout << "OfDpFlowTable::getTableFeature:" << std::endl;
    memcpy(&info, &(_ttpObj->getFeature()), sizeof(info));
    DBG_TRACE_RET(OFDP_OK);
}

// Get flow stats
bool OfDpFlowTable::getFlows(const OfDpFlowT &flow, std::set<OfDpFlowT *> &nset) const {
    return find_loose(&flow.match, OFDP_OFPP_NONE, OFDP_OFPP_NONE, OFDP_COOKIE_NONE, OFDP_COOKIE_NONE, nset);
}

/**
 * Dump the All Flows
 * 
 * @author sraihan (7/29/2014)
 * 
 * @param s 
 */
void OfDpFlowTable::Dump(std::ostream &s) const {
    s << "Table [Id:" << this->getId() << ", name: " << this->_ttpObj->getName() << "]" ;
    s << " flow_count:" << _flowCount;
    s << " Reusable flow Id size:" << _freeFlowIdQueue.size();
/*
    int count = 0;
    for(const auto& mInPort : mTuple | map_values){
        for(const auto& mDlVlan : mInPort | map_values){
            for(const auto& mDlType : mDlVlan | map_values){
                for(const auto& mDlVlanPcp : mDlType | map_values){
                    for(const auto& mMplsLabel : mDlVlanPcp | map_values){
                        for(const auto& mNwSrc : mMplsLabel | map_values){
                            for(const auto& mNwDst : mNwSrc | map_values){
                                for (const auto& TpnOtwg : mNwDst | map_values){
                                    for(const auto& mFlow : TpnOtwg | map_values){
                                        count++;
                                        s << "[" << count << "]" << mFlow;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    s << "out_port main:" << std::endl;
    for(const auto &p : mFlowIdOutPort)
    {
        s << " flow_id:" << p.first << " out_port:" << p.second << std::endl;
    }

    s << "out_port rollback:" << std::endl;
    for(const auto &p : mFlowIdOutPortRollBack)
    {
        s << " flow_id:" << p.first << " out_port:" << p.second << std::endl;
    }

    s << "out_group main:" << std::endl;
    for(const auto &p : mFlowIdOutGroup)
    {
            s << " flow_id:" << p.first << " out_group:" << p.second << std::endl;
    }

    s << "out_group rollback:" << std::endl;
    for(const auto &p : mFlowIdOutGroupRollBack)
    {
        s << " flow_id:" << p.first << " out_group:" << p.second << std::endl;
    }

 */
}

bool OfDpFlowTable::modify(const OfDpFlowT *oflow, const uint32_t flow_id) {
    DBG_TRACE(DBG_FLOW_ENTRY);

    // save rollback entries from main entries
    auto outPortIter = mFlowIdOutPort.equal_range(flow_id);
    for(auto& mFlowIdOutPortIter = outPortIter.first; mFlowIdOutPortIter != outPortIter.second; mFlowIdOutPortIter++){
        mFlowIdOutPortRollBack.insert(std::make_pair(flow_id, mFlowIdOutPortIter->second));
    }

    auto outGroupIter = mFlowIdOutGroup.equal_range(flow_id);
    for(auto& mFlowIdOutGroupIter = outGroupIter.first; mFlowIdOutGroupIter != outGroupIter.second; mFlowIdOutGroupIter++){
        mFlowIdOutGroupRollBack.insert(std::make_pair(flow_id, mFlowIdOutGroupIter->second));
    }

    //delete old out_port and out_group
    mFlowIdOutPort.erase(flow_id);
    mFlowIdOutGroup.erase(flow_id);

    // update main entries with new values
    for (int i = 0; i < oflow->num_actions; i++) {
        if (oflow->actions[i].command == OFDP_OUTPUT) {
            mFlowIdOutPort.insert(std::make_pair(flow_id, oflow->actions[i].value.output.port));
        }
    }

    for (int i = 0; i < oflow->num_actions; i++) {
        if (oflow->actions[i].command == OFDP_GROUP) {
            mFlowIdOutGroup.insert(std::make_pair(flow_id, oflow->actions[i].value.group_id));
        }
    }

    return true;
}

bool OfDpFlowTable::rollback_modify(const uint32_t flow_id) {
    DBG_TRACE(DBG_FLOW_ENTRY);

    //delete old out_port and out_group
    mFlowIdOutPort.erase(flow_id);
    mFlowIdOutGroup.erase(flow_id);

    // save rollback entries from main entries
    auto outPortIter = mFlowIdOutPortRollBack.equal_range(flow_id);
    for(auto& mFlowIdOutPortIter = outPortIter.first; mFlowIdOutPortIter != outPortIter.second; mFlowIdOutPortIter++){
        mFlowIdOutPort.insert(std::make_pair(flow_id, mFlowIdOutPortIter->second));
    }

    auto outGroupIter = mFlowIdOutGroupRollBack.equal_range(flow_id);
    for(auto& mFlowIdOutGroupIter = outGroupIter.first; mFlowIdOutGroupIter != outGroupIter.second; mFlowIdOutGroupIter++){
        mFlowIdOutGroup.insert(std::make_pair(flow_id, mFlowIdOutGroupIter->second));
    }

    //delete out_port and out_group from rollback
    mFlowIdOutPortRollBack.erase(flow_id);
    mFlowIdOutGroupRollBack.erase(flow_id);

    return true;
}

bool OfDpFlowTable::insert(const OfDpFlowT *oflow) {
    _buildFlowIdList(oflow->flow_id);
    return insert(oflow, oflow->flow_id);
}

/**
 * Insert a flow - no massaging of match fields
 * 
 * @author sraihan (7/29/2014)
 * 
 * @param oflow 
 * 
 * @return bool 
 */
bool OfDpFlowTable::insert(const OfDpFlowT *oflow, const uint32_t flow_id) {
    DBG_TRACE(DBG_FLOW_ENTRY);

    (const_cast<OfDpFlowT *>(oflow))->ls_idx = const_cast<OfDpLogicalSwT *>(mpCofaLS);
    if (flow_id == 0) (const_cast<OfDpFlowT *>(oflow))->flow_id = _getNextFlowId();
    _flowCount++;
    if (oflow->cookie == 0) (const_cast<OfDpFlowT *>(oflow))->cookie = oflow->flow_id;
    try {
        mTuple[oflow->match.in_port]
            [oflow->match.dl_vlan][oflow->match.dl_type][oflow->match.dl_vlan_pcp]
            [oflow->match.mpls_label1]
            [oflow->match.nw_src][oflow->match.nw_dst]
            [oflow->match.tpn][oflow->priority] = *oflow;
        for (int i = 0; i < oflow->num_actions; i++) {
            if (oflow->actions[i].command == OFDP_OUTPUT) {
                mFlowIdOutPort.insert(std::make_pair(oflow->flow_id, oflow->actions[i].value.output.port));
            }
        }

        for (int i = 0; i < oflow->num_actions; i++) {
            if (oflow->actions[i].command == OFDP_GROUP) {
                mFlowIdOutGroup.insert(std::make_pair(oflow->flow_id, oflow->actions[i].value.group_id));
            }
        }
    } catch (...) {
        DBG(DBG_ERR_OF, ("Flow insertion error flow_id %#lx, lsName: %s",(long unsigned)oflow->flow_id, oflow->ls_idx->name.c_str()));
    }

    return true;
}

/**
 * Remove 1 flow
 * 
 * @author sraihan (7/29/2014)
 * 
 * @param oflow 
 * 
 * @return bool 
 */
bool OfDpFlowTable::remove(const OfDpFlowT *oflow) {
    DBG_TRACE(DBG_FLOW_ENTRY);
    auto mInPort = mTuple.find(oflow->match.in_port);
    if (mInPort != mTuple.end()) {
        auto mDlVlan = mInPort->second.find(oflow->match.dl_vlan);
        if (mDlVlan != mInPort->second.end()) {
            auto mDlType = mDlVlan->second.find(oflow->match.dl_type);
            if (mDlType != mDlVlan->second.end()) {
                auto mDlVlanPcp = mDlType->second.find(oflow->match.dl_vlan_pcp);
                if (mDlVlanPcp != mDlType->second.end()) {
                    auto mMplsLabel = mDlVlanPcp->second.find(oflow->match.mpls_label1);
                    if (mMplsLabel != mDlVlanPcp->second.end()) {
                        auto mNwSrc = mMplsLabel->second.find(oflow->match.nw_src);
                        if (mNwSrc != mMplsLabel->second.end()) {
                            auto mNwDst = mNwSrc->second.find(oflow->match.nw_dst);
                            if (mNwDst != mNwSrc->second.end()) {
                                auto mTpn = mNwDst->second.find(oflow->match.tpn);
                                if (mTpn != mNwDst->second.end()) { //mTpn
                                    auto mPriority = mTpn->second.find(oflow->priority);
                                    if (mPriority != mTpn->second.end()) {
                                        const auto flowId = mPriority->second.flow_id;

                                        _flowCount--;
                                        _updateFlowIdList(flowId);

                                        mFlowIdOutPort.erase(flowId);
                                        mFlowIdOutPortRollBack.erase(flowId);
                                        mFlowIdOutGroup.erase(flowId);
                                        mFlowIdOutGroupRollBack.erase(flowId);
                                        mTpn->second.erase(oflow->priority);
                                        if (mTpn->second.empty()) mNwDst->second.erase(mTpn); // single priority flow removal
                                        if (mNwDst->second.empty()) mNwSrc->second.erase(mNwDst);
                                        if (mNwSrc->second.empty()) mMplsLabel->second.erase(mNwSrc);
                                        if (mMplsLabel->second.empty()) mDlVlanPcp->second.erase(mMplsLabel);
                                        if (mDlVlanPcp->second.empty()) mDlType->second.erase(mDlVlanPcp);
                                        if (mDlType->second.empty()) mDlVlan->second.erase(mDlType);
                                        if (mDlVlan->second.empty()) mInPort->second.erase(mDlVlan);
                                        if (mInPort->second.empty()) mTuple.erase(mInPort);
                                        return true;
                                    }
                                } //mTpn
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}

bool OfDpFlowTable::remove_set(std::set<OfDpFlowT *> &inset, std::set<uint32_t> &outset) {
    DBG_TRACE(DBG_FLOW_ENTRY);
    uint32_t flow_id = 0;

    for(const auto& oflow : inset){
        flow_id = remove_flow(oflow);
        if (flow_id != 0) {
            outset.insert(flow_id);
        } else {
            return false;
        }
    }
    return (inset.size() == outset.size());
}

/**
 * Remove a set of flow
 * 
 * @author sraihan (7/29/2014)
 * 
 * @param oflow 
 * 
 * @return uint32_t 
 */
uint32_t OfDpFlowTable::remove_flow(const OfDpFlowT *oflow) {
    DBG_TRACE(DBG_FLOW_ENTRY);
    // Needs to iterate all cookies.
    auto mInPort = mTuple.find(oflow->match.in_port);
    if (mInPort != mTuple.end()) {
        auto mDlVlan = mInPort->second.find(oflow->match.dl_vlan);
        if (mDlVlan != mInPort->second.end()) {
            auto mDlType = mDlVlan->second.find(oflow->match.dl_type);
            if (mDlType != mDlVlan->second.end()) {
                auto mDlVlanPcp = mDlType->second.find(oflow->match.dl_vlan_pcp);
                if (mDlVlanPcp != mDlType->second.end()) {
                    auto mMplsLabel = mDlVlanPcp->second.find(oflow->match.mpls_label1);
                    if (mMplsLabel != mDlVlanPcp->second.end()) {
                        auto mNwSrc = mMplsLabel->second.find(oflow->match.nw_src);
                        if (mNwSrc != mMplsLabel->second.end()) {
                            auto mNwDst = mNwSrc->second.find(oflow->match.nw_dst);
                            if (mNwDst != mNwSrc->second.end()) {
                                auto mTpn = mNwDst->second.find(oflow->match.tpn);
                                if (mTpn != mNwDst->second.end()) {
                                    for(const auto& mFlow : mTpn->second | map_values){
                                        if (oflow->flow_id == mFlow.flow_id) {
                                            const uint32_t rFlowId = mFlow.flow_id;
                                            // erase map entries  - avoid exact find before delete if flow table takes care of search.
                                            mFlowIdOutPort.erase(rFlowId);
                                            mFlowIdOutPortRollBack.erase(rFlowId);
                                            mFlowIdOutGroup.erase(rFlowId);
                                            mFlowIdOutGroupRollBack.erase(rFlowId);
                                            _flowCount--;
                                            _updateFlowIdList(rFlowId);
                                            mTpn->second.erase(mFlow.priority);
                                            if (mTpn->second.empty()) mNwDst->second.erase(mTpn); // single priority flow removal
                                            if (mNwDst->second.empty()) mNwSrc->second.erase(mNwDst);
                                            if (mNwSrc->second.empty()) mMplsLabel->second.erase(mNwSrc);
                                            if (mMplsLabel->second.empty()) mDlVlanPcp->second.erase(mMplsLabel);
                                            if (mDlVlanPcp->second.empty()) mDlType->second.erase(mDlVlanPcp);
                                            if (mDlType->second.empty()) mDlVlan->second.erase(mDlType);
                                            if (mDlVlan->second.empty()) mInPort->second.erase(mDlVlan);
                                            if (mInPort->second.empty()) mTuple.erase(mInPort);
                                            return rFlowId;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

/**
 * Search for exact match and exact priority flow
 * 
 * @author sraihan (7/29/2014)
 * 
 * @param oflow 
 * @param ofset 
 * 
 * @return bool 
 */
bool OfDpFlowTable::find_exact_with_priority(const OfDpFlowT *oflow, std::set<OfDpFlowT *> &ofset) const {
    DBG_TRACE(DBG_FLOW_ENTRY);
    DBG(DBG_FLOW_ENTRY, (" Flowtable size: %#lx\n",(long unsigned)mTuple.size()));
    // Needs to iterate all cookies ???
    auto mInPort = mTuple.find(oflow->match.in_port);
    if (mInPort != mTuple.end()) {
        auto mDlVlan = mInPort->second.find(oflow->match.dl_vlan);
        if (mDlVlan != mInPort->second.end()) {
            auto mDlType = mDlVlan->second.find(oflow->match.dl_type);
            if (mDlType != mDlVlan->second.end()) {
                auto mDlVlanPcp = mDlType->second.find(oflow->match.dl_vlan_pcp);
                if (mDlVlanPcp != mDlType->second.end()) {
                    auto mMplsLabel = mDlVlanPcp->second.find(oflow->match.mpls_label1);
                    if (mMplsLabel != mDlVlanPcp->second.end()) {
                        auto mNwSrc = mMplsLabel->second.find(oflow->match.nw_src);
                        if (mNwSrc != mMplsLabel->second.end()) {
                            auto mNwDst = mNwSrc->second.find(oflow->match.nw_dst);
                            if (mNwDst != mNwSrc->second.end()) {
                                auto mTpn = mNwDst->second.find(oflow->match.tpn);
                                if (mTpn != mNwDst->second.end()) {
                                    auto mPriorityFlow = mTpn->second.find(oflow->priority);
                                    if (mPriorityFlow != mTpn->second.end()) {
                                        ofset.insert(const_cast<OfDpFlowT *>(&mPriorityFlow->second));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

/**
 * Search for exact match
 * 
 * @author sraihan (7/29/2014)
 * 
 * @param oflow 
 * @param ofset 
 * 
 * @return bool 
 */
bool OfDpFlowTable::find_exact(const OfDpFlowT *oflow, std::set<OfDpFlowT *> &ofset) const {
    DBG_TRACE(DBG_FLOW_ENTRY);
    ofset.clear();

    // Needs to iterate all cookies.
    auto mInPort = mTuple.find(oflow->match.in_port);
    if (mInPort != mTuple.end()) {
        auto mDlVlan = mInPort->second.find(oflow->match.dl_vlan);
        if (mDlVlan != mInPort->second.end()) {
            auto mDlType = mDlVlan->second.find(oflow->match.dl_type);
            if (mDlType != mDlVlan->second.end()) {
                auto mDlVlanPcp = mDlType->second.find(oflow->match.dl_vlan_pcp);
                if (mDlVlanPcp != mDlType->second.end()) {
                    auto mMplsLabel = mDlVlanPcp->second.find(oflow->match.mpls_label1);
                    if (mMplsLabel != mDlVlanPcp->second.end()) {
                        auto mNwSrc = mMplsLabel->second.find(oflow->match.nw_src);
                        if (mNwSrc != mMplsLabel->second.end()) {
                            auto mNwDst = mNwSrc->second.find(oflow->match.nw_dst);
                            if (mNwDst != mNwSrc->second.end()) {
                                auto mTpn = mNwDst->second.find(oflow->match.tpn);
                                if (mTpn != mNwDst->second.end()) {
                                    for(const auto& mPriorityFlow : mTpn->second){
                                        ofset.insert(const_cast<OfDpFlowT *>(&(mPriorityFlow.second)));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}

/**
 * It tries to find overlapping entry in case of match failure 
 * overlapping is defined by wildcarded entry 
 * 
 * @author sraihan (7/29/2014)
 * 
 * @param oflow 
 * @param ofset 
 * 
 * @return bool 
 */
bool OfDpFlowTable::find_overlap(const OfDpFlowT *oflow, std::set<OfDpFlowT *> &ofset) const {
    DBG_TRACE(DBG_FLOW_ENTRY);
    std::set<uint32_t> rsltset;
    ofset.clear();
    const auto cswild = 0;

    auto mInPort = (oflow->match.in_port) ? mTuple.find(oflow->match.in_port) : mTuple.begin();
    mInPort = (mInPort == mTuple.end()) ? mTuple.find(cswild) : mInPort;

    for (; mInPort != mTuple.end(); (oflow->match.in_port) ? mTuple.end() : ++mInPort) {
        auto mDlVlan = (oflow->match.dl_vlan) ? mInPort->second.find(oflow->match.dl_vlan) : mInPort->second.begin();
        mDlVlan = (mDlVlan == mInPort->second.end()) ? mInPort->second.find(cswild) : mDlVlan;

        for (; mDlVlan != mInPort->second.end(); (oflow->match.dl_vlan) ? mInPort->second.end() : ++mDlVlan) {
            auto mDlType = (oflow->match.dl_type) ? mDlVlan->second.find(oflow->match.dl_type) : mDlVlan->second.begin();
            mDlType = (mDlType == mDlVlan->second.end()) ? mDlVlan->second.find(cswild) : mDlType;

            for (; mDlType != mDlVlan->second.end(); (oflow->match.dl_type) ? mDlVlan->second.end() : ++mDlType) {
                auto mDlVlanPcp = (oflow->match.dl_vlan_pcp) ? mDlType->second.find(oflow->match.dl_vlan_pcp) : mDlType->second.begin();
                mDlVlanPcp = (mDlVlanPcp == mDlType->second.end()) ? mDlType->second.find(cswild) : mDlVlanPcp;

                for (; mDlVlanPcp != mDlType->second.end(); (oflow->match.dl_vlan_pcp) ? mDlType->second.end() : ++mDlVlanPcp) {
                    auto mMplsLabel = (oflow->match.mpls_label1) ? mDlVlanPcp->second.find(oflow->match.mpls_label1) : mDlVlanPcp->second.begin();
                    mMplsLabel = (mMplsLabel == mDlVlanPcp->second.end()) ? mDlVlanPcp->second.find(cswild) : mMplsLabel;

                    for (; mMplsLabel != mDlVlanPcp->second.end(); (oflow->match.mpls_label1) ? mDlVlanPcp->second.end() : ++mMplsLabel) {
                        auto mNwSrc = (oflow->match.nw_src) ? mMplsLabel->second.find(oflow->match.nw_src) : mMplsLabel->second.begin();
                        mNwSrc = (mNwSrc == mMplsLabel->second.end()) ? mMplsLabel->second.find(cswild) : mNwSrc;

                        for (; mNwSrc != mMplsLabel->second.end(); (oflow->match.nw_src) ? mMplsLabel->second.end() : ++mNwSrc) { // NwSrc loop start
                            auto mNwDst = (oflow->match.nw_dst) ? mNwSrc->second.find(oflow->match.nw_dst) : mNwSrc->second.begin();
                            mNwDst = (mNwDst == mNwSrc->second.end()) ? mNwSrc->second.find(cswild) : mNwDst;

                            for (; mNwDst != mNwSrc->second.end(); (oflow->match.nw_dst) ? mNwSrc->second.end() : ++mNwDst) { // NwDst loop start
                                auto mTpn = (oflow->match.tpn) ? mNwDst->second.find(oflow->match.tpn) : mNwDst->second.begin();
                                mTpn = (mTpn == mNwDst->second.end()) ? mNwDst->second.find(cswild) : mTpn;

                                for (; mTpn != mNwDst->second.end(); (oflow->match.tpn) ? mNwDst->second.end() : ++mTpn) {
                                    for(const auto& mPriorityFlow : mTpn->second|map_values){ // priority loop start
                                        ofset.insert(const_cast<OfDpFlowT *>(&(mPriorityFlow)));
                                    } //priority
                                } //tpn
                            } //nw_dst
                        } //nw_src
                    } //mpls_label1
                } //dl_vlan_pcp
            } //dl_type
        } //dl_vlan
    } //in_port
    return true; /// always true check set<>
}

/**
 * Seach all if not exact match
 * 
 * @author sraihan (7/29/2014)
 * 
 * @param omatch 
 * @param inOutPort 
 * @param inOutGroup 
 * @param cookie 
 * @param cookie_mask 
 * @param ofset 
 * 
 * @return bool 
 */
bool OfDpFlowTable::find_loose(const OfDpMatchT *omatch, const uint32_t inOutPort, const uint32_t inOutGroup, const uint64_t cookie, const uint64_t cookie_mask, std::set<OfDpFlowStatsT *> &ofset) const {
    DBG_TRACE(DBG_FLOW_ENTRY);
    const uint64_t ccookie = ((cookie & cookie_mask) == 0) ? ~0 : ((cookie & cookie_mask));
    ofset.clear();

    auto mInPort = (omatch->in_port)? mTuple.find(omatch->in_port) : mTuple.begin();

    for (; mInPort != mTuple.end(); mInPort = (omatch->in_port) ? mTuple.end() : ++mInPort) { //for loop start
        auto mDlVlan = (omatch->dl_vlan) ? mInPort->second.find(omatch->dl_vlan) : mInPort->second.begin();

        for (; mDlVlan != mInPort->second.end(); mDlVlan = (omatch->dl_vlan) ? mInPort->second.end() : ++mDlVlan) { //for loop start
            auto mDlType = (omatch->dl_type) ? mDlVlan->second.find(omatch->dl_type) :mDlVlan->second.begin();

            for (; mDlType != mDlVlan->second.end(); mDlType = (omatch->dl_type) ? mDlVlan->second.end() : ++mDlType) { //for loop start
                auto mDlVlanPcp =(omatch->dl_vlan_pcp) ? mDlType->second.find(omatch->dl_vlan_pcp) : mDlType->second.begin();

                for (; mDlVlanPcp != mDlType->second.end(); mDlVlanPcp = (omatch->dl_vlan_pcp) ? mDlType->second.end() : ++mDlVlanPcp) { //for loop start
                    auto mMplsLabel = (omatch->mpls_label1)? mDlVlanPcp->second.find(omatch->mpls_label1) : mDlVlanPcp->second.begin();

                    for (; mMplsLabel != mDlVlanPcp->second.end(); mMplsLabel = (omatch->mpls_label1) ? mDlVlanPcp->second.end() : ++mMplsLabel) { // for loop start
                        auto mNwSrc = (omatch->nw_src) ? mMplsLabel->second.find(omatch->nw_src) : mMplsLabel->second.begin();

                        for (; mNwSrc != mMplsLabel->second.end(); mNwSrc = (omatch->nw_src) ? mMplsLabel->second.end() : ++mNwSrc) {
                            auto mNwDst = (omatch->nw_dst) ? mNwSrc->second.find(omatch->nw_dst) :  mNwSrc->second.begin();

                            for (; mNwDst != mNwSrc->second.end(); mNwDst = (omatch->nw_dst) ? mNwSrc->second.end() : ++mNwDst) {
                                auto mTpn = (omatch->tpn) ? mNwDst->second.find(omatch->tpn) :mNwDst->second.begin();

                                for (; mTpn != mNwDst->second.end(); mTpn = (omatch->tpn) ? mNwDst->second.end() : ++mTpn) {
                                    for(auto& mPriorityFlow : mTpn->second
                                        | filtered([&](const FlowEntries::value_type &p){ return (p.second.cookie & ccookie) != 0;})
                                        | map_values){
                                        if ((inOutPort != OFDP_OFPP_NONE) || (inOutGroup != OFDP_OFPP_NONE)) {
                                            if (inOutPort != OFDP_OFPP_NONE) {
                                                auto outPortIter = mFlowIdOutPort.equal_range(mPriorityFlow.flow_id);
                                                for(auto mFlowIdOutPortIter = outPortIter.first; mFlowIdOutPortIter != outPortIter.second; mFlowIdOutPortIter++){
                                                    if (mFlowIdOutPortIter->second == inOutPort) {
                                                        ofset.insert(const_cast<OfDpFlowT *>(&mPriorityFlow));
                                                        break;
                                                    }
                                                }
                                            }
                                            if (inOutGroup != OFDP_OFPP_NONE) {
                                                auto outGroupIter = mFlowIdOutGroup.equal_range(mPriorityFlow.flow_id);
                                                for(auto mFlowIdOutGroupIter = outGroupIter.first; mFlowIdOutGroupIter != outGroupIter.second; mFlowIdOutGroupIter++){
                                                    if(mFlowIdOutGroupIter->second == inOutGroup){
                                                        ofset.insert(const_cast<OfDpFlowT*>(&mPriorityFlow));
                                                        break;
                                                    }
                                                }
                                            }
                                        } else {
                                            ofset.insert(const_cast<OfDpFlowT *>(&mPriorityFlow));
                                        }

                                    } //priority - done innermost loop
                                } // Tpn
                            } //nw_dst

                        } //nw_src
                    } //mpls_label1
                } //dl_vlan_pcp

            } //dl_type

        } //dl_vlan

    } //in_port

    return true;
}

uint32_t OfDpFlowTable::_getNextFlowId() {
    uint32_t rv = 0x0;
    if (mpCofaLS->adminState) {
        assert(!_freeFlowIdQueue.empty());
        rv = _freeFlowIdQueue.front();
        _freeFlowIdQueue.pop();
        //std::cout << " skl get flowId=" << rv << std::endl;
    } else assert(0);

    return rv;
}

void OfDpFlowTable::_buildFlowIdList(uint32_t id) {
    if (mpCofaLS->adminState) {
        assert(0);
    } else {
        assert(id != 0 && id < maxFlowNum);
        _flowIdUsage.set(id);
        //std::cout << " skl used flowId=" << id << std::endl;
    }
}

void OfDpFlowTable::_updateFlowIdList(uint32_t id) {
    if (mpCofaLS->adminState) {
        _freeFlowIdQueue.push(id);
    } else _flowIdUsage.reset(id);
    // std::cout << " skl freed flowId=" << id << std::endl;
}

bool
OfDpFlowTable::getFlows(uint32_t groupId, std::set<OfDpFlowStatsT *> &rsltSet) const {
    OfDpMatchT match;

    find_loose(&match, OFDP_OFPP_NONE, groupId, OFDP_COOKIE_NONE, OFDP_COOKIE_NONE, rsltSet);

    return true;
}

void OfDpFlowTable::SetIndexTableRoleToPrimary() {
    if (!mpCofaLS->adminState) return;

    for (uint32_t i = startFlowNum; i < maxFlowNum; ++i) {
        if (!_flowIdUsage.test(i)) {
            _freeFlowIdQueue.push(i);
        }
    }

    DBG(DBG_FLOW_ENTRY, ("\n-----  OfDpFlowTable adminState %d,  numOfFreeId=%d\n", mpCofaLS->adminState
                         ,(int)_freeFlowIdQueue.size()));
}

// Output
std::ostream& operator<<(std::ostream &out, const OfDpFlowTable &instance) {
    instance.Dump(out);
    return (out);
}


