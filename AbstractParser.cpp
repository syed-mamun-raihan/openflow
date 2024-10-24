/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include "AbstractParser.hpp"
#include "AbstractConn.hpp"
#include "OfpHeader.h"
#include "OfConnStateManager.hpp"
#include "ofDebug.h"
#include "OfDpLogicalSw.h"

extern uint32_t  gBarrierCount;
extern uint32_t gFlowCount;

//CTOR
AbstractParser::AbstractParser(AbstractConn& rVconn, const uint8_t ver):mWireVersion(ver),
mbSendEchoFlag(false),
mVconn(rVconn)
{
}

//DTOR
AbstractParser::~AbstractParser()
{
}

/**
 * incoming msg.type == OFPT_BARRIER_REQUEST
 * 
 * @author sraihan (8/4/2014)
 * 
 * @param msg 
 * @param out 
 * 
 * @return bool 
 */
bool AbstractParser::BarrierReq(const ofp_message & msg, std::ostream& out) {
    out << "\n Barrier Req, xid: " << msg.get_xid() << "\n";
    if ( gDbgMask & DBG_ERR_OF )
        out << " barrier count= "<< ++gBarrierCount << ", Flow count= " << gFlowCount << "\n";
    if ( mVconn.mConnMgr.mLogicalSw.BarrierRequest(mVconn.GetSocketFd(), msg.xid) )
        return SendBarrierReply(msg.xid);
    else
        return true; // async call
}

/**
 * outgoing msg.type == OpenflowV3::OFPT_ECHO_REQUEST
 * 
 * @author sraihan (8/4/2014)
 * 
 * @param xid 
 * 
 * @return bool 
 */
bool AbstractParser::send_echo(const uint32_t xid)
{
    if ( !mbSendEchoFlag ) {
        ofp_message echo_req;
        echo_req.set_length(sizeof(echo_req));
        echo_req.set_xid(xid);
        echo_req.version = get_wire_version();
        echo_req.type = get_echo_type();
        mbSendEchoFlag = mVconn.Send(echo_req);
    }
    return mbSendEchoFlag;
}

/**
 * incoming msg.type == OpenflowV3::OFPT_ECHO_REPLY
 * 
 * @author sraihan (8/4/2014)
 * 
 * @param msg 
 * @param now 
 * 
 * @return bool 
 */
bool AbstractParser::echo_reply(const ofp_message & msg, const time_t now, std::ostream&)
{
    mbSendEchoFlag = false; // reset echo send flag as we got our response
    mVconn.mpStateMgr->TransitionTo(OfConnStateManager::S_ACTIVE, now);
    return true;
}

/**
 * incoming msg.type == OpenflowV3::OFPT_ECHO_REQUEST 
 * outgoing msg.type == OFPT_ECHO_REPLY 
 * @author sraihan (8/4/2014)
 * 
 * @param msg 
 * @param now 
 * @param out 
 * 
 * @return bool 
 */
bool AbstractParser::echo(const ofp_message & msg, const time_t now, std::ostream& out)
{
    bool retVal = false;

    if ( send_echo_reply(const_cast<ofp_message&>(msg)) )
        retVal = mVconn.mpStateMgr->TransitionTo(OfConnStateManager::S_ACTIVE, now);

    out <<"\niOpenFlow::echo - send_echo_reply: " << (retVal?"success":"failed") << "\n";
    return retVal;
}

/**
 * outgoing msg.type == OFPT_ECHO_REPLY 
 * @author sraihan (8/4/2014)
 * 
 * @param msg 
 * 
 * @return bool 
 */
bool AbstractParser::send_echo_reply(ofp_message & msg)
{
    mbSendEchoFlag = false; // reset echo send flag as remote is active
    msg.type = get_echo_reply_type();
    msg.version = get_wire_version();
    return mVconn.Send(msg);
}

/**
 * @author sraihan (8/4/2014) 
 * 
 * @return uint64_t 
 */
uint64_t AbstractParser::make_dpid()
{
    union {
        uint8_t  dpid[8];
        uint64_t dpid_;
    } dp_id;
    dp_id.dpid_ = 0;
    mVconn.GetMacAddress(reinterpret_cast<char*>(dp_id.dpid));
    dp_id.dpid[6] = 0;
    // printf("----------skl LS id=%d\n", dp_id.dpid[6] );
    dp_id.dpid_ = (uint64_t) ((int64_t)dp_id.dpid[6] << 48) |
                  ((uint64_t) dp_id.dpid[ 0 ] << 40) |
                  ((uint64_t) dp_id.dpid[ 1 ] << 32) |
                  ((uint64_t) dp_id.dpid[ 2 ] << 24) |
                  ((uint64_t) dp_id.dpid[ 3 ] << 16) |
                  ((uint64_t) dp_id.dpid[ 4 ] << 8) ; // to use chassis base MAC
    return dp_id.dpid_;
}

