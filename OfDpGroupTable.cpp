#include <boost/range/adaptor/map.hpp>
#include "OfConfig.hpp"
#include "ofDebug.h"
#include "OfDpDefine.hpp"
#include "OfDpFlowTable.h"
#include "OfDpGroupTable.h"
#include "OfDataPlaneApi.h"
#include "OfDpLogicalSw.h"

/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
// For logging
#include <cstdio>

OfDpGroupTable::OfDpGroupTable(OfDataPlaneApi *pDPIntf, OfDpLogicalSw *ls) :
    mpDPIntf(pDPIntf),
    mLogicalSwitch(ls) {
    DBG_TRACE_ENTER;
    memset(&mGroupFeatures, 0, sizeof(mGroupFeatures));
    mGroupFeatures.types = OFDP_GROUP_FF_BIT | OFDP_GROUP_INDIRECT_BIT;
    mGroupFeatures.capabilities = OFDP_SELECT_LIVENESS;

    mGroupFeatures.max_groups[OFDP_GROUP_ALL] = OFDP_MAX_ALL_GROUPS;
    mGroupFeatures.max_groups[OFDP_GROUP_SELECT] = OFDP_MAX_SELECT_GROUPS;
    mGroupFeatures.max_groups[OFDP_GROUP_INDIRECT] = OFDP_MAX_INDIRECT_GROUPS;
    mGroupFeatures.max_groups[OFDP_GROUP_FF] = OFDP_MAX_FF_GROUPS;

    mGroupFeatures.actions[OFDP_GROUP_ALL] = 0;
    mGroupFeatures.actions[OFDP_GROUP_SELECT] = 0;
    mGroupFeatures.actions[OFDP_GROUP_INDIRECT] = 0;
    mGroupFeatures.actions[OFDP_GROUP_FF] = 0;

    DBG_TRACE_EXIT;
}

OfDpGroupTable::~OfDpGroupTable() {
    DBG_TRACE_ENTER;

    DBG_TRACE_EXIT;
}

OfDpReturn OfDpGroupTable::ValidateGroup(const OfDpGroupT &group) {

    // Check for unsupported group type.
    if (group.type == OFDP_GROUP_NONE ||
        group.type == OFDP_GROUP_ALL ||
        group.type == OFDP_GROUP_SELECT) {

        return OFDP_GROUP_BAD_TYPE;
    }
    if (group.group_id > OFDP_MAX_GROUP_ID) {
        return OFDP_GROUP_INVALID_GROUP;
    }

    // Only 2 group buckets can be supported for fast failover case.
    // For indirect case, one bucket.
    if (group.type == OFDP_GROUP_FF) {
        if (group.num_buckets > 2) {
            return OFDP_GROUP_BUCKETS_TOOMANY;
        }
    } else if (group.num_buckets > 1) {
        return OFDP_GROUP_BUCKETS_TOOMANY;
    }

    uint32_t num_buckets = group.num_buckets;
    // check watch_port matches output port for each bucket, otherwise reject.


    for (uint32_t i = 0; i < num_buckets; i++) {

        bool outgrouppresent = false;
        bool outportpresent = false;
        uint32_t outport = 0;
        uint32_t outgroup = 0;

        uint32_t numactions = group.buckets[i].num_actions;
        uint32_t watchport = group.buckets[i].watch_port;
        uint32_t watchgroup = group.buckets[i].watch_group;

        for (uint32_t j = 0; j < numactions; j++) {
            uint8_t command = group.buckets[i].actions[j].command;
            if (command == OFDP_OUTPUT) {
                outportpresent = true;
                outport = group.buckets[i].actions[j].value.output.port;

            } else if (command == OFDP_GROUP) {
                outgrouppresent = true;
                outgroup = group.buckets[i].actions[j].value.group_id;
            }
        }

        if (!outgrouppresent && !outportpresent)  {
            DBG_ERROR((" Invalid bucket %u, , Port and Group absent",i));
            return OFDP_GROUP_BAD_WATCH;
        }

        if (outgrouppresent && watchgroup != outgroup) {
            DBG_ERROR((" Invalid bucket %u, watchgroup %u, setgroup %u",i,watchgroup,outgroup));
            return OFDP_GROUP_BAD_WATCH;
        }

        if (outportpresent && watchport != outport) {
            DBG_ERROR((" Invalid bucket %u, watchport %u, setport %u",i,watchport,outport));
            return OFDP_GROUP_BAD_WATCH;
        }
    }

    // Check for chaining in group actions
    if (!(mGroupFeatures.capabilities & OFDP_CHAINING)) {

        for (uint32_t i = 0; i < num_buckets; i++) {
            uint32_t num_actions = group.buckets[i].num_actions;

            for (uint32_t j = 0; j < num_actions; j++) {
                if (group.buckets[i].actions[j].command == OFDP_GROUP) {
                    return OFDP_GROUP_CHAINING_UNSUPPORTED;

                }
            }
        }
    }
    return OFDP_OK;
}

