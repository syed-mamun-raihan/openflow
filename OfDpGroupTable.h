#ifndef OfDpGroupTable_h_
#define OfDpGroupTable_h_
/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include <iostream>
#include "OfConfig.hpp"
#include "OfDpDefine.hpp"
#include "OfDpGroup.h"
#include "OfDataPlaneApi.h"
#include "GenericObservableConfig.hpp"

class OfConnManager;
class OfDpLogicalSw;

class OfDpGroupTable
{
public:
    OfDpGroupTable(OfDataPlaneApi* pDPIntf,
                   OfDpLogicalSw* ls);
    ~OfDpGroupTable();

    // Update group
    OfDpReturn updateGroup( OfDpGroupT &group, OfDpResponseT& resp);

    // Get group
    const OfDpGroup* getGroup( const uint32_t grpId ) const { return const_cast<OfDpGroupTable*>(this)->getGroup(grpId); }
    OfDpGroup* getGroup( const uint32_t grpId );
    bool add_groupTable(const OfDpGroupT& grp);
    bool is_groupPresent(const OfDpGroupT& grp) const;
    bool delete_groupTable(int grpId);

    OfDpReturn getStat(OfDpGroupDescStatsT& info,
                       OfConnManager &ofSwitch,
                       OfDpResponseT& response);

    OfDpReturn getStat(OfDpGroupStatsT& info,
                        OfConnManager &ofSwitch,
                        OfDpResponseT& response);

    // Get group Feature
    OfDpReturn getFeatures(OfDpGroupFeature& feature);

    const OfDpLogicalSw* GetLogicalSw() const { return mLogicalSwitch;}
    OfDpLogicalSw* GetLogicalSw() { return mLogicalSwitch;}
    const GenericConfigObserver<OfDpGroupT>& getAllGroups() const { return mGroups; }
    GenericConfigObserver<OfDpGroupT>& getAllGroups() { return mGroups; }


    // Output
    friend std::ostream& operator<<( std::ostream& out, OfDpGroupTable& instance );

private:
    OfDpReturn ValidateGroup(const OfDpGroupT& group);
    OfDpReturn DeleteGroup(uint32_t groupId, OfDpResponseT& response);

    OfDataPlaneApi*  mpDPIntf = 0;
	// indexed with types
    OfDpGroupFeature mGroupFeatures;
    OfDpLogicalSw* mLogicalSwitch = 0;
    GenericConfigObserver<OfDpGroupT> mGroups;

};

#endif
