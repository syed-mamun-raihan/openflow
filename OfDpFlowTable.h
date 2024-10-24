#ifndef OfDpFlowTable_h_
    #define OfDpFlowTable_h_

/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
    #include <cstdint>
    #include <cstdlib>
    #include <iostream>
    #include <array>
    #include <bitset>
    #include <map>
    #include <set>
    #include <tuple>
    #include <queue>
    #include <algorithm>
    #include <mutex>
    #include <memory>
    #include "OfDpDefine.hpp"
    #include "OfConfig.hpp"
    #include "OfDpDefine.hpp"
    #include "ofDebug.h"


class OfDataPlaneApi;
class OfDpResourceTable;
class OfConnManager;

/**
 * only static configurations
 * 
 * @author sraihan 
 */
class OfTableTypeProfileBase {
public:
    OfTableTypeProfileBase()
    {
        _tableFeature.config = OFDP_TABLE_MISS_DROP|OFDP_TABLE_MISS_CONTROLLER;
        snprintf(_tableFeature.name, OFDP_OFP_MAX_TABLE_NAME_LEN, "flow_table_%u", _tableFeature.table_id);
    }

    virtual ~OfTableTypeProfileBase(){}

    // get table config
    uint32_t getTableConfig() const { return _tableFeature.config;}

    // Get supported Actions
    uint64_t getSupportedActions() const { return _tableFeature.actions;} // for 1.0

    // Get supported Match fields.
    uint64_t getSupportedMatch() const { return _tableFeature.match_fields;}

    // Get supported Wildcard fields.
    uint64_t getSupportedWildcards() const { return _tableFeature.wildcard_fields;}

    // Get supported Write Actions.
    uint64_t getSupportedWriteActions() const { return _tableFeature.write_actions;}

    // Get supported Apply Actions.
    uint64_t getSupportedApplyActions() const { return _tableFeature.apply_actions;}

    // Get supported Write Set Fields
    uint64_t getSupportedWriteSetFields() const { return _tableFeature.write_set_fields;}

    // Get supported Apply Set Fields
    uint64_t getSupportedApplySetFields() const { return _tableFeature.apply_set_fields;}

    // Get supported Metadata Match
    uint64_t getSupportedMetadataMatch() const { return _tableFeature.metadata_match;}

    // Get supported Metadata Write
    uint64_t getSupportedMetadataWrite() const { return _tableFeature.metadata_write;}

    // Get supported Instructions
    uint64_t getSupportedInstructions() const { return _tableFeature.instructions;}

    const char* getName() const { return _tableFeature.name;}

    uint8_t getId() const { return _tableFeature.table_id;}

    uint32_t getMaxEntries() const { return _tableFeature.max_entries;}

    void setMaxEntries(const uint32_t param) { _tableFeature.max_entries = param;}

    const OfDpTableFeatureT& getFeature() const { return _tableFeature;}
    OfDpTableFeatureT& getFeature() { return _tableFeature;}

protected:
    OfDpTableFeatureT _tableFeature;
};

/**
 * Provides information for Table Type Profile
 * 
 * @author sraihan (7/29/2014)
 */
class OfTableTypeSingle final : public OfTableTypeProfileBase {
public:
    OfTableTypeSingle(const uint8_t id = 0, const uint32_t maxEntries =  OFDP_SWITCH_MAX_ENTRIES);
};

/**
 * This table owns Multi-Index map for in-memory flow storage
 * 
 * @author sraihan (7/29/2014)
 */
class OfDpFlowTable final {
public:

    OfDpFlowTable( OfTableTypeProfileBase* tableProfile,
                   OfDataPlaneApi* const pDPIntrf,
                   const OfDpLogicalSwT* );
    ~OfDpFlowTable();

    // Get Table ID.
    uint32_t getId() const { return _ttpObj->getId();}

    // Update Flow.
    OfDpReturn updateFlow( const OfDpFlowT &flow,
                           OfDpResponseT& response);

    // get table config
    uint32_t getTableConfig() const { return _ttpObj->getTableConfig();}

    // Get supported Actions
    uint64_t getSupportedActions() const { return _ttpObj->getSupportedActions();}

    // Get supported Match fields.
    uint64_t getSupportedMatch() const { return _ttpObj->getSupportedMatch();}

    // Get supported Wildcard fields.
    uint64_t getSupportedWildcards() const { return _ttpObj->getSupportedWildcards();}

    // Get supported Write Actions.
    uint64_t getSupportedWriteActions() const { return _ttpObj->getSupportedWriteActions();}

    // Get supported Apply Actions.
    uint64_t getSupportedApplyActions() const { return _ttpObj->getSupportedApplyActions();}

    // Get supported Write Set Fields
    uint64_t getSupportedWriteSetFields() const { return _ttpObj->getSupportedWriteSetFields();}

    // Get supported Apply Set Fields
    uint64_t getSupportedApplySetFields() const { return _ttpObj->getSupportedApplySetFields();}

    // Get supported Metadata Match
    uint64_t getSupportedMetadataMatch() const { return _ttpObj->getSupportedMetadataMatch();}

    // Get supported Metadata Write
    uint64_t getSupportedMetadataWrite() const { return _ttpObj->getSupportedMetadataWrite();}