/**
 * @see OFPT_ERROR
 * 
 * @author sraihan (8/4/2014)
 * 
 * @param msg 
 * @param type 
 * @param code 
 * @param out 
 * 
 * @return bool 
 */
bool AbstractParser::report_bad_message(const ofp_message& msg,
                                        const uint16_t type,
                                        const uint16_t code,
                                        std::ostream& out)
{
    bool retVal = false;
    const uint8_t*const bad_msg = (uint8_t*)&msg;
    const size_t data_len = 64;
    const size_t data_to_copy_len = ((msg.get_length() < data_len) ? msg.get_length() : data_len);
    const size_t msg_len = sizeof(ofp_error_msg)+data_to_copy_len;
    ofp_message* error_pkt = reinterpret_cast<ofp_message*>(new uint8_t[msg_len]);
    if ( error_pkt ) {
        error_pkt->set_length(msg_len);
        error_pkt->type = get_error_type();
        error_pkt->version = get_wire_version();
        error_pkt->xid = msg.xid;
        static_cast<ofp_error_msg*>(error_pkt)->set_type(type);
        static_cast<ofp_error_msg*>(error_pkt)->set_code(code);

        if ( data_to_copy_len )
            memcpy((char*)static_cast<ofp_error_msg*>(error_pkt)->data,
                   bad_msg,
                   data_to_copy_len);
        retVal = mVconn.Send(*error_pkt);
        std::cout << *static_cast<ofp_error_msg*>(error_pkt) << "\n";
        //      assert(false);
        delete[] (uint8_t*)(error_pkt);
    }
    if ( gDbgMask & DBG_ERR_OF )
        out << " bad_message, type= " <<  type << ", code= " << code << ", retVal= " << retVal <<"\n";
    return retVal;
}

/**
 *  
 * @author sraihan (8/4/2014)
 * 
 * @param ofp_struct_type 
 * @param msg 
 * @param out 
 * 
 * @return bool 
 */
template<typename ofp_struct_type>
bool AbstractParser::modify_msg_validate(const ofp_message& msg, std::ostream& out) 
{
    if (msg.get_length() < ((const ofp_struct_type&)msg).get_minimum_length()) {
        if (gDbgMask & DBG_ERR_OF) {
            out << "\nBad Len, expected minimum: "
                << ((const ofp_struct_type&)msg).get_minimum_length()
                << ", Got: " << msg.get_length() << "\n";
        }
        report_bad_message(msg, OpenflowV4::OFPET_BAD_REQUEST, OpenflowV4::OFPBRC_BAD_LEN, out);
        return false; // stop processing further in this batch
    } else if (mVconn.IsPassive()) {
        if (gDbgMask & DBG_ERR_OF) {
            out << "Err: Passive Controller can't Modify \n ";
        }
        report_bad_message(msg, OpenflowV4::OFPET_BAD_REQUEST, OpenflowV4::OFPBRC_IS_SLAVE, out);
        return false;
    }
    return true;
}

template
bool AbstractParser::modify_msg_validate<OpenflowV4::ofp_packet_out>(const ofp_message& msg, std::ostream& out) ;

template
bool AbstractParser::modify_msg_validate<OpenflowV4::ofp_flow_mod>(const ofp_message& msg, std::ostream& out) ;

template
bool AbstractParser::modify_msg_validate<OpenflowV4::ofp_group_mod>(const ofp_message& msg, std::ostream& out) ;

template
bool AbstractParser::modify_msg_validate<OpenflowV4::ofp_port_mod>(const ofp_message& msg, std::ostream& out) ;

template
bool AbstractParser::modify_msg_validate<OpenflowV4::ofp_switch_config>(const ofp_message& msg, std::ostream& out) ;

/**
 * @author sraihan (8/4/2014) 
 * 
 * @param ofp_struct_type 
 * @param msg 
 * @param out 
 * 
 * @return bool 
 */
template<typename ofp_struct_type>
bool AbstractParser::read_msg_validate(const ofp_message& msg, std::ostream& out) 
{
    if (msg.get_length() < ((const ofp_struct_type&)msg).get_minimum_length()) {
        if (gDbgMask & DBG_ERR_OF) {
            out << "\nBad Len, expected minimum: "
                << ((const ofp_struct_type&)msg).get_minimum_length()
                << ", Got: " << msg.get_length() << "\n";
        }
        report_bad_message(msg, OpenflowV4::OFPET_BAD_REQUEST, OpenflowV4::OFPBRC_BAD_LEN, out);
        return false; // stop processing further in this batch
    } 
    return true;
}

template
bool AbstractParser::read_msg_validate<ofp_get_config>(const ofp_message& msg, std::ostream& out) ;

template
bool AbstractParser::read_msg_validate<OpenflowV4::ofp_queue_get_config_request>(const ofp_message& msg, std::ostream& out) ;


