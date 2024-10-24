#ifndef _IOPENFLOW_H
#define _IOPENFLOW_H
/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include "OfpHeader.h"
#include "OfDpDefine.hpp"
#include "openflow_1.3.h"

class AbstractConn;

#define PREPARE_OFDP_RESPONSE(param) 	const size_t resp_len = (((param).get_length() < 64) ? (param).get_length() : 64);\
										OfDpResponseT response(mVconn.GetSocketFd(), (const uint8_t*const)&(param), resp_len);

#define PREPARE_OFDP_RESPONSE_PTR(param) const size_t resp_len = (((param).get_length() < 64) ? (param).get_length() : 64);\
										 OfDpResponseT *response = new OfDpResponseT(mVconn.GetSocketFd(), (const uint8_t*const)&(param), resp_len);

/**
 *@class AbstractParser
 *@author Syed M Raihan
 *@date 10/4/2013
 *@brief Uses Decorator Pattern and a slave class to Connections(@class AbstractConn)
 */
class AbstractParser {
public:
    explicit AbstractParser(AbstractConn& rVconn, const uint8_t ver);

    virtual ~AbstractParser();

    void ResetSendEchoFlag() { mbSendEchoFlag = false;}

    //\param	ofp_message
    //\desc 	abstract function placeholder that must be implemented by derived
    virtual bool process_of_msg(const ofp_message&  msg, const time_t now, std::ostream& ) = 0;

    //\param	none
    //\desc 	outgoing msg.type == OpenflowV3::OFPT_ECHO_REQUEST
    virtual bool send_echo(const uint32_t xid = 0);

    //\param	OfDpPortStatus
    //\desc 	outgoing async msg
    virtual bool SendPortStatus(const OfDpPortStatusT& info) = 0;

    //\param	OfDpPacketInT
    //\desc 	outgoing async msg
    virtual bool SendPacketIn(const OfDpPacketInT& info) = 0;

    //\param	OfDpFlowRemovedT
    //\desc 	outgoing async msg
    virtual bool SendFlowRemoved(const OfDpFlowRemovedT& info) = 0;

    //\param    xid
    //\desc     outgoing async msg
    virtual bool SendBarrierReply(const uint32_t xid) = 0;

    // Null msg or response
    virtual bool GetFeatureReply(const OfDpSwitchFeaturesT& info, const OfDpResponseT& response) = 0;

    // Null msg or response
    virtual bool GetConfigReply(const OfDpSwitchConfigT& info, const OfDpResponseT& response) = 0;

    // Null msg or response
    virtual bool SetConfigReply(const OfDpResponseT& response) = 0;

    // Null msg or response
    virtual bool PacketOutReply(const OfDpResponseT& response) = 0;

    // Null msg or response
    virtual bool ResourceModReply(const OfDpFlowT & info, const OfDpResponseT& response) = 0;

    // Null msg or response
    virtual bool ResourceModReply(const OfDpGroupT & info, const OfDpResponseT& response) =0;

    // Null msg or response
    virtual bool ResourceModReply(const OfDpPortInfoT & info, const OfDpResponseT& response) = 0;

    // Get description stats
    virtual bool GetStatReply( const OfDpDescStatsT &stats, const OfDpResponseT& response) = 0;

    // Get flow stats
    virtual bool GetStatReply( const OfDpFlowStatsT &stats, const OfDpResponseT& response) = 0;

    // Get aggregate flow stats
    virtual bool GetStatReply( const OfDpAggregateStatsT &stats, const OfDpResponseT& response) = 0;

    // Get table stats
    virtual bool GetStatReply( const OfDpTableStatsT &stats, const OfDpResponseT& response) = 0;

    // Get port stats
    virtual bool GetStatReply( const OfDpPortStatsT &stats, const OfDpResponseT& response) = 0;

    // Get queue stats
    virtual bool GetStatReply(const OfDpQueueStatsT &stats, const OfDpResponseT& response)=0;

    // Queue config
    virtual bool GetConfigReply( const OfDpQueueConfigT& info, const OfDpResponseT& response)=0;