OfDpReturn OfDpGroupTable::DeleteGroup(uint32_t groupId, OfDpResponseT &response) {
    std::set<OfDpFlowT *> flows;
    OfDpReturn ret = OFDP_OK;
    mLogicalSwitch->getTable().getFlows(groupId, flows);
    if (flows.empty() == false) {
        OfDpFlowT flow;
        flow.ls_idx = mLogicalSwitch; /// replace with ls*
        std::set<OfDpFlowT *> inSet;
        for(auto i : flows) inSet.insert(i);

        if ((ret = mpDPIntf->DeleteFlowsFromDb(flows, flow, response)) == OFDP_OK) {
            std::set<uint32_t> outSet;
            mLogicalSwitch->getTable().remove_set(inSet, outSet);
        } else {
            DBG_ERROR((" Fail to delete group %u", groupId));
        }
    }

    if ((ret = mpDPIntf->DeleteGroupFromDb(groupId, response)) == OFDP_OK) {
        if (delete_groupTable(groupId) == false) {
            DBG_ERROR((" Flows exist for group %u", groupId));
        }
    }
    return OFDP_OK;
}


// Update group
OfDpReturn OfDpGroupTable::updateGroup(OfDpGroupT &group, OfDpResponseT &resp) {
    DBG_TRACE_ENTER;
    OfDpReturn ret = OFDP_OK;
    
    // update lsIndex
    group.ls_idx = mLogicalSwitch;
    // Add/remove group
    if (group.command == OFDP_CONFIG_CREATE) {
        //validate

        if ((ret = ValidateGroup(group)) != OFDP_OK) {
            DBG_ERROR((" Validation failure %s",to_string(ret)));
            return ret;
        }

        if(add_groupTable(group)){
            return mpDPIntf->AddToDb(group, resp);
        }else{
            DBG_ERROR((" Validation failure %s",to_string(OFDP_GROUP_EXISTS)));
            return OFDP_GROUP_EXISTS;
        }

    } else if (group.command == OFDP_CONFIG_REMOVE) {
        if (group.group_id != OFDP_ALL_GROUP_ID && group.group_id > OFDP_MAX_GROUP_ID) {
            return OFDP_GROUP_INVALID_GROUP;
        }

        if (group.group_id == OFDP_ALL_GROUP_ID) {
            //get all groups and flows.

            auto& groups = getAllGroups();
            for (auto& i : groups | boost::adaptors::map_values) {
                if (DeleteGroup((*i).GetConfigObject().group_id, resp) != OFDP_OK) {
                    DBG_ERROR((" Fail to delete group %u",group.group_id));
                }
            }
        } else {
            if (!is_groupPresent(group)) {
                DBG_ERROR((" Fail to delete group %u",group.group_id));
                return OFDP_GROUP_INVALID_GROUP;
            }

            // Have to delete all flows from index table.

            if (DeleteGroup(group.group_id, resp) != OFDP_OK) {

                DBG_ERROR((" Fail to delete group %u",group.group_id));
            }
        }
    } else if (group.command == OFDP_CONFIG_MODIFY) {
        //validate
        OfDpReturn ret = OFDP_OK;

        if (!is_groupPresent(group)) {
            DBG_ERROR((" Fail to delete group %u",group.group_id));
            return OFDP_GROUP_INVALID_GROUP;
        }

        if ((ret = ValidateGroup(group)) == OFDP_OK) {
            std::set<OfDpFlowStatsT *> flowIdList;
            mLogicalSwitch->getTable().getFlows(group.group_id, flowIdList);
            ret = mpDPIntf->UpdateDb(group,
                                     flowIdList,
                                     resp);
        } else {
            DBG_ERROR((" Validation failure %s",to_string(ret)));
        }

    } else {
        ret = OFDP_ERR_UNKNOWN_COMMAND;
    }

    DBG_TRACE_RET(ret);
}

// Get group
OfDpGroup* OfDpGroupTable::getGroup(uint32_t grpId) {
    DBG_TRACE_ENTER;

    DBG_TRACE_ENTER;
    return (NULL);
}


OfDpReturn
OfDpGroupTable::getFeatures(OfDpGroupFeature &rFeatures) {
    memcpy(&rFeatures, &mGroupFeatures, sizeof(OfDpGroupFeature));
    return OFDP_OK;
}

OfDpReturn
OfDpGroupTable::getStat(OfDpGroupDescStatsT &info,
                        OfConnManager &ofSwitch,
                        OfDpResponseT &response) {

    return mpDPIntf->GetRecordsFromDb(info, ofSwitch, response);
}

OfDpReturn
OfDpGroupTable::getStat(OfDpGroupStatsT &info,
                        OfConnManager &ofSwitch,
                        OfDpResponseT &response) {

    return mpDPIntf->GetRecordsFromDb(info, ofSwitch, response);
}

bool
OfDpGroupTable::add_groupTable(const OfDpGroupT &grp) {
    try {
        return mGroups.Attach(new OfDpGroup(grp)); 
    } catch (...) {
    }
    return false;
}

bool
OfDpGroupTable::delete_groupTable(int groupId) {
    // cannot remove if flows are associated to the group id
    // In that case the user has to delete the flows and then the group table.

    //Make a search through the flowtable to get the associated flows.
    // Create a wildcarded search entry.

    OfDpMatchT match;

    std::set<OfDpFlowStatsT *> rsltSet;
    mLogicalSwitch->getTable().find_loose(&match, OFDP_OFPP_NONE, groupId, OFDP_COOKIE_NONE, OFDP_COOKIE_NONE, rsltSet);

    //std::cout << "Deleting group:" << groupId << " lsId:" << (uint32_t)mpCofaLS << std::endl;
    if (rsltSet.empty() == false) {
        std::cout << "Group Id is not empty, flows still associated to group Id" << std::endl;
        return false;
    }

    mGroups.Detach(groupId);

    return true;
}

bool
OfDpGroupTable::is_groupPresent(const OfDpGroupT &grp) const {
    return ((mGroups.find(grp.group_id) == mGroups.end())? false : true);
}


