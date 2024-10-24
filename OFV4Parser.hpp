#ifndef _IOPENFLOWV4IMPL_H
#define _IOPENFLOWV4IMPL_H
/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */

#include "AbstractParser.hpp"
#include "openflow_1.3.h"

//Uses Decorator Pattern - It is the concrete class for AbstractParser
class OFV4Parser : public AbstractParser {
public:
    OFV4Parser(AbstractConn& rVconn,const uint8_t ver = OFDP_VERSION_V4);

    virtual ~OFV4Parser();

    void DumpTableFeatureStatsT(const OfDpTableFeatureStatsT info);

    /**
     * It fans out to specific message handler
     * 
     * @author sraihan 
     * 
     * @param msg 
     * @param now 
     * @param out 
     * 
     * @return bool 
     */
    virtual bool process_of_msg(const ofp_message&  msg, const time_t now, std::ostream& out);

    /**
     * outgoing packets to controller
     * 
     * @author sraihan 
     * 
     * @param info 
     * 
     * @return bool 
     */
    virtual bool SendPacketIn(const OfDpPacketInT& info) ;

    /**
     * outgoing FlowRemoved status to controller
     * 
     * @author sraihan 
     * 
     * @param info 
     * 
     * @return bool 
     */
    virtual bool SendFlowRemoved(const OfDpFlowRemovedT& info) ;

    /**
     * outgoing PortStatus to controller
     * 
     * @author sraihan 
     * 
     * @param info 
     * 
     * @return bool 
     */
    virtual bool SendPortStatus(const OfDpPortStatusT& info);

    /**
     * msg.type == OpenflowV4::OFPT_BARRIER_REQUEST - Needs sync 
     * with controller 
     * 
     * @author sraihan 
     * 
     * @param xid 
     * 
     * @return bool 
     */
    virtual bool SendBarrierReply(const uint32_t xid);

    /**
     * OfDpSwitchFeaturesT
     * 
     * @author sraihan 
     * 
     * @param info 
     * @param response 
     * 
     * @return bool 
     */
    virtual bool GetFeatureReply(const OfDpSwitchFeaturesT& info, const OfDpResponseT& response);

    /**
     * OfDpSwitchConfigT
     * 
     * @author sraihan 
     * 
     * @param info 
     * @param response 
     * 
     * @return bool 
     */
    virtual bool GetConfigReply(const OfDpSwitchConfigT& info, const OfDpResponseT& response);

    /**
     * 
     * 
     * @author sraihan 
     * 
     * @param response 
     * 
     * @return bool 
     */
    virtual bool SetConfigReply(const OfDpResponseT& response);

    /**
     * 
     * 
     * @author sraihan 
     * 
     * @param response 
     * 
     * @return bool 
     */
    virtual bool PacketOutReply(const OfDpResponseT& response);

    /**
     * OfDpFlowT
     * 
     * @author sraihan 
     * 
     * @param info 
     * @param response 
     * 
     * @return bool 
     */
    virtual bool ResourceModReply(const OfDpFlowT & info, const OfDpResponseT& response);

    /**
     * OfDpGroupT
     * 
     * @author sraihan 
     * 
     * @param info 
     * @param response 
     * 
     * @return bool 
     */
    virtual bool ResourceModReply(const OfDpGroupT & info, const OfDpResponseT& response);

    /**
     * OfDpPortInfoT
     * 
     * @author sraihan 
     * 
     * @param info 
     * @param response 
     * 
     * @return bool 
     */
    virtual bool ResourceModReply(const OfDpPortInfoT & info, const OfDpResponseT& response);

    /**
     * OfDpDescStatsT
     * 
     * @author sraihan 
     * 
     * @param stats 
     * @param response 
     * 
     * @return bool 
     */
    virtual bool GetStatReply( const OfDpDescStatsT &stats, const OfDpResponseT& response);

    /**
     * OfDpFlowStatsT
     * 
     * @author sraihan 
     * 
     * @param stats 
     * @param response 
     * 
     * @return bool 
     */
    virtual bool GetStatReply( const OfDpFlowStatsT &stats, const OfDpResponseT& response);

    /**
     * OfDpAggregateStatsT
     * 
     * @author sraihan 
     * 
     * @param stats 
     * @param response 
     * 
     * @return bool 
     */
    virtual bool GetStatReply( const OfDpAggregateStatsT &stats, const OfDpResponseT& response);

    /**
     * OfDpPortStatsT reply
     * 
     * @author sraihan 
     * 
     * @param stats 
     * @param response 
     * 
     * @return bool 
     */
    virtual bool GetStatReply( const OfDpPortStatsT &stats, const OfDpResponseT& response);

    /**
     * OfDpQueueStatsT reply
     * 
     * @author sraihan 
     * 
     * @param stats 
     * @param response 
     * 
     * @return bool 
     */
    virtual bool GetStatReply(const OfDpQueueStatsT &stats, const OfDpResponseT& response);

    /**
     * OfDpTableStatsT reply
     * 
     * @author sraihan 
     * 
     * @param stats 
     * @param response 
     * 
     * @return bool 
     */
    virtual bool GetStatReply( const OfDpTableStatsT &stats, const OfDpResponseT& response);