    // Get stats
    virtual bool GetStatReply(const OfDpGroupDescStatsT &info, const OfDpResponseT& response)=0;

    // Get stats
    virtual bool GetStatReply(const OfDpGroupFeatureStatsT &info, const OfDpResponseT& response)= 0;

    // Get stats
    virtual bool GetStatReply(const OfDpGroupStatsT &info,  const OfDpResponseT& response) = 0;

    // Get stats 1.3+
    virtual bool GetStatReply(const OfDpTableFeatureStatsT &info,  const OfDpResponseT& response) =0;

    // Get table stats 1.3 +
    virtual bool GetStatReply( const OfDpPortListT &stats, const OfDpResponseT& response) =0;

    virtual bool report_bad_message(const ofp_message& msg,
                                    const uint16_t type,
                                    const uint16_t code,
                                    std::ostream& );

    //\param	ofp_message
    //\desc 	incoming msg.type == OpenflowV3::OFPT_ECHO_REQUEST
    virtual bool echo(const ofp_message & msg, const time_t now, std::ostream&);

    //\param	ofp_message
    //\desc 	incoming msg.type == OpenflowV3::OFPT_ECHO_REPLY
    virtual bool echo_reply(const ofp_message & msg, const time_t now, std::ostream&);

    //\param	ofp_message
    //\desc 	outgoing msg.type == OpenflowV3::OFPT_ECHO_REPLY
    virtual bool send_echo_reply(ofp_message & msg);

    inline uint8_t get_wire_version() const;
    virtual uint8_t get_hello_type() const = 0;
    virtual uint16_t get_hello_failed_type() const = 0;
    virtual uint16_t get_hello_failed_code() const = 0;
    virtual uint8_t get_echo_type() const = 0;
    virtual uint8_t get_echo_reply_type() const = 0;
    virtual uint8_t get_error_type() const = 0; // Top level error msg type
    virtual uint16_t get_generic_error_sub_type() const = 0;
    virtual uint16_t get_generic_error_sub_code() const = 0;

protected:
    const uint8_t mWireVersion;
    bool mbSendEchoFlag;
    AbstractConn&  mVconn; // passed by owner of iOpenFlow class of objects
    uint64_t make_dpid();
    bool BarrierReq(const ofp_message & msg, std::ostream& out);
	
    /**
     * 
     * @author sraihan (8/4/2014)
     * @return bool 
     */
    template<typename ofp_struct_type>
    bool modify_msg_validate(const ofp_message& msg, std::ostream& out) ;

    /**
     * 
     * @author sraihan (8/4/2014)
     * @return bool 
     */
    template<typename ofp_struct_type>
    bool read_msg_validate(const ofp_message& msg, std::ostream& out) ;
    	
private:
    AbstractParser(const AbstractParser& ) = delete;
    AbstractParser& operator = (const AbstractParser& ) = delete;
};

inline uint8_t AbstractParser::get_wire_version() const {
    return mWireVersion;
}

extern template
bool AbstractParser::modify_msg_validate<OpenflowV4::ofp_packet_out>(const ofp_message& msg, std::ostream& out) ;

extern template
bool AbstractParser::modify_msg_validate<OpenflowV4::ofp_flow_mod>(const ofp_message& msg, std::ostream& out) ;

extern template
bool AbstractParser::modify_msg_validate<OpenflowV4::ofp_group_mod>(const ofp_message& msg, std::ostream& out) ;

extern template
bool AbstractParser::modify_msg_validate<OpenflowV4::ofp_port_mod>(const ofp_message& msg, std::ostream& out) ;

extern template
bool AbstractParser::modify_msg_validate<OpenflowV4::ofp_switch_config>(const ofp_message& msg, std::ostream& out) ;

extern template
bool AbstractParser::read_msg_validate<ofp_get_config>(const ofp_message& msg, std::ostream& out) ;

extern template
bool AbstractParser::read_msg_validate<OpenflowV4::ofp_queue_get_config_request>(const ofp_message& msg, std::ostream& out) ;

#endif
