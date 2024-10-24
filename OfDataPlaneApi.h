#ifndef _OfDataPlaneApi_h_
#define _OfDataPlaneApi_h_
/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include <set>
#include "OfConfig.hpp"
#include "OfDpDefine.hpp"

/**
 * This file declares Dataplane interface which is a singleton 
 * It must be implemented by a singleton class which has own 
 * message loop to communicate with datapath 
 * 
 * @author sraihan ()
 */

class OfConnManager;

/**
 * @class OfDataPlaneApi
 * 
 * @author sraihan ()
 */
class OfDataPlaneApi  {
public:
	virtual ~OfDataPlaneApi(){}

    /**
     * Add a Flow in the datapath must be override to support 
     * FlowMod 
     *  
     * @author sraihan () 
     * 
     * @param flow 
     * @param response 
     * 
     * @return OfDpReturn 
     */
    virtual OfDpReturn AddToDb(const OfDpFlowT &flow,
                               OfDpResponseT& response){return OFDP_OK;}
    /**
     * Deletes flows from Datapath
     * 
     * @author sraihan ()
     * 
     * @param rsltSet 
     * @param flow 
     * @param response 
     * 
     * @return OfDpReturn 
     */
    virtual OfDpReturn DeleteFlowsFromDb(std::set<OfDpFlowStatsT*> &rsltSet,
                                         const OfDpFlowT &flow,
                                         OfDpResponseT& response){return OFDP_OK;}
    /**
     * Updates existing flows 
     * 
     * @author sraihan ()
     * 
     * @param rsltSet 
     * @param flow 
     * @param response 
     * 
     * @return OfDpReturn 
     */
    virtual OfDpReturn UpdateDb(std::set<OfDpFlowStatsT*> &rsltSet,
                                const OfDpFlowT &flow,
                                OfDpResponseT& response){return OFDP_OK;}
    /**
     * Update a set flows
     * 
     * @author sraihan ()
     * 
     * @param rsltSet 
     * @param ofSwitch 
     * @param response 
     * 
     * @return OfDpReturn 
     */
    virtual OfDpReturn GetRecordsFromDb(std::set<OfDpFlowStatsT*> &rsltSet,
                                        OfConnManager &ofSwitch,
                                        OfDpResponseT& response) const {return OFDP_OK;}
    /**
     * Add a Group in the datapath
     * 
     * @author sraihan ()
     * 
     * @param group 
     * @param response 
     * 
     * @return OfDpReturn 
     */
    virtual OfDpReturn AddToDb(const OfDpGroupT &group,
                               OfDpResponseT& response){return OFDP_OK;}
    /**
     * Update a Group and a Set of flows associated with it
     * 
     * @author sraihan ()
     * 
     * @param group 
     * @param rFlowSet 
     * @param response 
     * 
     * @return OfDpReturn 
     */
    virtual OfDpReturn UpdateDb(const OfDpGroupT &group,
                                std::set<OfDpFlowStatsT*> &rFlowSet,
                                OfDpResponseT& response){return OFDP_OK;}
    /**
     * Delete a Group from DB
     * 
     * @author sraihan ()
     * 
     * @param groupId 
     * @param response 
     * 
     * @return OfDpReturn 
     */
    virtual OfDpReturn DeleteGroupFromDb(uint32_t groupId,
                                         OfDpResponseT& response){return OFDP_OK;}
    /**
     * Get Group Desc stats
     * TODO: Consider refactoring as it exposes ConnectionManager 
     * Or think about implementing visitor pattern 
     *  
     * @author sraihan ()
     * 
     * @param group 
     * @param ofSwitch 
     * @param response 
     * 
     * @return OfDpReturn 
     */
    virtual OfDpReturn GetRecordsFromDb(const OfDpGroupDescStatsT &group,
                                        OfConnManager &ofSwitch,
                                        OfDpResponseT& response) const {return OFDP_OK;}
    /**
     * Get Group stats
     * 
     * @author sraihan ()
     * 
     * @param group 
     * @param ofSwitch 
     * @param response 
     * 
     * @return OfDpReturn 
     */
    virtual OfDpReturn GetRecordsFromDb(const OfDpGroupStatsT &group,
                                        OfConnManager &ofSwitch,
                                        OfDpResponseT& response) const {return OFDP_OK;}

    /**
     * Provides port information - if port is not available returns 
     * default values 
     * Datapath here can offer puuling the port information 
     * 
     * @author sraihan ()
     * 
     * @param port_no 
     * @param portInfo 
     * 
     * @return bool 
     */
    virtual bool GetPort(OfDpPortInfoT& portInfo) const {return true;}

    /**
     * Datapath should call this function to send port status update 
     * and update port database 
     * 
     * @author sraihan ()
     * 
     * @param portInfo 
     */
    virtual bool PortEventCallback(const OfDpPortInfoT& portInfo) {return true;};
};

#endif