    // Get supported Instructions
    uint64_t getSupportedInstructions() const { return _ttpObj->getSupportedInstructions();}

    // Get flow stats
    OfDpReturn getFlowStats( const OfDpFlowStatsReqT &statsReq,
                             OfConnManager &ofSwitch,
                             OfDpResponseT& response ) const;

    // Get flow stats
    bool getFlows(const OfDpFlowT& flow, std::set<OfDpFlowT*>& ) const;


    // Get aggregate stats
    OfDpReturn getAggregateStats( const OfDpAggregateStatsReqT &statsReq,
                                  OfDpAggregateStatsT &stats,
                                  OfConnManager &ofSwitch,
                                  OfDpResponseT& response ) const;

    // Get table stats
    OfDpReturn getTableStats( OfDpTableStatsT &stats,
                              OfConnManager &ofSwitch,
                             OfDpResponseT& response) const;

    // Get table stats
    OfDpReturn getTableFeature( OfDpTableFeatureT &info ) const;

    // Output
    friend std::ostream& operator<<( std::ostream& out, const OfDpFlowTable& instance );

    // start of MultiIndexTable utility functions
    void SetIndexTableRoleToPrimary();
    uint32_t getFlowCount(void) const { return _flowCount;}
    const OfDpLogicalSwT* getLsIndex(void) const { return mpCofaLS;}

    // Add/Modify/Delete methods.
    OfDpReturn _addFlow( const OfDpFlowT &flow,
                         OfDpResponseT& response);

    OfDpReturn _modifyFlow( const OfDpFlowT &flow,
                            OfDpResponseT& response);

    OfDpReturn _modifyStrictFlow( const OfDpFlowT &flow,
                                  OfDpResponseT& response );

    OfDpReturn _deleteFlow( const OfDpFlowT &flow,
                            OfDpResponseT& response);

    OfDpReturn _deleteStrictFlow( const OfDpFlowT &flow,
                                  OfDpFlowStatsT &stats,
                                  OfDpResponseT& response,
                                  OfDpFlowRemovedReasonT reason=OFDP_OFPRR_DELETE);

    void _printStats( FILE *outFp, const OfDpFlowStatsT &stats );

    // Diasbled CTORs
    bool insert(const OfDpFlowT *oflow);
    bool insert(const OfDpFlowT *oflow, const uint32_t flow_id);
    bool modify(const OfDpFlowT *oflow, const uint32_t flow_id);
    bool rollback_modify(const uint32_t flow_id);

    bool remove(const OfDpFlowT *oflow);
    bool remove_set(std::set<OfDpFlowT*>& inset, std::set<uint32_t>& outset);
    uint32_t remove_flow(const OfDpFlowT* ofset);

    bool find_exact(const OfDpFlowT *oflow, std::set<OfDpFlowT*>& ofset) const;
    bool find_exact_with_priority(const OfDpFlowT *oflow, std::set<OfDpFlowT*>& ofset) const;
    bool find_overlap(const OfDpFlowT *oflow, std::set<OfDpFlowT*>& ofset) const;
    bool find_loose(const OfDpMatchT *omatch, const uint32_t inOutPort, const uint32_t inOutGroup, const uint64_t cookie, const uint64_t cookie_mask, std::set<OfDpFlowStatsT*>& ofset) const;

    uint32_t _getNextFlowId();

    bool getFlows(uint32_t groupId, std::set<OfDpFlowStatsT*>& rsltSet) const;
    void Dump(std::ostream& s) const;

 private:
     OfDpFlowTable(const OfDpFlowTable& ) = delete;
     OfDpFlowTable& operator = (const OfDpFlowTable& ) = delete;

     OfTableTypeProfileBase* _ttpObj = 0;
     // memory borrowed , lblyth- is this comment still accurate?
     OfDataPlaneApi* const _mpDPInterface = 0;
     OfDpLogicalSwT* mpCofaLS = 0;
     uint32_t _flowCount;

     /// This first one is inner most map
     typedef std::map<uint32_t, OfDpFlowT > FlowEntries;
     typedef std::map<uint32_t, FlowEntries> TpnOtwg;
     typedef std::map<uint32_t, TpnOtwg> NwDst;
     typedef std::map<uint32_t, NwDst> NwSrc;
     typedef std::map<uint32_t, NwSrc> MplsLabels;
     typedef std::map<uint32_t, MplsLabels> DlVlanPcp;
     typedef std::map<uint32_t, DlVlanPcp> DlType;
     typedef std::map<uint32_t, DlType> DlVlan;
     typedef std::map<uint32_t, DlVlan> MultiIndexTable; // Inport to VlanMap
     /// Last one is outer most map

    MultiIndexTable mTuple;

    std::multimap<uint32_t, uint32_t> mFlowIdOutPort;
    std::multimap<uint32_t, uint32_t> mFlowIdOutPortRollBack;
    //  Group table related
    std::multimap<uint32_t, uint32_t> mFlowIdOutGroup;
    std::multimap<uint32_t, uint32_t> mFlowIdOutGroupRollBack;

     std::queue<uint32_t> _freeFlowIdQueue;
     std::bitset<maxFlowNum> _flowIdUsage;


     void _updateFlowIdList(uint32_t);
     void _buildFlowIdList(uint32_t);

};

#endif