    /**
     * OfDpPortListT reply
     * 
     * @author sraihan 
     * 
     * @param stats 
     * @param response 
     * 
     * @return bool 
     */
    virtual bool GetStatReply( const OfDpPortListT &stats, const OfDpResponseT& response);

    /**
     * OfDpTableFeatureStatsT reply
     * 
     * @author sraihan 
     * 
     * @param info 
     * @param response 
     * 
     * @return bool 
     */
    virtual bool GetStatReply(const OfDpTableFeatureStatsT &info,  const OfDpResponseT& response);

    /**
     * OfDpGroupStatsT reply
     * 
     * @author sraihan 
     * 
     * @param stats 
     * @param response 
     * 
     * @return bool 
     */
    virtual bool GetStatReply(const OfDpGroupStatsT& stats, const OfDpResponseT& response);

    /**
     * OfDpQueueConfigT reply
     * 
     * @author sraihan 
     * 
     * @param info 
     * @param response 
     * 
     * @return bool 
     */
    virtual bool GetConfigReply( const OfDpQueueConfigT& info, const OfDpResponseT& response);

    /**
     * OfDpGroupDescStatsT reply
     * 
     * @author sraihan 
     * 
     * @param info 
     * @param response 
     * 
     * @return bool 
     */
    virtual bool GetStatReply(const OfDpGroupDescStatsT &info, const OfDpResponseT& response);

    /**
     * OfDpGroupFeatureStatsT reply
     * 
     * @author sraihan 
     * 
     * @param info 
     * @param response 
     * 
     * @return bool 
     */
    virtual bool GetStatReply(const OfDpGroupFeatureStatsT &info, const OfDpResponseT& response);

    /**
     * msg.type == OpenflowV4::OFPT_FEATURES_REQUEST
     * @see ofp_switch_features
     * @author sraihan 
     * 
     * @param msg 
     * @param out 
     * 
     * @return bool 
     */
    virtual bool GetFeature(const ofp_message & msg, std::ostream& out);

    /**
     * msg.type == OpenflowV4::OFPT_CONFIG_REQUEST
     * @see ofp_switch_config
     * @author sraihan 
     * 
     * @param msg 
     * @param out 
     * 
     * @return bool 
     */
    virtual bool GetConfig(const ofp_message & msg, std::ostream& out);

    /**
     * msg.type == OpenflowV4::OFPT_PACKET_OUT
     * @see ofp_packet_out
     * @author sraihan 
     * 
     * @param msg 
     * @param out 
     * 
     * @return bool 
     */
    virtual bool PacketOut(const ofp_message & msg, std::ostream& out);

    /**
     * msg.type == OpenflowV4::OFPT_FLOW_MOD
     * @see ofp_flow_mod
     * @author sraihan 
     * 
     * @param msg 
     * @param out 
     * 
     * @return bool 
     */
    virtual bool FlowMod(const ofp_message & msg, std::ostream& out);

    /**
     * msg.type == OpenflowV4::OFPT_PORT_MOD
     * @see ofp_port_mod
     * @author sraihan 
     * 
     * @param msg 
     * @param out 
     * 
     * @return bool 
     */
    virtual bool PortMod(const ofp_message & msg, std::ostream& out);

    /**
     * msg.type == OpenflowV4::OFPT_GROUP_MOD
     * @see ofp_group_mod
     * @author sraihan 
     * 
     * @param msg 
     * @param out 
     * 
     * @return bool 
     */
    virtual bool GroupMod(const ofp_message & msg, std::ostream& out);

    /**
     * msg.type == OpenflowV4::OFPT_TABLE_MOD
     * @see ofp_table_mod
     * @author sraihan 
     * 
     * @param msg 
     * @param out 
     * 
     * @return bool 
     */
    virtual bool TableMod(const ofp_message & msg, std::ostream& out);

    /**
     * msg.type == OpenflowV4::OFPT_STATS_REQUEST
     * @see ofp_stats
     * @author sraihan 
     * 
     * @param msg 
     * @param out 
     * 
     * @return bool 
     */
    virtual bool StatsReq(const ofp_message & msg, std::ostream& out);

    /**
     * msg.type == OpenflowV4::OFPT_QUEUE_GET_CONFIG_REQUEST
     * @see ofp_queue_get_config_request
     * @author sraihan 
     * 
     * @param msg 
     * @param out 
     * 
     * @return bool 
     */
    virtual bool QueueConfReq(const ofp_message & msg, std::ostream& out);

    /**
     * msg.type == OpenflowV4::OFPT_ROLE_REQUEST
     * @see ofp_role_request
     * @author sraihan 
     * 
     * @param msg 
     * @param out 
     * 
     * @return bool 
     */
    virtual bool RoleReq(const ofp_message & msg, std::ostream& out);

    /**
     * msg.type == OpenflowV4::OFPT_SET_CONFIG
     * @see ofp_switch_config
     * @author sraihan 
     * 
     * @param msg 
     * @param out 
     * 
     * @return bool 
     */
    virtual bool SetConfig(const ofp_message & msg, std::ostream& out);

    /**
     * msg.type == OpenflowV4::OFPT_STATS_REQUEST
     * @see ofp_desc_stats
     * @author sraihan 
     * 
     * @param msg 
     * @param out 
     * 
     * @return bool 
     */
    virtual bool GetDescStat(const ofp_message& param, std::ostream& out);

    /**
     * msg.type == OpenflowV4::OFPT_STATS_REQUEST
     * @see ofp_flow_stats
     * @author sraihan 
     * 
     * @param msg 
     * @param out 
     * 
     * @return bool 
     */
    virtual bool GetFlowStat(const ofp_message& msg, std::ostream& out);

    /**
     * msg.type == OpenflowV4::OFPT_STATS_REQUEST
     * @see ofp_aggregate_stats
     * @author sraihan 
     * 
     * @param msg 
     * @param out 
     * 
     * @return bool 
     */
    virtual bool GetAggregateStat(const ofp_message& msg, std::ostream& out);

    /**
     * msg.type == OpenflowV4::OFPT_STATS_REQUEST
     * @see ofp_table_features_stats
     * @author sraihan 
     * 
     * @param msg 
     * @param out 
     * 
     * @return bool 
     */
    virtual bool GetTableFeatureStat(const ofp_message& param, std::ostream& out);

    /**
     * msg.type == OpenflowV4::OFPT_STATS_REQUEST
     * @see ofp_port_desc_stats
     * @author sraihan 
     * 
     * @param msg 
     * @param out 
     * 
     * @return bool 
     */
    virtual bool GetPortList(const ofp_message& param, std::ostream& out);

    /**
     * msg.type == OpenflowV4::OFPT_STATS_REQUEST
     * @see ofp_table_stats
     * @author sraihan 
     * 
     * @param msg 
     * @param out 
     * 
     * @return bool 
     */
    virtual bool GetTableStat(const ofp_message& param, std::ostream& out);

    /**
     * msg.type == OpenflowV4::OFPT_STATS_REQUEST
     * @see ofp_port_stats
     * @author sraihan 
     * 
     * @param msg 
     * @param out 
     * 
     * @return bool 
     */
    virtual bool GetPortStat(const ofp_message& param, std::ostream& out);

    /**
     * msg.type == OpenflowV4::OFPT_STATS_REQUEST
     * @see ofp_queue_stats
     * @author sraihan 
     * 
     * @param msg 
     * @param out 
     * 
     * @return bool 
     */
    virtual bool GetQueueStat(const ofp_message& param, std::ostream& out);

    /**
     * msg.type == OpenflowV4::OFPT_STATS_REQUEST
     * @see ofp_group_stats
     * @author sraihan 
     * 
     * @param msg 
     * @param out 
     * 
     * @return bool 
     */
    virtual bool GetGroupStat(const ofp_message& param, std::ostream& out);

    /**
     * msg.type == OpenflowV4::OFPT_STATS_REQUEST
     * @see ofp_group_desc_stats
     * @author sraihan 
     * 
     * @param msg 
     * @param out 
     * 
     * @return bool 
     */
    virtual bool GetGroupDescStat(const ofp_message& param, std::ostream& out);

    /**
     * msg.type == OpenflowV4::OFPT_STATS_REQUEST
     * @see ofp_group_features_stats
     * @author sraihan 
     * 
     * @param msg 
     * @param out 
     * 
     * @return bool 
     */
    virtual bool GetGroupFeatureStat(const ofp_message& param, std::ostream& out);

    /**
     * datapath's error code to of error code for a given error type
     * 
     * @author sraihan 
     * 
     * @param retVal 
     * @param type 
     * @param code 
     * 
     * @return bool 
     */
    virtual bool TranslateOfDpReturnToOFVal(OfDpReturn retVal, uint16_t& type, uint16_t& code) const;

public:
    /**
     *  
     * @author sraihan 
     * 
     * @return uint8_t 
     */
    virtual uint8_t get_hello_type() const;
    /**
     *  
     * @author sraihan 
     * 
     * @return uint16_t 
     */
    virtual uint16_t get_hello_failed_type() const;
    /**
     *  
     * @author sraihan 
     * 
     * @return uint16_t 
     */
    virtual uint16_t get_hello_failed_code() const;
    /**
     *  
     * @author sraihan 
     * 
     * @return uint8_t 
     */
    virtual uint8_t get_echo_type() const;
    /**
     *  
     * @author sraihan 
     * 
     * @return uint8_t 
     */
    virtual uint8_t get_echo_reply_type() const;
    /**
     *  
     * @author sraihan 
     * 
     * @return uint8_t 
     */
    virtual uint8_t get_error_type() const; // Top level msg type
    /**
     *  
     * @author sraihan 
     * 
     * @return uint16_t 
     */
    virtual uint16_t get_generic_error_sub_type() const;
    /**
     *  
     * @author sraihan 
     * 
     * @return uint16_t 
     */
    virtual uint16_t get_generic_error_sub_code() const;

};// iOpenFlowV4Impl

#endif
