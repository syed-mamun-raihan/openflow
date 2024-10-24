/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include <boost/range/adaptor/map.hpp>

#include "TcpClient.hpp"
#include "SslClient.hpp"
#include "OfpHeader.h"
#include "AbstractConn.hpp"
#include "OfConnManager.hpp"
#include "OfpHeader.h"
#include "OfConnStateManager.hpp"
#include "OfConnStateObjects.hpp"
#include "OFV4Parser.hpp"
#include "OfDpDefine.hpp"
#include "OfDpDefinePD.h"
#include "OfDpLogicalSw.h"

uint32_t  gBarrierCount=0;
uint32_t gFlowCount=0;

using namespace OpenflowV4; // 1.3 Spec

/**
 * CTOR 
 * @author sraihan 
 * 
 * @param rVconn 
 * @param ver 
 */
OFV4Parser::OFV4Parser(AbstractConn& rVconn, const uint8_t ver):AbstractParser(rVconn, ver)
{
}
/**
 * DTOR
 * 
 * @author sraihan 
 */
OFV4Parser::~OFV4Parser()
{
}

/**
 *  It fans out to specific message handler
 * 
 * @author sraihan 
 * 
 * @param msg 
 * @param now 
 * @param out 
 * 
 * @return bool 
 */
bool OFV4Parser::process_of_msg(const ofp_message& msg, const time_t now, std::ostream& out) {
    switch ( msg.type ) {
    case OFPT_ECHO_REQUEST:  return echo(msg, now, out);
    case OFPT_ECHO_REPLY:  return echo_reply(msg, now, out);
    case OFPT_FEATURES_REQUEST:  return GetFeature(msg, out);
    case OFPT_GET_CONFIG_REQUEST:  return GetConfig(msg, out);
    case OFPT_SET_CONFIG:  return SetConfig(msg, out);
    case OFPT_PACKET_OUT:  return PacketOut(msg, out);
    case OFPT_FLOW_MOD:  return FlowMod(msg, out);
    case OFPT_GROUP_MOD:  return GroupMod(msg, out);
    case OFPT_PORT_MOD:  return PortMod(msg, out);
    case OFPT_TABLE_MOD:  return TableMod(msg, out);
    case OFPT_STATS_REQUEST:  return StatsReq(msg, out);
    case OFPT_BARRIER_REQUEST:  return BarrierReq(msg, out);
    case OFPT_QUEUE_GET_CONFIG_REQUEST:  return QueueConfReq(msg, out);
    case OFPT_ROLE_REQUEST:  return RoleReq(msg, out);
    case OFPT_GET_ASYNC_REQUEST:
        break;
    case OFPT_GET_ASYNC_REPLY:
        break;
    case OFPT_SET_ASYNC:
        break;
        /* Meters and rate limiters configuration messages. */
    case OFPT_METER_MOD:
        break;
    default:
        break;
    };
    return report_bad_message(msg, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE, out); // bad type
}

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
bool OFV4Parser::GetConfig(const ofp_message & msg, std::ostream& out) {
    out << "\nOFV4Parser::GetConfig " ;
    if (!read_msg_validate<ofp_get_config>(msg,out)) {
        return false;
    }
    OfDpSwitchConfigT ret_msg;
    PREPARE_OFDP_RESPONSE(msg)
    if ( mVconn.mConnMgr.mLogicalSw.GetConfig(ret_msg, response) ) {
        return GetConfigReply(ret_msg, response);
    }
    return true;
}

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

bool OFV4Parser::PacketOut(const ofp_message & msg, std::ostream& out) {
    out << "\n OFV4Parser::PacketOut " ;
    if (!modify_msg_validate<ofp_packet_out>(msg,out)) {
        return false;
    }
    OfDpPacketOutT packet_out; 
    
    const ofp_packet_out& pkt_out_msg = ((const ofp_packet_out&)msg);
    packet_out.buffer_id = pkt_out_msg.get_buffer_id();
    packet_out.in_port = pkt_out_msg.get_in_port();
    // parse actions
    const ofp_tlv_base* action_head = &pkt_out_msg.actions[0];
    size_t current_len = pkt_out_msg.get_minimum_length();
    const size_t packet_with_actions_len = pkt_out_msg.get_minimum_length() + pkt_out_msg.get_actions_len();
    while ( current_len < packet_with_actions_len ) {
        if ( TEST_GOOD_ENCODING(&msg, msg.get_length(), action_head) ) {
            current_len += action_head->get_len();
            if ( current_len <= packet_with_actions_len ) {
                uint16_t code = OFPBAC_BAD_TYPE;
                if ( packet_out.num_actions == sizeof(packet_out.actions)/sizeof(packet_out.actions[0]) ) {
                    return report_bad_message(msg, OFPET_BAD_ACTION, OFPBAC_TOO_MANY, out);
                } else if ( !ParseActionItem(action_head, packet_out.actions.data(), packet_out.num_actions, out, code) ) {
                    return report_bad_message(msg, OFPET_BAD_ACTION, code, out);
                }
            }
        } else {
            out << " Actions: done\n";
            break;
        }
    }// end while
    out << "\n actions processed " << packet_out.num_actions << "\n";

    // copy data if any
    const uint8_t*const data = (const uint8_t*const)(&msg) + packet_with_actions_len;

    packet_out.message_len = pkt_out_msg.get_length() - packet_with_actions_len;

    memcpy(packet_out.message, data, packet_out.message_len);
    PREPARE_OFDP_RESPONSE(msg)

    if ( mVconn.mConnMgr.mLogicalSw.PacketOut(packet_out, response) ) {
        return PacketOutReply(response);
    }
    return true;
}

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
bool OFV4Parser::FlowMod(const ofp_message & msg, std::ostream& out) {
    const char* func_name = "\n OFV4Parser::FlowMod ";
    out << func_name;
    if (!modify_msg_validate<ofp_flow_mod>(msg,out)) {
        return false;
    }

    if ( gDsDbPerfDebugFlag ) {
        if ( flowstats.getCnt() == 0 ) {
            gettimeofday(&perf_starttime, 0);
        }
    }

    const ofp_flow_mod& ofm=(const ofp_flow_mod&)msg;
    OfDpFlowT saosFlow;
    const uint16_t command = ofm.get_command();
    if ( command == OFPFC_ADD ) {
        saosFlow.command = OFDP_FLOW_ADD;
    } else if ( command == OFPFC_MODIFY ) {
        saosFlow.command = OFDP_FLOW_MODIFY;
    } else if ( command == OFPFC_MODIFY_STRICT ) {
        saosFlow.command = OFDP_FLOW_MODIFY_STRICT;
    } else if ( command == OFPFC_DELETE ) {
        saosFlow.command = OFDP_FLOW_DELETE;
    } else if ( command == OFPFC_DELETE_STRICT ) {
        saosFlow.command = OFDP_FLOW_DELETE_STRICT;
    } else
        return report_bad_message(msg, OFPET_FLOW_MOD_FAILED, OFPFMFC_BAD_COMMAND, out);

    saosFlow.cookie = ofm.get_cookie();
    saosFlow.cookie_mask = ofm.get_cookie_mask();
    saosFlow.hard_timeout = ofm.get_hard_timeout();
    saosFlow.idle_timeout = ofm.get_idle_timeout();
    saosFlow.out_port = ofm.get_out_port();
    saosFlow.out_group = ofm.get_out_group();
    saosFlow.priority = ofm.get_priority();
    saosFlow.flags = (OfDpFlowModFlags) ofm.get_flags();
    saosFlow.num_actions = 0;
    if ( msg.get_length() >= msg.get_minimum_length() ) {
        const oxm_fields_t* oxm_head = ofm.match.oxm_fields;
        const size_t match_len = ofm.match.get_len(); // TLV - length
        out << " match length " << match_len << "\n";
        size_t cur_len = sizeof(ofp_tlv_base);

        if ( ofm.match.get_type() != OFPMT_OXM ) {
            out << " match type is not OFPMT_OXM \n";
            return report_bad_message(msg, OFPET_BAD_MATCH, OFPBMC_BAD_TYPE, out);
        }
        while ( cur_len < match_len ) {
            uint16_t code = OFPBMC_BAD_LEN;
            if ( TEST_GOOD_ENCODING(&ofm.match, ofm.match.get_len(), oxm_head) ) {
                cur_len += oxm_head->getLength()+sizeof(oxm_fields_t); // OXM_FLIELD LEN
                if ( cur_len <= match_len ) {
                    code = OFPBMC_BAD_TYPE;
                    if ( !ParseOXMField(oxm_head, saosFlow.match , out, code) ) {
                        return report_bad_message(msg, OFPET_BAD_MATCH, code, out);
                    }
                }
            } else {
                out << " OXM_MATCH: done\n";
                break;
            }
        }// check next field

        const ofp_tlv_base* instruction_head = ofm.match.get_next_element();// it takes care of padding
        //---
        const uint16_t max_actions = sizeof (saosFlow.actions) / sizeof (saosFlow.actions[0]);
        cur_len = (ofm.get_minimum_length() - sizeof(ofp_match)) + ::ofp_tlv_base::get_size_64bit_aligned(match_len);
        while ( cur_len < msg.get_length() ) {
            uint16_t err_type = OFPET_BAD_INSTRUCTION;
            uint16_t err_code = OFPBIC_BAD_LEN;
            if ( TEST_GOOD_ENCODING(&msg, msg.get_length(), instruction_head) ) {
                cur_len += instruction_head->get_len();
                if ( cur_len <= msg.get_length() ) {
                    if ( !ParseInstructionItem(instruction_head,
                                               saosFlow.actions.data(),
                                               saosFlow.num_actions,
                                               max_actions,
                                               out,
                                               err_type,
                                               err_code) ) {
                        out << " bad OFPET_BAD_INSTRUCTION: type : " << err_type << " code : " << err_code << "\n";
                        return report_bad_message(msg, err_type, err_code, out);
                    }
                }
            } else {
                out << " Instructions: done\n";
                break;
            }
        }// end while
        out << "\n actions processed " << saosFlow.num_actions << "\n";
    } else {
        out << "- no oxm_match or instructions found -\n";
    }
	/// Search output action
	for(const auto& i : saosFlow.actions){
		if(i.command == OFDP_NONE_COMMAND) break;
		else if(i.command == OFDP_OUTPUT)
		{
			if(i.value.output.port) saosFlow.out_port = i.value.output.port;
			break;
		}
	}
    PREPARE_OFDP_RESPONSE(msg)
    if ( mVconn.mConnMgr.mLogicalSw.ResourceMod(saosFlow, response) ) {
        return ResourceModReply(saosFlow, response);
    }
    return true;
}

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
bool OFV4Parser::GroupMod(const ofp_message & msg, std::ostream& out) {
    const char* func_name = "\n OFV4Parser::GroupMod ";
    out << func_name;
    if (!modify_msg_validate<ofp_group_mod>(msg,out)) {
        return false;
    }

    const ofp_group_mod& omg=(const ofp_group_mod&)msg;
    OfDpGroupT saosGroup;
    saosGroup.group_id = omg.get_group_id();
    if ( omg.get_command() == OFPGC_ADD )
        saosGroup.command = OFDP_CONFIG_CREATE;
    else if ( omg.get_command() == OFPGC_MODIFY )
        saosGroup.command = OFDP_CONFIG_MODIFY;
    else if ( omg.get_command() == OFPGC_DELETE )
        saosGroup.command = OFDP_CONFIG_REMOVE;
    else
        return report_bad_message(msg, OFPET_GROUP_MOD_FAILED, OFPGMFC_BAD_COMMAND, out);

    if ( omg.group_type == OFPGT_INDIRECT )
        saosGroup.type = OFDP_GROUP_INDIRECT;
    else if ( omg.group_type == OFPGT_SELECT )
        saosGroup.type = OFDP_GROUP_SELECT;
    else if ( omg.group_type == OFPGT_FF )
        saosGroup.type = OFDP_GROUP_FF;
    else if ( omg.group_type == OFPGT_ALL )
        saosGroup.type = OFDP_GROUP_ALL;
    else
        return report_bad_message(msg, OFPET_GROUP_MOD_FAILED, OFPGMFC_INVALID_GROUP, out);

    const ofp_bucket* cur_bucket = omg.buckets;
    const uint16_t max_buckets = sizeof(saosGroup.buckets)/sizeof(saosGroup.buckets[0]);
    const uint8_t* tail_ptr = reinterpret_cast<const uint8_t*>(&msg)+msg.get_length();
    while ( cur_bucket < reinterpret_cast<const ofp_bucket*>(tail_ptr) ) {
        uint16_t err_type = OFPET_GROUP_MOD_FAILED;
        uint16_t err_code = OFPGMFC_BAD_BUCKET;
        if ( !ParseBuckets(cur_bucket,
                           &saosGroup.buckets[saosGroup.num_buckets] ,
                           saosGroup.num_buckets,
                           max_buckets,
                           out,
                           err_type,
                           err_code) )
            return report_bad_message(msg, err_type, err_code, out);
    }// check next field
    out << "\n buckets processed " << saosGroup.num_buckets << "\n";

    PREPARE_OFDP_RESPONSE(msg)
    if ( mVconn.mConnMgr.mLogicalSw.ResourceMod(saosGroup, response) ) {
        return ResourceModReply(saosGroup, response);
    }
    return true;
}

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
bool OFV4Parser::PortMod(const ofp_message & msg, std::ostream& out) 
{
    if (!modify_msg_validate<ofp_port_mod>(msg,out)) {
        return false;
    }
    OfDpPortInfoT port;
    port.port_no = static_cast<const ofp_port_mod&>(msg).get_port_no();
    port.advertised = static_cast<const ofp_port_mod&>(msg).get_advertise();
    port.config = static_cast<const ofp_port_mod&>(msg).get_config();
    const unsigned len = std::min(sizeof(port.hw_addr),sizeof(static_cast<const ofp_port_mod&>(msg).hw_addr));
    memcpy(port.hw_addr, static_cast<const ofp_port_mod&>(msg).hw_addr, len);
    port.mask = static_cast<const ofp_port_mod&>(msg).get_mask();

    PREPARE_OFDP_RESPONSE(msg)
    if ( mVconn.mConnMgr.mLogicalSw.ResourceMod(port, response) ) {
        return ResourceModReply(port, response);
    }
    return true;
}

//\param    xid
//\desc     outgoing async msg
bool OFV4Parser::SendBarrierReply(const uint32_t xid) {
    const ofp_message msg = { get_wire_version(), OFPT_BARRIER_REPLY, (uint16_t)htons(sizeof(ofp_message)), xid};
#ifdef STANDALONE
    // Count this message reply too
    std::cout << "\n Barrier Reply, xid: " << msg.get_xid()
    << " Net Incoming messages#" << mVconn.GetPacketsReceived()+1
    << " Net Outgoing messages#" << mVconn.GetPacketsSent() <<"\n";
#endif
    return mVconn.Send(msg);
}

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
bool OFV4Parser::QueueConfReq(const ofp_message & msg, std::ostream& out) {
    const char* func_name = "\nOFV4Parser::QueueConfReq ";
    out << func_name;
    if (!read_msg_validate<ofp_queue_get_config_request>(msg,out)) {
        return false;
    }
    const ofp_queue_get_config_request& req = static_cast<const ofp_queue_get_config_request&>(msg);
    OfDpQueueConfigT* reply;
    out << ":port: " << req.get_port();
    PREPARE_OFDP_RESPONSE(msg)
    if ( mVconn.mConnMgr.mLogicalSw.GetConfig(req.get_port(), reply, response) ) {
        GetConfigReply(*reply, response);
    }
    delete[] reinterpret_cast<uint8_t*>(reply);
    return true;
}

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
bool OFV4Parser::RoleReq(const ofp_message & msg, std::ostream& out) {
    const ofp_role_request& req = static_cast<const ofp_role_request&>(msg);
    if ( msg.get_length() == req.get_minimum_length() ) {
        out << " Role:  " << req.get_role() << " Generation_Id: " << req.get_generation_id() << std::endl;
        OfDpControllerChannelType nextRole(OFDP_CHANNEL_ROLE_EQUAL);
        // Check if next role is known
        switch ( req.get_role() ) {
        case OFPCR_ROLE_EQUAL: nextRole = OFDP_CHANNEL_ROLE_EQUAL; break;
        case OFPCR_ROLE_MASTER: nextRole = OFDP_CHANNEL_ROLE_MASTER; break;
        case OFPCR_ROLE_SLAVE: nextRole = OFDP_CHANNEL_ROLE_SLAVE; break;
        case OFPCR_ROLE_NOCHANGE:
            {
                uint32_t value = 0;
                if ( mVconn.GetCurrentRole() == OFDP_CHANNEL_ROLE_EQUAL ) {
                    value = OFPCR_ROLE_EQUAL;
                } else if ( mVconn.GetCurrentRole() == OFDP_CHANNEL_ROLE_MASTER ) {
                    value = OFPCR_ROLE_MASTER;
                } else if ( mVconn.GetCurrentRole() == OFDP_CHANNEL_ROLE_SLAVE ) {
                    value = OFPCR_ROLE_SLAVE;
                }
                const_cast<ofp_role_request&>(req).set_role(value);
                const_cast<ofp_role_request&>(req).type = OFPT_ROLE_REPLY;
                mVconn.Send(msg);
            }
            return true;
        default:
            // Unknown Role
            return report_bad_message(msg,
                                      OFPET_ROLE_REQUEST_FAILED,
                                      OFPRRFC_BAD_ROLE, out);
        };
        // Check if next role is same as current
        if ( mVconn.GetCurrentRole() == nextRole ) {
            const_cast<ofp_role_request&>(req).type = OFPT_ROLE_REPLY;
            mVconn.Send(msg);
            return true;
        }
        // check for validGenerationId & ensure role reversal otherwise
        if ( nextRole != OFDP_CHANNEL_ROLE_EQUAL &&
             (int64_t)(req.get_generation_id() - mVconn.mConnMgr.mMaxGenerationId) < 0 ) {
            return report_bad_message(msg,
                                      OFPET_ROLE_REQUEST_FAILED,
                                      OFPRRFC_STALE, out);
        } else if ( nextRole != OFDP_CHANNEL_ROLE_EQUAL ) {
            mVconn.mConnMgr.mMaxGenerationId = req.get_generation_id();
        }
        if ( nextRole == OFDP_CHANNEL_ROLE_MASTER ) {
            mVconn.mConnMgr.ResetMastersToSlave();
        }
        // update channel role and send a reply back
        mVconn.GetCurrentRole() = nextRole;
        const_cast<ofp_role_request&>(req).type = OFPT_ROLE_REPLY;
        mVconn.Send(msg);
        return true;
    }
    report_bad_message(msg, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN, out);
    return false;
}

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
bool OFV4Parser::SetConfig(const ofp_message & msg, std::ostream& out) {
    out << "\n OFV4Parser::SetConfig " ;
    if (!modify_msg_validate<OpenflowV4::ofp_switch_config>(msg,out)) {
        return false;
    }
    OfDpSwitchConfigT config;
    out << ((ofp_switch_config&)msg);

    config.flags = ((ofp_switch_config&)msg).get_flags();
    config.miss_send_len = ((ofp_switch_config&)msg).get_miss_send_len();
    PREPARE_OFDP_RESPONSE(msg)
    if ( mVconn.mConnMgr.mLogicalSw.SetConfig(config, response) ) {
        SetConfigReply(response);
    }
    return true;
}

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
bool OFV4Parser::GetDescStat(const ofp_message& param, std::ostream& out)
{
    OfDpDescStatsT ret_msg;
    PREPARE_OFDP_RESPONSE(param)
    if ( mVconn.mConnMgr.mLogicalSw.GetStat(ret_msg, response) ) {
        return GetStatReply(ret_msg, response);
    }
    return true;
}

/**
 * OfDpPortStatusT 
 * @author sraihan 
 * 
 * @param info 
 * 
 * @return bool 
 */
bool OFV4Parser::SendPortStatus(const OfDpPortStatusT& info)
{
    if ( !mVconn.IsOpen() ) return false;
    ofp_message lpcWriteBuffer[_OF_JUMBO_PACKET_SIZE/sizeof(ofp_message)];
    ofp_message& ret_msg = lpcWriteBuffer[0];
    ofp_port_status& port_status = static_cast<ofp_port_status&>(ret_msg);
    port_status.set_length(sizeof(port_status));
    port_status.version = get_wire_version();
    port_status.type = OFPT_PORT_STATUS;
    port_status.xid = 0;
    switch ( info.reason ) {
    case OFDP_OFPPR_ADD:
        port_status.reason = OFPPR_ADD;
        break;
    case OFDP_OFPPR_DELETE:
        port_status.reason = OFPPR_DELETE;
        break;
    case OFDP_OFPPR_MODIFY:
        port_status.reason = OFPPR_MODIFY;
        break;
    default:
        return false;
        break;
    };
    port_status.desc.set_advertised(info.desc.advertised);
    port_status.desc.set_config(info.desc.config);
    port_status.desc.set_curr(info.desc.curr);
    port_status.desc.set_peer(info.desc.peer);
    port_status.desc.set_port_no(info.desc.port_no);
    port_status.desc.set_state(info.desc.state);
    port_status.desc.set_supported(info.desc.supported);
    memcpy(port_status.desc.hw_addr, info.desc.hw_addr, sizeof(port_status.desc.hw_addr));
    memcpy(port_status.desc.name, info.desc.name.c_str(), sizeof(port_status.desc.name));
    port_status.desc.set_curr_speed(info.desc.curr_speed);
    port_status.desc.set_max_speed(info.desc.max_speed);
    return mVconn.Send((const ofp_message&)(port_status));
}

/**
 * OfDpFlowRemovedT
 * 
 * @author sraihan 
 * 
 * @param info 
 * 
 * @return bool 
 */
bool OFV4Parser::SendFlowRemoved(const OfDpFlowRemovedT& info)
{
    if ( !mVconn.IsOpen() || mVconn.IsPassive() ) return false;
    ofp_message lpcWriteBuffer[_OF_JUMBO_PACKET_SIZE/sizeof(ofp_message)];
    ofp_message& msg = lpcWriteBuffer[0];
    msg.set_length(sizeof(ofp_flow_removed));
    msg.version = get_wire_version();
    msg.type = OFPT_FLOW_REMOVED;
    msg.xid = 0;
    // ofp_flow_removed header
    static_cast<ofp_flow_removed&>(msg).reason = info.reason;
    static_cast<ofp_flow_removed&>(msg).set_byte_count(info.byte_count);
    static_cast<ofp_flow_removed&>(msg).set_cookie(info.cookie);
    static_cast<ofp_flow_removed&>(msg).set_priority(info.priority);
    static_cast<ofp_flow_removed&>(msg).table_id = info.table_id;
    static_cast<ofp_flow_removed&>(msg).set_duration_nsec(info.duration_nsec);
    static_cast<ofp_flow_removed&>(msg).set_duration_sec(info.duration_sec);
    static_cast<ofp_flow_removed&>(msg).set_idle_timeout(info.idle_timeout);
    static_cast<ofp_flow_removed&>(msg).set_packet_count(info.packet_count);
    static_cast<ofp_flow_removed&>(msg).set_priority(info.priority);

    if ( EncodeOXMFields<ofp_flow_removed>(msg,info.match) ) {
        static_cast<ofp_flow_removed&>(msg).match.set_type(OFPMT_OXM);
        if ( (gDbgMask == (DBG_DETAIL|DBG_PARSER_PARSE)) ) {
            mVconn.DumpPacket(reinterpret_cast<uint8_t*>(&msg), msg.get_length(), "FlowRemoved", std::cout);
        }
        return mVconn.Send(msg);
    }
    return false;
}

/**
 * @see ofp_flow_stats
 * @see ofp_flow_stats_request
 * @author sraihan 
 * 
 * @param param 
 * @param out 
 * 
 * @return bool 
 */
bool OFV4Parser::GetFlowStat(const ofp_message& param, std::ostream& out)
{
    const ofp_flow_stats_request& msg = (const ofp_flow_stats_request&)(param);
    OfDpFlowStatsReqT request;
    request.table_id = msg.table_id;
    request.cookie = msg.get_cookie();
    request.cookie_mask = msg.get_cookie_mask();
    request.out_port = msg.get_out_port(); // Special ports are 32 bit
    request.out_group = msg.get_out_group(); // Special groups are 32 bit
    out << "\n OFV4Parser::GetFlowStat ";
    const oxm_fields_t* cur_ptr = msg.match.oxm_fields;
    const size_t match_len = msg.match.get_len(); // TLV - length
    out << " match length " << match_len << "\n";
    size_t cur_len = sizeof(ofp_tlv_base);
    uint16_t code = OFPBMC_BAD_LEN;

    if ( msg.match.get_type() != OFPMT_OXM ) {
        code = OFPBMC_BAD_TYPE;
        out << " match type is not OFPMT_OXM \n";
        return report_bad_message(param, OFPET_BAD_MATCH, code, out);
    }

    while ( cur_len < match_len ) {
        if ( TEST_GOOD_ENCODING(&msg.match, msg.match.get_len(), cur_ptr) ) {
            cur_len += cur_ptr->getLength()+sizeof(oxm_fields_t); // OXM_FLIELD LEN
            if ( cur_len <= match_len ) {
                code = OFPBMC_BAD_TYPE;
                if ( !ParseOXMField(cur_ptr, request.match , out, code) )
                    return report_bad_message(param, OFPET_BAD_MATCH, code, out);
            }
        } else {
            out << "OXM_MATCH: done\n";
            break;
        }
    }// check next field
    PREPARE_OFDP_RESPONSE(param)
    mVconn.mConnMgr.mLogicalSw.GetStat(request, response);
    return true;
}

/**
 * @see ofp_aggregate_stats
 * @see ofp_aggregate_stats_request
 * @author sraihan 
 * 
 * @param param 
 * @param out 
 * 
 * @return bool 
 */
bool OFV4Parser::GetAggregateStat(const ofp_message& param, std::ostream& out)
{
    const ofp_aggregate_stats_request& msg = (const ofp_aggregate_stats_request&)(param);
    OfDpAggregateStatsReqT request;

    request.table_id = msg.table_id;
    request.cookie = msg.get_cookie();
    request.cookie_mask = msg.get_cookie_mask();
    request.out_port = msg.get_out_port();
    request.out_group = msg.get_out_group();
    out << "\n OFV4Parser::GetAggregateStat ";

    if ( msg.match.get_type() != OFPMT_OXM ) {
        out << " match type is not OFPMT_OXM \n";
        return report_bad_message(param, OFPET_BAD_MATCH, OFPBMC_BAD_TYPE, out);
    }
    const oxm_fields_t* cur_ptr = msg.match.oxm_fields;
    const size_t match_len = msg.match.get_len(); // TLV - length
    out << " match length " << match_len << "\n";
    size_t cur_len = sizeof(ofp_tlv_base);
    while ( cur_len < match_len ) {
        uint16_t code = OFPBMC_BAD_LEN;
        if ( TEST_GOOD_ENCODING(&msg.match, msg.match.get_len(), cur_ptr) ) {
            cur_len += cur_ptr->getLength()+sizeof(oxm_fields_t); // OXM_FLIELD LEN
            if ( cur_len <= match_len ) {
                code = OFPBMC_BAD_TYPE;
                if ( !ParseOXMField(cur_ptr, request.match , out, code) )
                    return report_bad_message(param, OFPET_BAD_MATCH, code, out);
            }
        } else {
            out << " OXM_MATCH: done.\n";
            break;
        }
    }// check next field
    OfDpAggregateStatsT reply;

    PREPARE_OFDP_RESPONSE(param)
    if ( mVconn.mConnMgr.mLogicalSw.GetStat(request, reply, response) ) {
        return GetStatReply(reply, response);
    }
    return true;
}

/**
 * @see ofp_table_stats
 * 
 * @author sraihan 
 * 
 * @param param 
 * @param out 
 * 
 * @return bool 
 */
bool OFV4Parser::GetTableStat(const ofp_message& param, std::ostream& out)
{
    OfDpTableStatsT reply;
    PREPARE_OFDP_RESPONSE(param)
    if ( mVconn.mConnMgr.mLogicalSw.GetStat(reply, response) ) {
        return GetStatReply(reply, response);
    }
    return true;
}

/**
 * @see ofp_port_stats_request
 * @see ofp_port_stats
 * @author sraihan 
 * 
 * @param param 
 * @param out 
 * 
 * @return bool 
 */
bool OFV4Parser::GetPortStat(const ofp_message& param, std::ostream& out)
{
    const ofp_port_stats_request &statsReq = (const ofp_port_stats_request&)(param);
    const OfDpPortStatsReqT request(statsReq.get_port_no()) ;
    OfDpPortStatsT reply;

    PREPARE_OFDP_RESPONSE(param)
    if ( mVconn.mConnMgr.mLogicalSw.GetStat(request, reply, response) ) {
        return GetStatReply(reply, response);
    }
    return true;
}

/**
 * @see ofp_queue_stats
 * @see OfDpQueueStatsT 
 * @see ofp_queue_stats_request 
 * @author sraihan 
 * 
 * @param param 
 * @param out 
 * 
 * @return bool 
 */
bool OFV4Parser::GetQueueStat(const ofp_message& param, std::ostream& out)
{
    const ofp_queue_stats_request &msg = (const ofp_queue_stats_request&)(param);
    OfDpQueueStatsT stats;
    stats.queue_id = msg.get_queue_id();
    stats.flags = 0;
    stats.port_no = msg.get_port_no();
    stats.tx_bytes = std::numeric_limits<typeof(stats.tx_bytes)>::max();
    stats.tx_packets = std::numeric_limits<typeof(stats.tx_packets)>::max();
    stats.tx_errors = std::numeric_limits<typeof(stats.tx_errors)>::max();

    PREPARE_OFDP_RESPONSE(param)
    if ( mVconn.mConnMgr.mLogicalSw.GetStat(stats, response) ) {
        return GetStatReply(stats, response);
    }
    return true;
}

/**
 * @see ofp_group_stats
 * @see ofp_group_stats_request 
 * @see OfDpGroupStatsT
 * @author sraihan 
 * 
 * @param param 
 * @param out 
 * 
 * @return bool 
 */
bool OFV4Parser::GetGroupStat(const ofp_message& param, std::ostream& out)
{
    const ofp_group_stats_request &msg = (const ofp_group_stats_request&)(param);
    OfDpGroupStatsT stats;
    stats.group_id = msg.get_group_id();

    PREPARE_OFDP_RESPONSE(param)
    if ( mVconn.mConnMgr.mLogicalSw.GetStat(stats, response) ) {
        return GetStatReply(stats, response);
    }
    return true;
}

/**
 * @see ofp_group_desc_stats 
 * @see OfDpGroupDescStatsT 
 * 
 * @author sraihan 
 * 
 * @param param 
 * @param out 
 * 
 * @return bool 
 */
bool OFV4Parser::GetGroupDescStat(const ofp_message& param, std::ostream& out)
{
    OfDpGroupDescStatsT stats;
    stats.group_id = -1;
    PREPARE_OFDP_RESPONSE(param)
    if ( mVconn.mConnMgr.mLogicalSw.GetStat(stats, response) ) {
        return GetStatReply(stats, response);
    }
    return true;
}

/**
 * @see ofp_group_features_stats
 * @see OfDpGroupFeatureStatsT
 * @author sraihan 
 * 
 * @param param 
 * @param out 
 * 
 * @return bool 
 */
bool OFV4Parser::GetGroupFeatureStat(const ofp_message& param, std::ostream& out)
{
    OfDpGroupFeatureStatsT stats;

    PREPARE_OFDP_RESPONSE(param)
    if ( mVconn.mConnMgr.mLogicalSw.GetStat(stats, response) ) {
        return GetStatReply(stats, response);
    }
    return true;
}

/**
 * @see OfDpSwitchConfigT
 * @see ofp_switch_config
 * @author sraihan 
 * 
 * @param info 
 * @param response 
 * 
 * @return bool 
 */
bool OFV4Parser::GetConfigReply(const OfDpSwitchConfigT& info, const OfDpResponseT& response)
{
    const ofp_message* temp = reinterpret_cast<const ofp_message*>(response.msg);
    if ( response.return_code == OFDP_OK ) {
        ofp_message lpcWriteBuffer[_OF_JUMBO_PACKET_SIZE/sizeof(ofp_message)];
        bzero(reinterpret_cast<char*>(lpcWriteBuffer), _OF_JUMBO_PACKET_SIZE);
        ofp_message& ret_msg = lpcWriteBuffer[0];
        ofp_switch_config& msg_to_send = static_cast<ofp_switch_config&>(ret_msg);
        msg_to_send.set_length(sizeof(msg_to_send));
        msg_to_send.type = OFPT_GET_CONFIG_REPLY;
        msg_to_send.xid = temp->xid;
        msg_to_send.version = get_wire_version();
        msg_to_send.set_flags(info.flags);
        msg_to_send.set_miss_send_len(info.miss_send_len);
        return mVconn.Send((const ofp_message&) msg_to_send);
    }
    uint16_t type=OFPET_BAD_REQUEST;
    uint16_t code=OFPBRC_BAD_TYPE;
    std::ostream null_out(0);
    if ( TranslateOfDpReturnToOFVal(response.return_code, type, code) ) {
        return report_bad_message(*temp, type, code, null_out);
    }
    return true;
}

/**
 * @see ofp_switch_config
 * 
 * @author sraihan 
 * 
 * @param response 
 * 
 * @return bool 
 */
bool OFV4Parser::SetConfigReply(const OfDpResponseT& response)
{
    const ofp_message* temp = reinterpret_cast<const ofp_message*>(response.msg);
    uint16_t type=OFPET_BAD_REQUEST;
    uint16_t code=OFPBRC_BAD_TYPE;
    std::ostream null_out(0);
    if ( TranslateOfDpReturnToOFVal(response.return_code, type, code) ) {
        return report_bad_message(*temp, type, code, null_out);
    }
    return true;
}

/**
 * 
 * 
 * @author sraihan 
 * 
 * @param response 
 * 
 * @return bool 
 */
bool OFV4Parser::PacketOutReply(const OfDpResponseT& response)
{
    const ofp_message* temp = reinterpret_cast<const ofp_message*>(response.msg);
    uint16_t type= (response.return_code == OFDP_BUFFER_UNKNOWN) ? OFPET_BAD_REQUEST : OFPET_BAD_ACTION;
    uint16_t code=OFPBRC_BAD_TYPE;
    std::ostream null_out(0);
    if ( TranslateOfDpReturnToOFVal(response.return_code, type, code) ) {
        return report_bad_message(*temp, type, code, null_out);
    }
    return true;
}
/**
 *  
 * @author sraihan 
 * 
 * @param info 
 * @param response 
 * 
 * @return bool 
 */
bool OFV4Parser::ResourceModReply(const OfDpFlowT & info, const OfDpResponseT& response)
{
    const ofp_message* temp = reinterpret_cast<const ofp_message*>(response.msg);
    uint16_t type=OFPET_FLOW_MOD_FAILED;
    uint16_t code=OFPFMFC_UNKNOWN;
    std::ostream null_out(0);
    if ( TranslateOfDpReturnToOFVal(response.return_code, type, code) ) {
        return report_bad_message(*temp, type, code, null_out);
    }
    return true;
}

/**
 *  
 * @author sraihan 
 * 
 * @param info 
 * @param response 
 * 
 * @return bool 
 */
bool OFV4Parser::ResourceModReply(const OfDpGroupT & info, const OfDpResponseT& response)
{
    const ofp_message* temp = reinterpret_cast<const ofp_message*>(response.msg);
    uint16_t type= OFPET_GROUP_MOD_FAILED;
    uint16_t code= OFPGMFC_UNKNOWN_GROUP;
    std::ostream null_out(0);
    if ( TranslateOfDpReturnToOFVal(response.return_code, type, code) ) {
        return report_bad_message(*temp, type, code, null_out);
    }
    return true;
}

/**
 *  
 * @author sraihan 
 * 
 * @param info 
 * @param response 
 * 
 * @return bool 
 */
bool OFV4Parser::ResourceModReply(const OfDpPortInfoT & info, const OfDpResponseT& response)
{
    const ofp_message* temp = reinterpret_cast<const ofp_message*>(response.msg);
    uint16_t type=OFPET_PORT_MOD_FAILED;
    uint16_t code=OFPBRC_BAD_TYPE;
    std::ostream null_out(0);
    if ( TranslateOfDpReturnToOFVal(response.return_code, type, code) ) {
        return report_bad_message(*temp, type, code, null_out);
    }
    return true;
}

/**
 *  
 * @author sraihan 
 * 
 * @param stats 
 * @param response 
 * 
 * @return bool 
 */
bool OFV4Parser::GetStatReply( const OfDpDescStatsT &stats, const OfDpResponseT& response)
{
    const ofp_message* temp = reinterpret_cast<const ofp_message*>(response.msg);
    if ( response.return_code == OFDP_OK ) {
        ofp_message lpcWriteBuffer[_OF_JUMBO_PACKET_SIZE/sizeof(ofp_message)];
        bzero(reinterpret_cast<char*>(lpcWriteBuffer), _OF_JUMBO_PACKET_SIZE);

        ofp_message& msg = lpcWriteBuffer[0];
        ofp_desc_stats& ret_msg = static_cast<ofp_desc_stats&>(msg);
        ret_msg.ofp_message::version = this->get_wire_version();
        ret_msg.ofp_message::type = OFPT_STATS_REPLY;
        ret_msg.ofp_message::xid = temp->xid;
        ret_msg.ofp_message::set_length(sizeof(ret_msg));
        ret_msg.ofp_stats_msg::set_stats_type(OFPST_DESC);
        ret_msg.ofp_stats_msg::set_flags(0);

        strncpy(ret_msg.dp_desc, stats.dp_desc, DESC_STR_LEN-1);
        strncpy(ret_msg.hw_desc, stats.hw_desc, DESC_STR_LEN-1);
        strncpy(ret_msg.mfr_desc, stats.mfr_desc, DESC_STR_LEN-1);
        strncpy(ret_msg.serial_num, stats.serial_num, SERIAL_NUM_LEN-1);
        strncpy(ret_msg.sw_desc, stats.sw_desc, DESC_STR_LEN-1);
        return mVconn.Send((const ofp_message&)(ret_msg));
    }
    uint16_t type=OFPET_BAD_REQUEST;
    uint16_t code=OFPBRC_BAD_STAT;
    std::ostream null_out(0);
    if ( TranslateOfDpReturnToOFVal(response.return_code, type, code) ) {
        return report_bad_message(*temp, type, code, null_out);
    }
    return true;
}

/**
 *  
 * @author sraihan 
 * 
 * @param stats 
 * @param response 
 * 
 * @return bool 
 */
bool OFV4Parser::GetStatReply( const OfDpFlowStatsT &stats, const OfDpResponseT& response)
{
    const ofp_message* temp = reinterpret_cast<const ofp_message*>(response.msg);
    if ( response.return_code == OFDP_OK ) {
        ofp_message lpcWriteBuffer[_OF_JUMBO_PACKET_SIZE/sizeof(ofp_message)];
        bzero(reinterpret_cast<char*>(lpcWriteBuffer), _OF_JUMBO_PACKET_SIZE);
        ofp_message& ret_msg = lpcWriteBuffer[0];
        if ( !(stats.flags & OFDP_STATS_MORE) ) {
            ofp_stats_msg& statsReply = static_cast<ofp_stats_msg&>(ret_msg);
            statsReply.version = get_wire_version();
            statsReply.type = OFPT_STATS_REPLY;
            statsReply.set_length(statsReply.get_minimum_length());
            statsReply.ofp_message::xid = temp->xid;
            statsReply.set_flags(stats.stats_flags);
            statsReply.set_stats_type(OFPST_FLOW);
            if ( (gDbgMask == (DBG_DETAIL|DBG_PARSER_PARSE)) ) {
                mVconn.DumpPacket(reinterpret_cast<uint8_t*>(&ret_msg), ret_msg.get_length(), "FlowStats", std::cout);
            }
            return mVconn.Send((const ofp_message&)(statsReply));
        } else {
            ret_msg.version = get_wire_version();
            ret_msg.type = OFPT_STATS_REPLY;
            ret_msg.xid = temp->xid;

            ofp_flow_stats &flowStats = static_cast<ofp_flow_stats &>(ret_msg);
            ret_msg.set_length(flowStats.get_minimum_length());
            flowStats.set_flags(stats.flags);
            flowStats.set_stats_type(OFPST_FLOW);
            flowStats.table_id = stats.table_id;
            flowStats.set_duration_sec(stats.duration_sec);
            flowStats.set_duration_nsec(stats.duration_nsec);
            flowStats.set_priority(stats.priority);
            flowStats.set_idle_timeout(stats.idle_timeout);
            flowStats.set_hard_timeout(stats.hard_timeout);
            flowStats.set_cookie(stats.cookie);
            flowStats.set_packet_count(stats.packet_count);
            flowStats.set_byte_count(stats.byte_count);

            if ( EncodeOXMFields<ofp_flow_stats>(ret_msg, stats.match) &&
                 EncodeActionItem(ret_msg, stats.actions.data(), stats.num_actions, true, OFPIT_APPLY_ACTIONS) ) {
                flowStats.ofp_flow_stats::set_stats_length(ret_msg.get_length()-sizeof(ofp_stats_msg));
                flowStats.match.set_type(OFPMT_OXM);
                if ( (gDbgMask == (DBG_DETAIL|DBG_PARSER_PARSE)) ) {
                    mVconn.DumpPacket(reinterpret_cast<uint8_t*>(&ret_msg), ret_msg.get_length(), "FlowStats", std::cout);
                }
                return mVconn.Send(ret_msg);
            }
        } // OFPST_MORE
    } // OFDP_OK
    uint16_t type=OFPET_BAD_REQUEST;
    uint16_t code=OFPBRC_BAD_STAT;
    std::ostream null_out(0);
    if ( TranslateOfDpReturnToOFVal(response.return_code, type, code) ) {
        return report_bad_message(*temp, type, code, null_out);
    }

    return true;
}

/**
 *  
 * @author sraihan 
 * 
 * @param stats 
 * @param response 
 * 
 * @return bool 
 */
bool OFV4Parser::GetStatReply( const OfDpAggregateStatsT &stats, const OfDpResponseT& response)
{
    const ofp_message* temp = reinterpret_cast<const ofp_message*>(response.msg);
    if ( response.return_code == OFDP_OK ) {
        ofp_message lpcWriteBuffer[_OF_JUMBO_PACKET_SIZE/sizeof(ofp_message)];
        bzero(reinterpret_cast<char*>(lpcWriteBuffer), _OF_JUMBO_PACKET_SIZE);
        ofp_message& ret_msg = lpcWriteBuffer[0];
        ofp_aggregate_stats& aggrStat = static_cast<ofp_aggregate_stats&>(ret_msg);
        aggrStat.ofp_message::version = this->get_wire_version();
        aggrStat.ofp_message::type = OFPT_STATS_REPLY;
        aggrStat.ofp_message::xid = temp->xid;
        aggrStat.ofp_message::set_length(sizeof(aggrStat));
        aggrStat.ofp_stats_msg::set_stats_type(OFPST_AGGREGATE);
        aggrStat.ofp_stats_msg::set_flags(0); // nothing will follow

        aggrStat.set_packet_count(stats.packet_count);
        aggrStat.set_byte_count(stats.byte_count);
        aggrStat.set_flow_count(stats.flow_count);
        if ( (gDbgMask & DBG_DETAIL) ) {
            mVconn.DumpPacket(reinterpret_cast<uint8_t*>(&ret_msg), ret_msg.get_length(), "AggrStat", std::cout);
        }
        return mVconn.Send((const ofp_message&)(aggrStat));
    }
    uint16_t type=OFPET_BAD_REQUEST;
    uint16_t code=OFPBRC_BAD_STAT;
    std::ostream null_out(0);
    if ( TranslateOfDpReturnToOFVal(response.return_code, type, code) ) {
        return report_bad_message(*temp, type, code, null_out);
    }
    return true;
}

/**
 *  
 * @author sraihan 
 * 
 * @param info 
 * @param response 
 * 
 * @return bool 
 */
bool OFV4Parser::GetConfigReply( const OfDpQueueConfigT& info, const OfDpResponseT& response)
{
    const ofp_message* temp = reinterpret_cast<const ofp_message*>(response.msg);
    if ( response.return_code == OFDP_OK ) {
        //process reply
        // determine the size first
        uint16_t msg_size = sizeof(ofp_queue_get_config_reply);
        for ( unsigned i=0;i<info.num_queues; i++ ) {
            unsigned cur_queue_size = sizeof(ofp_packet_queue);
            for ( unsigned j = 0; j < info.queues[i].num_properties; j++ ) {
                if ( info.queues[i].properties[j].property == OFDP_OFPQT_MIN_RATE ) {
                    cur_queue_size += sizeof(ofp_queue_prop_rate);
                } else {
                    cur_queue_size += sizeof(ofp_queue_prop_header);
                }
            }
            msg_size += cur_queue_size;
        }
        ofp_message lpcWriteBuffer[_OF_JUMBO_PACKET_SIZE/sizeof(ofp_message)];
        bzero(reinterpret_cast<char*>(lpcWriteBuffer), _OF_JUMBO_PACKET_SIZE);
        ofp_message& msg = lpcWriteBuffer[0];
        msg.version = get_wire_version();
        msg.set_length(msg_size);
        msg.type = OFPT_QUEUE_GET_CONFIG_REPLY;
        msg.xid = temp->xid;
        ofp_queue_get_config_reply& reply =(ofp_queue_get_config_reply&) (msg);
        reply.port = (static_cast<const ofp_queue_get_config_request*>(temp))->port;
        for ( unsigned i=0;i<info.num_queues; i++ ) {
            unsigned cur_queue_size = sizeof(reply.queues[i]);
            reply.queues[i].set_queue_id(info.queues[i].queue_id);
            for ( unsigned j = 0; j < info.queues[i].num_properties; j++ ) {
                if ( info.queues[i].properties[j].property == OFDP_OFPQT_MIN_RATE ) {
                    ((ofp_queue_prop_rate&)reply.queues[i].properties[j]).set_rate(info.queues[i].properties[j].data.rate);
                    reply.queues[i].properties[j].set_property( OFPQT_MIN_RATE );
                    reply.queues[i].properties[j].set_len(sizeof(ofp_queue_prop_rate));
                    cur_queue_size += sizeof(ofp_queue_prop_rate);
                } else {
                    reply.queues[i].properties[j].set_property( OFPQT_NONE );
                    reply.queues[i].properties[j].set_len(sizeof(ofp_queue_prop_header));
                    cur_queue_size += sizeof(ofp_queue_prop_header);
                }
            }
            reply.queues[i].set_len(cur_queue_size);
        }
        if ( (gDbgMask & DBG_DETAIL) ) {
            mVconn.DumpPacket(reinterpret_cast<uint8_t*>(&msg), msg.get_length(), "QueueConfig", std::cout);
        }
        return mVconn.Send(msg);
    }
    uint16_t type=OFPET_QUEUE_OP_FAILED;
    uint16_t code=OFPQOFC_BAD_PORT;
    std::ostream null_out(0);
    if ( TranslateOfDpReturnToOFVal(response.return_code, type, code) ) {
        return report_bad_message(*temp, type, code, null_out);
    }
    return true;
}

/**
 *  
 * @author sraihan 
 * 
 * @param stats 
 * @param response 
 * 
 * @return bool 
 */
bool OFV4Parser::GetStatReply(const OfDpGroupDescStatsT &stats, const OfDpResponseT& response)
{
    const ofp_message* temp = reinterpret_cast<const ofp_message*>(response.msg);
    if ( response.return_code == OFDP_OK ) {
        ofp_message lpcWriteBuffer[_OF_JUMBO_PACKET_SIZE/sizeof(ofp_message)];
        bzero(reinterpret_cast<char*>(lpcWriteBuffer), _OF_JUMBO_PACKET_SIZE);
        ofp_message& ret_msg = lpcWriteBuffer[0];
        ofp_group_desc_stats& groupDescStats = static_cast<ofp_group_desc_stats&>(ret_msg);
        groupDescStats.version = get_wire_version();
        groupDescStats.ofp_message::type = OFPT_STATS_REPLY;
        groupDescStats.ofp_message::set_length(groupDescStats.get_minimum_length());
        groupDescStats.xid = temp->xid;
        groupDescStats.ofp_stats_msg::set_stats_type(OFPST_GROUP_DESC);
        groupDescStats.ofp_stats_msg::set_flags(stats.flags);
        groupDescStats.body[0].group_type = stats.type;
        groupDescStats.body[0].set_group_id(stats.group_id);
        if ( EncodeBuckets(ret_msg, groupDescStats.body[0].buckets, stats.buckets.data(), stats.num_buckets, std::cout ) ) {
            groupDescStats.body[0].set_length(ret_msg.get_length() - sizeof(ofp_stats_msg));
            if ( gDbgMask == (DBG_DETAIL|DBG_PARSER_PARSE) )
                mVconn.DumpPacket(reinterpret_cast<uint8_t*>(&ret_msg), ret_msg.get_length(), " OfDpGroupDescStatsT ", std::cout);
            return mVconn.Send(ret_msg);
        }
    }
    uint16_t type=OFPET_BAD_REQUEST;
    uint16_t code=OFPBRC_BAD_STAT;
    std::ostream null_out(0);
    if ( TranslateOfDpReturnToOFVal(response.return_code, type, code) ) {
        return report_bad_message(*temp, type, code, null_out);
    }
    return true;
}

/**
 *  
 * @author sraihan 
 * 
 * @param stats 
 * @param response 
 * 
 * @return bool 
 */
bool OFV4Parser::GetStatReply(const OfDpGroupFeatureStatsT &stats, const OfDpResponseT& response)
{
    const ofp_message* temp = reinterpret_cast<const ofp_message*>(response.msg);
    if ( response.return_code == OFDP_OK ) {
        ofp_message lpcWriteBuffer[_OF_JUMBO_PACKET_SIZE/sizeof(ofp_message)];
        bzero(reinterpret_cast<char*>(lpcWriteBuffer), _OF_JUMBO_PACKET_SIZE);
        ofp_message& ret_msg = lpcWriteBuffer[0];
        ofp_group_features_stats& groupFeatureStats = static_cast<ofp_group_features_stats&>(ret_msg);
        groupFeatureStats.version = get_wire_version();
        groupFeatureStats.ofp_message::type = OFPT_STATS_REPLY;
        groupFeatureStats.ofp_message::set_length(groupFeatureStats.get_minimum_length());
        groupFeatureStats.xid = temp->xid;
        groupFeatureStats.ofp_stats_msg::set_stats_type(OFPST_GROUP_FEATURES);
        groupFeatureStats.ofp_stats_msg::set_flags(stats.flags);
        groupFeatureStats.set_capabilities(stats.feature.capabilities);
        groupFeatureStats.set_types(stats.feature.types);
        for ( int i=0; i < OFDP_GROUP_MAX_TYPE && i < 4; i++ ) {
            groupFeatureStats.max_groups[i] = htonl(stats.feature.max_groups[i]);
            groupFeatureStats.actions[i] = htonl(stats.feature.actions[i]);
        }
        return mVconn.Send(ret_msg);
    }
    uint16_t type=OFPET_BAD_REQUEST;
    uint16_t code=OFPBRC_BAD_STAT;
    std::ostream null_out(0);
    if ( TranslateOfDpReturnToOFVal(response.return_code, type, code) ) {
        return report_bad_message(*temp, type, code, null_out);
    }
    return true;
}

/**
 *  
 * @author sraihan 
 * 
 * @param retVal 
 * @param type 
 * @param code 
 * 
 * @return bool 
 */
bool OFV4Parser::TranslateOfDpReturnToOFVal(OfDpReturn retVal, uint16_t& type, uint16_t& code) const
{
    switch ( retVal ) {
    case OFDP_ERROR_UNKNOWN:
        break;
    case OFDP_LS_NOT_EXIST:
        break;
    case OFDP_ERR_TABLE_ID:
        if ( type == OFPET_FLOW_MOD_FAILED ) {
            code = OFPFMFC_BAD_TABLE_ID;
        }
        break;
    case OFDP_ENTRY_EXISTS:
        if ( type == OFPET_FLOW_MOD_FAILED ) {
            code = OFPFMFC_OVERLAP;
        } else if ( type == OFPET_GROUP_MOD_FAILED ) {
            code = OFPGMFC_GROUP_EXISTS;
        } else if ( type == OFPET_PORT_MOD_FAILED ) {
            code = OFPPMFC_BAD_PORT;
        } else if ( type == OFPET_QUEUE_OP_FAILED ) {
            code = OFPQOFC_BAD_QUEUE;
        } else if ( type == OFPET_TABLE_MOD_FAILED ) {
            code = OFPTMFC_BAD_TABLE;
        }
        break;
    case OFDP_ENTRY_NOT_EXIST:
        if ( type == OFPET_FLOW_MOD_FAILED ) {
            code = OFPFMFC_BAD_COMMAND;
        } else if ( type == OFPET_PORT_MOD_FAILED ) {
            code = OFPPMFC_BAD_PORT;
        } else if ( type == OFPET_QUEUE_OP_FAILED ) {
            code = OFPQOFC_BAD_QUEUE;
        } else if ( type == OFPET_GROUP_MOD_FAILED ) {
            code = OFPGMFC_INVALID_GROUP;
        } else if ( type == OFPET_TABLE_MOD_FAILED ) {
            code = OFPTMFC_BAD_TABLE;
        }
        break;
    case OFDP_ERR_INPUT:
        break;
    case OFDP_ERR_NOT_ACTIVATABLE:
        break;
    case OFDP_ERR_NOT_ACTIVATED:
        break;
    case OFDP_ERR_ACTIVATED:
        break;
    case OFDP_ERR_UNKNOWN_COMMAND:
        break;
    case OFDP_ERR_NO_FREE_ENTRY:
        break;
    case OFDP_ERR_ATTACHED:
        break;
    case OFDP_UNSUPPORTED_VERSION:
        if ( type == OFPET_BAD_REQUEST ) {
            code = OFPBRC_BAD_VERSION;
        }
        break;
    case OFDP_STATS_NOT_AVAILABLE: // OFPBRC_BAD_STAT
        if ( type == OFPET_BAD_REQUEST ) {
            code = OFPBRC_BAD_STAT;
        }
        break;
    case OFDP_ERR_TABLE_FULL:
        if ( type == OFPET_FLOW_MOD_FAILED ) {
            code = OFPFMFC_TABLE_FULL;
        }
        break;
    case OFDP_ERR_FLOW_OVERLAP:
        if ( type == OFPET_FLOW_MOD_FAILED ) {
            code = OFPFMFC_OVERLAP;
        }
        break;
    case OFDP_BUFFER_UNKNOWN:
        if ( type == OFPET_BAD_REQUEST ) {
            code = OFPBRC_BUFFER_UNKNOWN;
        }
        break;
    case OFDP_ERR_NOT_ACTIVATED_PENDING:
        break;
    case OFDP_BAD_PORT_ID:
        if ( type == OFPET_FLOW_MOD_FAILED ) {
            code = OFPFMFC_BAD_COMMAND;
        } else if ( type == OFPET_PORT_MOD_FAILED ) {
            code = OFPPMFC_BAD_PORT;
        } else if ( type == OFPET_QUEUE_OP_FAILED ) {
            code = OFPQOFC_BAD_PORT;
        } else {
            type = OFPET_BAD_REQUEST;
            code = OFPBRC_BAD_PORT;
        }
        break;
    case OFDP_BAD_QUEUE_ID:
        if ( type == OFPET_QUEUE_OP_FAILED ) {
            code = OFPQOFC_BAD_QUEUE;
        }
        break;
    case OFDP_INSTRUCTION_BAD_TABLE_ID:
        type = OFPET_BAD_INSTRUCTION;
        code = OFPBIC_BAD_TABLE_ID;
        break;
    case OFDP_INSTRUCTION_UNSUP_METADATA:
        type = OFPET_BAD_INSTRUCTION;
        code = OFPBIC_UNSUP_METADATA;
        break;
    case OFDP_INSTRUCTION_UNSUP_METADATA_MASK:
        type = OFPET_BAD_INSTRUCTION;
        code = OFPBIC_UNSUP_METADATA_MASK;
        break;
    case OFDP_ACTION_BAD_TYPE:
        type = OFPET_BAD_ACTION;
        code = OFPBAC_BAD_TYPE;
        break;
    case OFDP_ACTION_BAD_OUT_PORT:
        type = OFPET_BAD_ACTION;
        code = OFPBAC_BAD_OUT_PORT;
        break;
    case OFDP_ACTION_BAD_ARGUMENT:
        type = OFPET_BAD_ACTION;
        code = OFPBAC_BAD_ARGUMENT;
        break;
    case OFDP_ACTION_BAD_SET_ARGUMENT:
        type = OFPET_BAD_ACTION;
        code = OFPBAC_BAD_SET_ARGUMENT;
        break;
    case OFDP_ACTION_TOO_MANY:
        type = OFPET_BAD_ACTION;
        code = OFPBAC_TOO_MANY;
        break;
    case OFDP_ACTION_BAD_QUEUE:
        type = OFPET_BAD_ACTION;
        code = OFPBAC_BAD_QUEUE;
        break;
    case OFDP_ACTION_BAD_OUT_GROUP:
        type = OFPET_BAD_ACTION;
        code = OFPBAC_BAD_OUT_GROUP;
        break;
    case OFDP_ACTION_MATCH_INCONSISTENT:
        type = OFPET_BAD_ACTION;
        code = OFPBAC_MATCH_INCONSISTENT;
        break;
    case OFDP_ACTION_UNSUPPORTED_ORDER:
        type = OFPET_BAD_ACTION;
        code = OFPBAC_UNSUPPORTED_ORDER;
        break;
    case OFDP_ACTION_BAD_TAG:
        type = OFPET_BAD_ACTION;
        code = OFPBAC_BAD_TAG;
        break;
    case OFDP_MATCH_BAD_ETHERTYPE:
        type = OFPET_BAD_MATCH;
        code = OFPBMC_BAD_VALUE;
        break;
    case OFDP_MATCH_BAD_TYPE:
        type = OFPET_BAD_MATCH;
        code = OFPBMC_BAD_TYPE;
        break;
    case OFDP_MATCH_BAD_TAG:
        type = OFPET_BAD_MATCH;
        code = OFPBMC_BAD_TAG;
        break;
    case OFDP_MATCH_BAD_DL_ADDR_MASK:
        type = OFPET_BAD_MATCH;
        code = OFPBMC_BAD_DL_ADDR_MASK;
        break;
    case OFDP_MATCH_BAD_NW_ADDR_MASK:
        type = OFPET_BAD_MATCH;
        code = OFPBMC_BAD_NW_ADDR_MASK;
        break;
    case OFDP_MATCH_BAD_WILDCARDS:
        type = OFPET_BAD_MATCH;
        code = OFPBMC_BAD_WILDCARDS;
        break;
    case OFDP_MATCH_BAD_FIELD:
        type = OFPET_BAD_MATCH;
        code = OFPBMC_BAD_FIELD;
        break;
    case OFDP_MATCH_BAD_VALUE:
        type = OFPET_BAD_MATCH;
        code = OFPBMC_BAD_VALUE;
        break;
    case OFDP_MATCH_BAD_MASK:
        type = OFPET_BAD_MATCH;
        code = OFPBMC_BAD_MASK;
        break;
    case OFDP_MATCH_BAD_PREREQ:
        type = OFPET_BAD_MATCH;
        code = OFPBMC_BAD_PREREQ;
        break;
    case OFDP_MATCH_DUP_FIELD:
        type = OFPET_BAD_MATCH;
        code = OFPBMC_DUP_FIELD;
        break;
    case OFDP_GROUP_INVALID_GROUP:
        type = OFPET_GROUP_MOD_FAILED;
        code = OFPGMFC_INVALID_GROUP;
        break;
    case OFDP_GROUP_WEIGHT_UNSUPPORTED:
        type = OFPET_GROUP_MOD_FAILED;
        code = OFPGMFC_WEIGHT_UNSUPPORTED;
        break;
    case OFDP_GROUP_OUT_OF_GROUPS:
        type = OFPET_GROUP_MOD_FAILED;
        code = OFPGMFC_OUT_OF_GROUPS;
        break;
    case OFDP_GROUP_OUT_OF_BUCKETS:
        type = OFPET_GROUP_MOD_FAILED;
        code = OFPGMFC_OUT_OF_BUCKETS;
        break;
    case OFDP_GROUP_CHAINING_UNSUPPORTED:
        type = OFPET_GROUP_MOD_FAILED;
        code = OFPGMFC_CHAINING_UNSUPPORTED;
        break;
    case OFDP_GROUP_WATCH_UNSUPPORTED:
        type = OFPET_GROUP_MOD_FAILED;
        code = OFPGMFC_WATCH_UNSUPPORTED;
        break;
    case OFDP_GROUP_LOOP:
        type = OFPET_GROUP_MOD_FAILED;
        code = OFPGMFC_LOOP;
        break;
    case OFDP_GROUP_CHAINED_GROUP:
        type = OFPET_GROUP_MOD_FAILED;
        code = OFPGMFC_CHAINED_GROUP;
        break;
    case OFDP_GROUP_BAD_TYPE:
        type = OFPET_GROUP_MOD_FAILED;
        code = OFPGMFC_BAD_TYPE;
        break;
    case OFDP_GROUP_BAD_WATCH:
        type = OFPET_GROUP_MOD_FAILED;
        code = OFPGMFC_BAD_WATCH;
        break;
        // Following are not error
    case OFDP_LINX_RETURN_MAX:
    case OFDP_PENDING:
    default:
        return false; // not an error
        break;
    };
    return true;
}

/**
 *  
 * @author sraihan 
 * 
 * @param stats 
 * @param response 
 * 
 * @return bool 
 */
bool OFV4Parser::GetStatReply( const OfDpPortStatsT &stats, const OfDpResponseT& response)
{
    const ofp_message* temp = reinterpret_cast<const ofp_message*>(response.msg);
    if ( response.return_code == OFDP_OK ) {
        ofp_message lpcWriteBuffer[_OF_JUMBO_PACKET_SIZE/sizeof(ofp_message)];
        bzero(reinterpret_cast<char*>(lpcWriteBuffer), _OF_JUMBO_PACKET_SIZE);
        ofp_message& msg = lpcWriteBuffer[0];
        ofp_port_stats& portStats = static_cast<ofp_port_stats&>(msg);
        portStats.version = get_wire_version();
        portStats.ofp_message::type = OFPT_STATS_REPLY;
        portStats.ofp_message::set_length(portStats.get_minimum_length());
        portStats.xid = temp->xid;
        portStats.ofp_stats_msg::set_stats_type(OFPST_PORT);
        portStats.ofp_stats_msg::set_flags(stats.flags);
        portStats.body[0].set_port_no(stats.port_no);
        portStats.body[0].set_rx_packets(stats.rx_packets);
        portStats.body[0].set_tx_packets(stats.tx_packets);
        portStats.body[0].set_rx_bytes(stats.rx_bytes);
        portStats.body[0].set_tx_bytes(stats.tx_bytes);
        portStats.body[0].set_rx_dropped(stats.rx_dropped);
        portStats.body[0].set_tx_dropped(stats.tx_dropped);
        portStats.body[0].set_rx_errors(stats.rx_errors);
        portStats.body[0].set_tx_errors(stats.rx_errors);
        portStats.body[0].set_rx_frame_err(stats.rx_frame_err);
        portStats.body[0].set_rx_over_err(stats.rx_over_err);
        portStats.body[0].set_rx_crc_err(stats.rx_crc_err);
        portStats.body[0].set_collisions(stats.collisions);
        portStats.body[0].set_duration_sec(stats.duration_sec);
        portStats.body[0].set_duration_nsec(stats.duration_nsec);
        return mVconn.Send((const ofp_message& )((portStats)));

    }
    uint16_t type=OFPET_BAD_REQUEST;
    uint16_t code=OFPBRC_BAD_STAT;
    std::ostream null_out(0);
    if ( TranslateOfDpReturnToOFVal(response.return_code, type, code) ) {
        return report_bad_message(*temp, type, code, null_out);
    }
    return true;
}

/**
 *  
 * @author sraihan 
 * 
 * @param stats 
 * @param response 
 * 
 * @return bool 
 */
bool OFV4Parser::GetStatReply(const OfDpQueueStatsT &stats, const OfDpResponseT& response)
{
    const ofp_message* temp = reinterpret_cast<const ofp_message*>(response.msg);
    if ( response.return_code == OFDP_OK ) {
        ofp_message lpcWriteBuffer[_OF_JUMBO_PACKET_SIZE/sizeof(ofp_message)];
        bzero(reinterpret_cast<char*>(lpcWriteBuffer), _OF_JUMBO_PACKET_SIZE);
        ofp_message& msg = lpcWriteBuffer[0];
        ofp_queue_stats& queueStats = static_cast<ofp_queue_stats&>(msg);
        queueStats.version = get_wire_version();
        queueStats.ofp_message::type = OFPT_STATS_REPLY;
        queueStats.ofp_message::set_length(queueStats.get_minimum_length());
        queueStats.xid = temp->xid;
        queueStats.ofp_stats_msg::set_stats_type(OFPST_QUEUE);
        queueStats.ofp_stats_msg::set_flags(stats.flags);
        queueStats.body[0].set_port_no(stats.port_no);
        queueStats.body[0].set_queue_id(stats.queue_id);
        queueStats.body[0].set_tx_packets(stats.tx_packets);
        queueStats.body[0].set_tx_bytes(stats.tx_bytes);
        queueStats.body[0].set_tx_errors(stats.tx_errors);
        return mVconn.Send((const ofp_message& )((queueStats)));

    }
    uint16_t type=OFPET_BAD_REQUEST;
    uint16_t code=OFPBRC_BAD_STAT;
    std::ostream null_out(0);
    if ( TranslateOfDpReturnToOFVal(response.return_code, type, code) ) {
        return report_bad_message(*temp, type, code, null_out);
    }
    return true;
}

/**
 * @see  OfDpPacketInT
 * @author sraihan 
 * 
 * @param info 
 * 
 * @return bool 
 */
bool OFV4Parser::SendPacketIn(const OfDpPacketInT& info)
{
    if ( !mVconn.IsOpen() || mVconn.IsPassive() ) return false;
    // ofp_message header
    ofp_message buffer_to_send[1600 - sizeof(ofp_packet_in)]={0}; 
    bzero(reinterpret_cast<char*>(buffer_to_send), sizeof(buffer_to_send));
    ofp_message& msg = buffer_to_send[0];
    msg.set_length(sizeof(OpenflowV4::ofp_packet_in));
    msg.version = get_wire_version();
    msg.type = OpenflowV4::OFPT_PACKET_IN;
    msg.xid = 0;
    // ofp_packet_in header
    static_cast<OpenflowV4::ofp_packet_in&>(msg).set_buffer_id(info.buffer_id);
    static_cast<OpenflowV4::ofp_packet_in&>(msg).table_id = info.table_id;
    static_cast<OpenflowV4::ofp_packet_in&>(msg).reason = info.reason;
    static_cast<OpenflowV4::ofp_packet_in&>(msg).set_total_len(info.total_len);
    OfDpMatchT match;
    match.in_port = info.in_port;
    if ( !EncodeOXMFields<OpenflowV4::ofp_packet_in>(msg,match) ) return false;
    static_cast<OpenflowV4::ofp_packet_in&>(msg).match.set_type(OFPMT_OXM);
    // Packet in data
    msg.set_length(msg.get_length() + 2 +  info.total_len);
    uint8_t* data = reinterpret_cast<uint8_t*>(&msg) + msg.get_length() - info.total_len;
    memcpy(data, info.message, info.total_len);

    const bool retVal = mVconn.Send(msg, false);
    if ( retVal && (gDbgMask & DBG_DETAIL) ) {
        mVconn.DumpPacket(data, info.total_len, "PacketIn", std::cout);
    }

    return retVal;
}

/**
 * @see ofp_switch_features
 * 
 * @author sraihan 
 * 
 * @param msg 
 * @param out 
 * 
 * @return bool 
 */
bool OFV4Parser::GetFeature(const ofp_message & msg, std::ostream& out) {
    const char* func_name = "iOpenFlowV4Impl::GetFeature \n";
    out << func_name;
    if ( msg.get_length() != msg.get_minimum_length() ) {
        out << "\nBad Len, Expected: " << msg.get_minimum_length() << ", Got: " << msg.get_length() << "\n";
        report_bad_message(msg,OFPET_BAD_REQUEST, OFPBRC_BAD_LEN, out);
        return false; // stop processing further in this batch
    }

    OfDpSwitchFeaturesT ret_msg;

    PREPARE_OFDP_RESPONSE(msg)
    if ( mVconn.mConnMgr.mLogicalSw.GetFeature(ret_msg, response) ) {
        GetFeatureReply(ret_msg, response);
    }
    return true;
}

/**
 * @see OfDpSwitchFeaturesT
 * @see GetFeature
 * @author sraihan 
 * 
 * @param info 
 * @param response 
 * 
 * @return bool 
 */
bool OFV4Parser::GetFeatureReply(const OfDpSwitchFeaturesT& info, const OfDpResponseT& response)
{
    const ofp_message* temp = reinterpret_cast<const ofp_message*>(response.msg);
    if ( response.return_code == OFDP_OK ) {
        ofp_message lpcWriteBuffer[_OF_JUMBO_PACKET_SIZE/sizeof(ofp_message)];
        bzero(reinterpret_cast<char*>(lpcWriteBuffer), _OF_JUMBO_PACKET_SIZE);
        ofp_message& msg = lpcWriteBuffer[0];
        ofp_switch_features& reply = static_cast<ofp_switch_features&>(msg);
        reply.set_length(sizeof(reply));
        reply.type = OFPT_FEATURES_REPLY;
        reply.version = get_wire_version();
        reply.xid = temp->xid;
        reply.n_tables = info.n_tables;
        reply.auxiliary_id = 0; // TODO: fix when auxilary channel is available
        reply.set_n_buffers(info.n_buffers);
        // TODO: Is capabilities bit mapped same across spec?
        reply.set_capabilities(info.capabilities);
        if ( info.datapath_id == 0 ) {
            reply.set_datapath_id(make_dpid());
        } else {
            reply.set_datapath_id(info.datapath_id);
        }
        return mVconn.Send(reply);
    }
    uint16_t type=OFPET_BAD_REQUEST;
    uint16_t code=OFPBRC_BAD_TYPE;
    std::ostream null_out(0);
    if ( TranslateOfDpReturnToOFVal(response.return_code, type, code) ) {
        return report_bad_message(*temp, type, code, null_out);
    }
    return true;
}

/**
 * @see ofp_table_mod
 * 
 * @author sraihan 
 * 
 * @param msg 
 * @param out 
 * 
 * @return bool 
 */
bool OFV4Parser::TableMod(const ofp_message & msg, std::ostream& out) {
    if ( mVconn.IsPassive() ) {
        return report_bad_message(msg, OFPET_BAD_REQUEST, OFPBRC_IS_SLAVE, out);
    }
    out << "\niOpenFlowV4Impl::TableMod: not supported.\n";
    return true;
}

/** 
 * msg.type == OFPT_STATS_REQUEST 
 * @see ofp_stats_msg
 * 
 * @author sraihan 
 * 
 * @param msg 
 * @param out 
 * 
 * @return bool 
 */
bool OFV4Parser::StatsReq(const ofp_message & msg, std::ostream& out) {
    const ofp_stats_msg& stat_msg = static_cast<const ofp_stats_msg&>(msg);
    out << "\n iOpenFlowV4Impl::StatsReq - ";
    if ( msg.get_length() < stat_msg.get_minimum_length() ) {
        out << "\n Bad Len, expected minimum: " << stat_msg.get_minimum_length() << ", Got: " << msg.get_length() << "\n";
        report_bad_message(msg,OFPET_BAD_REQUEST, OFPBRC_BAD_LEN, out);
        return false; // stop processing further in this batch
    }
    bool retVal = true;
    out << stat_msg << "\n";

    //printf(" in  iOpenFlowV4Impl::StatsReq type=%d\n", stat_msg.get_stats_type() );
    switch ( stat_msg.get_stats_type() ) {
    case OFPST_DESC:
        out << " OFPST_DESC -";
        if ( msg.get_length() != ((ofp_stats_msg&)msg).get_minimum_length() ) {
            out << " bad length ";
            report_bad_message(msg, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN, out);
            return false;
        }
        retVal = GetDescStat(msg, out);
        break;
    case OFPST_FLOW:
        out << " OFPST_FLOW -";
        if ( msg.get_length() < ((ofp_flow_stats_request&)msg).get_minimum_length() ) {
            out << " bad length ";
            report_bad_message(msg, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN, out);
            return false;
        }
        retVal = GetFlowStat(msg, out);
        break;
    case OFPST_AGGREGATE:
        out << " OFPST_AGGREGATE -";
        if ( msg.get_length() < ((ofp_aggregate_stats_request&)msg).get_minimum_length() ) {
            out << " bad length ";
            report_bad_message(msg, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN, out);
            return false;
        }
        retVal = GetAggregateStat(msg, out);
        break;
    case OFPST_TABLE:
        out << " OFPST_TABLE -";
        if ( msg.get_length() != ((ofp_stats_msg&)msg).get_minimum_length() ) {
            out << " bad length ";
            report_bad_message(msg, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN, out);
            return false;
        }
        retVal = GetTableStat(msg, out);
        break;
    case OFPST_PORT:
        out << " OFPST_PORT -";
        if ( msg.get_length() != ((ofp_port_stats_request&)msg).get_minimum_length() ) {
            out << " bad length ";
            report_bad_message(msg, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN, out);
            return false;
        }
        retVal = GetPortStat(msg, out);
        break;
    case OFPST_QUEUE:
        out << " OFPST_QUEUE -";
        if ( msg.get_length() != ((ofp_queue_stats_request&)msg).get_minimum_length() ) {
            out << " bad length ";
            report_bad_message(msg, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN, out);
            return false;
        }
        retVal = GetQueueStat(msg, out);
        break;
    case OFPST_GROUP:
        out << " OFPST_GROUP -";
        if ( msg.get_length() != ((ofp_group_stats_request&)msg).get_minimum_length() ) {
            out << " bad length ";
            report_bad_message(msg, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN, out);
            return false;
        }
        retVal = GetGroupStat(msg, out);
        break;
    case OFPST_GROUP_DESC:
        out << " OFPST_GROUP_DESC -";
        if ( msg.get_length() != ((ofp_stats_msg&)msg).get_minimum_length() ) {
            out << " bad length ";
            report_bad_message(msg, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN, out);
            return false;
        }
        retVal = GetGroupDescStat(msg, out);
        break;
    case OFPST_GROUP_FEATURES:
        out << " OFPST_GROUP_FEATURES -";
        if ( msg.get_length() != ((ofp_stats_msg&)msg).get_minimum_length() ) {
            out << " bad length ";
            report_bad_message(msg, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN, out);
            return false;
        }
        retVal = GetGroupFeatureStat(msg, out);
        break;
    case OpenflowV4::OFPMP_TABLE_FEATURES:
        out << " OFPMP_TABLE_FEATURES -";
        if ( msg.get_length() > ((OpenflowV4::ofp_table_features_stats&)msg).get_minimum_length() ) {
            out << " bad length ";
            report_bad_message(msg, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN, out);
            return false;
        }
        retVal = GetTableFeatureStat(msg, out);
        break;
    case OpenflowV4::OFPMP_PORT_DESC:
        out << " OFPMP_PORT_DESC -";
        if ( msg.get_length() != ((ofp_stats_msg&)msg).get_minimum_length() ) {
            out << " bad length ";
            report_bad_message(msg, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN, out);
            return false;
        }
        retVal = GetPortList(msg, out);
        break;
    case OpenflowV4::OFPMP_METER:
    case OpenflowV4::OFPMP_METER_CONFIG:
    case OpenflowV4::OFPMP_METER_FEATURES:
        return true; /// TODO: Avoid continuous return of error messages
    default:
        out << " bad subtype or not supported ";
        retVal = report_bad_message(msg, OFPET_BAD_REQUEST, OFPBRC_BAD_TYPE, out); // bad type
        break;
    };

    if ( retVal ) {
        out << " stat response is sent or pending";
    } else {
        out << " Not sending a stat response now ";
    }

    return retVal;
}

/**
 * @see OfDpTableStatsT
 * 
 * @author sraihan 
 * 
 * @param stats 
 * @param response 
 * 
 * @return bool 
 */
bool OFV4Parser::GetStatReply( const OfDpTableStatsT &stats, const OfDpResponseT& response)
{
    const ofp_message* temp = reinterpret_cast<const ofp_message*>(response.msg);
    if ( response.return_code == OFDP_OK ) {
        ofp_message lpcWriteBuffer[_OF_JUMBO_PACKET_SIZE/sizeof(ofp_message)];
        bzero(reinterpret_cast<char*>(lpcWriteBuffer), _OF_JUMBO_PACKET_SIZE);
        ofp_message& msg = lpcWriteBuffer[0];
        ofp_table_stats& tableStat = static_cast<ofp_table_stats&>(msg);
        tableStat.ofp_message::version = this->get_wire_version();
        tableStat.ofp_message::type = OFPT_STATS_REPLY;
        tableStat.ofp_message::xid = temp->xid;
        tableStat.ofp_message::set_length(tableStat.get_minimum_length());

        tableStat.ofp_stats_msg::set_stats_type(OFPST_TABLE);
        tableStat.ofp_stats_msg::set_flags(stats.flags); // nothing will follow

        tableStat.body[0].set_active_count(stats.active_count);
        tableStat.body[0].set_lookup_count(stats.lookup_count);
        tableStat.body[0].set_matched_count(stats.matched_count);
        tableStat.body[0].table_id = stats.table_id;
        return mVconn.Send((const ofp_message&)(tableStat));
    }
    uint16_t type=OFPET_BAD_REQUEST;
    uint16_t code=OFPBRC_BAD_STAT;
    std::ostream null_out(0);
    if ( TranslateOfDpReturnToOFVal(response.return_code, type, code) ) {
        return report_bad_message(*temp, type, code, null_out);
    }
    return true;
}

/**
 * @see OfDpPortListT
 * 
 * @author sraihan 
 * 
 * @param param 
 * @param out 
 * 
 * @return bool 
 */
bool OFV4Parser::GetPortList(const ofp_message& param, std::ostream& out)
{
    OfDpPortListT reply;
    PREPARE_OFDP_RESPONSE(param)
    if ( mVconn.mConnMgr.mLogicalSw.GetStat(reply, response) ) {
        return GetStatReply(reply, response);
    }
    return true;
}

/**
 * @see ofp_table_features_stats
 * @see OfDpTableFeatureStatsT
 * @author sraihan 
 * 
 * @param param 
 * @param out 
 * 
 * @return bool 
 */
bool OFV4Parser::GetTableFeatureStat(const ofp_message& param, std::ostream& out)
{
    OfDpTableFeatureStatsT reply;
    const ofp_table_features_stats& request = static_cast<const ofp_table_features_stats&>(param);
    reply.feature.table_id = request.body[0].table_id;
    //TODO: ignoring feature set, may need to look what we can allow configuration
    //WARN: This reconfiguration will break compatibility with older spec
    // and downgrade of spec should not be allowed
    PREPARE_OFDP_RESPONSE(param)
    if ( mVconn.mConnMgr.mLogicalSw.GetStat(reply, response) ) {
        return GetStatReply(reply, response);
    }
    return true;
}

/**
 * @see ofp_port_list_stats
 * @see OfDpPortListT
 * @author sraihan 
 * 
 * @param info 
 * @param response 
 * 
 * @return bool 
 */
bool OFV4Parser::GetStatReply(const OfDpPortListT &info, const OfDpResponseT& response)
{
    const ofp_message* temp = reinterpret_cast<const ofp_message*>(response.msg);
    if ( response.return_code == OFDP_OK ) {
        ofp_message lpcWriteBuffer[_OF_JUMBO_PACKET_SIZE/sizeof(ofp_message)];
        bzero(reinterpret_cast<char*>(lpcWriteBuffer), _OF_JUMBO_PACKET_SIZE);
        ofp_message& msg = lpcWriteBuffer[0];
        ofp_port_list_stats& reply = static_cast<ofp_port_list_stats&>(msg);
        reply.set_length(sizeof(reply));
        reply.type = OFPT_STATS_REPLY;
        reply.version = get_wire_version();
        reply.xid = temp->xid;
        reply.ofp_stats_msg::set_stats_type(OpenflowV4::OFPMP_PORT_DESC);
        reply.ofp_stats_msg::set_flags(info.flags);
        ofp_phy_port* ports = reinterpret_cast<ofp_phy_port*>(reply.body);
        for ( const auto& port : info.ports ) {
            memcpy(ports[0].hw_addr, port.hw_addr,6);
            strncpy(ports[0].name, port.name.c_str(),sizeof(ports[0].name));
            ports[0].set_port_no(port.port_no);
            ports[0].set_advertised(port.advertised);
            ports[0].set_config(port.config);
            ports[0].set_curr(port.curr);
            ports[0].set_peer(port.peer);
            ports[0].set_state(port.state);
            ports[0].set_supported(port.supported);
        }
        return mVconn.Send(static_cast<ofp_message&>(reply));
    }
    uint16_t type=OFPET_BAD_REQUEST;
    uint16_t code=OFPBRC_BAD_STAT;
    std::ostream null_out(0);
    if ( TranslateOfDpReturnToOFVal(response.return_code, type, code) ) {
        return report_bad_message(*temp, type, code, null_out);
    }
    return true;
}

/**
 * @see OfDpTableFeatureStatsT
 * 
 * @author sraihan 
 * 
 * @param info 
 */
void OFV4Parser::DumpTableFeatureStatsT(const OfDpTableFeatureStatsT info)
{

    std::cout <<" instructions "<< std::hex<< info.feature.instructions << std::dec << std::endl;
    std::cout <<" instructions_miss "<< std::hex<< info.feature.instructions_miss<< std::dec << std::endl;
    std::cout <<" actions "<< std::hex<< info.feature.actions<< std::dec << std::endl;
    std::cout <<" next_tables "<< std::hex<< info.feature.next_tables<< std::dec << std::endl;
    std::cout <<" next_tables_miss "<< std::hex<< info.feature.next_tables_miss<< std::dec << std::endl;
    std::cout <<" write_actions "<< std::hex<< info.feature.write_actions<< std::dec << std::endl;
    std::cout <<" write_actions_miss "<< std::hex<< info.feature.write_actions_miss<< std::dec << std::endl;
    std::cout <<" apply_actions "<< std::hex<< info.feature.apply_actions<< std::dec << std::endl;
    std::cout <<" apply_actions_miss "<< std::hex<< info.feature.apply_actions_miss<< std::dec << std::endl;
    std::cout <<" match_fields "<< std::hex<< info.feature.match_fields<< std::dec << std::endl;
    std::cout <<" wildcard_fields "<< std::hex<< info.feature.wildcard_fields<< std::dec << std::endl;
    std::cout <<" write_set_fields "<< std::hex<< info.feature.write_set_fields<< std::dec << std::endl;
    std::cout <<" write_set_fields_miss "<< std::hex<< info.feature.write_set_fields_miss<< std::dec << std::endl;
    std::cout <<" apply_set_fields "<< std::hex<< info.feature.apply_set_fields<< std::dec << std::endl;
    std::cout <<" apply_set_fields_miss "<< std::hex<< info.feature.apply_set_fields_miss<< std::dec << std::endl;
}

/**
 * @see OfDpTableFeatureStatsT
 * @see ofp_table_features_stats
 * @author sraihan 
 * 
 * @param info 
 * @param response 
 * 
 * @return bool 
 */
bool OFV4Parser::GetStatReply(const OfDpTableFeatureStatsT &info, const OfDpResponseT& response)
{
    const ofp_message* temp = reinterpret_cast<const ofp_message*>(response.msg);

    //std::cout << "iOpenFlowV4Impl::GetStatReply StackTrace" << std::endl;
    //LnTraceback::LMSG_MyTraceback();

    //DumpTableFeatureStatsT(info);
    if ( response.return_code == OFDP_OK ) {
        ofp_message lpcWriteBuffer[_OF_JUMBO_PACKET_SIZE/sizeof(ofp_message)];
        bzero(reinterpret_cast<char*>(lpcWriteBuffer), _OF_JUMBO_PACKET_SIZE);
        ofp_message& msg = lpcWriteBuffer[0];
        ofp_table_features_stats& reply = static_cast<ofp_table_features_stats&>(msg);
        reply.set_length(sizeof(reply));
        reply.type = OFPT_STATS_REPLY;
        reply.version = get_wire_version();
        reply.xid = temp->xid;
        reply.ofp_stats_msg::set_stats_type(OpenflowV4::OFPMP_TABLE_FEATURES);
        reply.ofp_stats_msg::set_flags(info.flags);
        reply.body[0].table_id = info.feature.table_id;
        reply.body[0].set_stat_length(sizeof(reply.body[0]));
        strcpy(reply.body[0].name, info.feature.name);
        reply.body[0].set_metadata_match(info.feature.metadata_match);
        reply.body[0].set_metadata_write(info.feature.metadata_write);
        reply.body[0].set_config(info.feature.config);
        reply.body[0].set_max_entries(info.feature.max_entries);
        ofp_tlv_base* cur = reply.body[0].properties;
        const ofp_tlv_base* props = reply.body[0].properties;
        if ( !EncodeTableFeatureInstructions(info.feature.instructions, OFPTFPT_INSTRUCTIONS, cur,reply) ) {
            return false;
        }
        // count write actions
        props = props->get_next_padded();
        cur  = const_cast<ofp_tlv_base*>(props);
        if ( !EncodeTableFeatureActions(info.feature.write_actions, OFPTFPT_WRITE_ACTIONS, cur, reply) ) {
            return false;
        }

        // count apply actions
        props = props->get_next_padded();
        cur  = const_cast<ofp_tlv_base*>(props);
        if ( !EncodeTableFeatureActions(info.feature.apply_actions, OFPTFPT_APPLY_ACTIONS, cur, reply) ) {
            return false;
        }
        // count match fields
        props = props->get_next_padded();
        cur  = const_cast<ofp_tlv_base*>(props);
        if ( !EncodeTableFeatureOXM(info.feature.match_fields, OFPTFPT_MATCH, cur, reply) ) {
            return false;
        }
        // count wildcards
        props = props->get_next_padded();
        cur  = const_cast<ofp_tlv_base*>(props);
        if ( !EncodeTableFeatureOXM(info.feature.wildcard_fields, OFPTFPT_WILDCARDS, cur, reply) ) {
            return false;
        }
        // count apply set fields
        props = props->get_next_padded();
        cur  = const_cast<ofp_tlv_base*>(props);
        if ( !EncodeTableFeatureActions(info.feature.write_set_fields, OFPTFPT_WRITE_SETFIELD, cur, reply) ) {
            return false;
        }

        // count apply set fields
        props = props->get_next_padded();
        cur  = const_cast<ofp_tlv_base*>(props);
        if ( !EncodeTableFeatureActions(info.feature.apply_set_fields, OFPTFPT_APPLY_SETFIELD, cur, reply) ) {
            return false;
        }
        // Following properties are not supported - so these are empty lists
        // instructions miss tables
        props = props->get_next_padded();
        cur  = const_cast<ofp_tlv_base*>(props);
        if ( !EncodeTableFeatureInstructions(info.feature.instructions_miss, OFPTFPT_INSTRUCTIONS_MISS, cur,reply) ) {
            return false;
        }
        // write action miss tables
        props = props->get_next_padded();
        cur  = const_cast<ofp_tlv_base*>(props);
        if ( !EncodeTableFeatureActions(info.feature.write_actions_miss, OFPTFPT_WRITE_ACTIONS_MISS, cur, reply) ) {
            return false;
        }
        // apply action miss tables
        props = props->get_next_padded();
        cur  = const_cast<ofp_tlv_base*>(props);
        if ( !EncodeTableFeatureActions(info.feature.apply_actions_miss, OFPTFPT_APPLY_ACTIONS_MISS, cur, reply) ) {
            return false;
        }

        // write set field miss tables
        props = props->get_next_padded();
        cur  = const_cast<ofp_tlv_base*>(props);
        if ( !EncodeTableFeatureActions(info.feature.write_set_fields_miss, OFPTFPT_WRITE_SETFIELD_MISS, cur, reply) ) {
            return false;
        }

        // apply set field miss tables
        props = props->get_next_padded();
        cur  = const_cast<ofp_tlv_base*>(props);
        if ( !EncodeTableFeatureActions(info.feature.apply_set_fields_miss, OFPTFPT_APPLY_SETFIELD_MISS, cur, reply) ) {
            return false;
        }
        // We are not multi table yet
        // next tables
        props = props->get_next_padded();
        cur  = const_cast<ofp_tlv_base*>(props);
        reply.set_length( reply.get_length() + 8 );
        reply.body[0].set_stat_length(reply.body[0].get_stat_length() + 8);
        cur->set_type(OFPTFPT_NEXT_TABLES);
        cur->set_len( 4 );
        // next table miss
        props = props->get_next_padded();
        cur  = const_cast<ofp_tlv_base*>(props);
        reply.set_length( reply.get_length() + 8 );
        reply.body[0].set_stat_length(reply.body[0].get_stat_length() + 8);
        cur->set_type(OFPTFPT_NEXT_TABLES_MISS);
        cur->set_len( 4 );
        // experinter staff not yet, yet
        // experimenter tables
        props = props->get_next_padded();
        cur  = const_cast<ofp_tlv_base*>(props);
        reply.set_length( reply.get_length() + 16 ); // sizeof(ofp_table_feature_prop_experimenter) + 4
        reply.body[0].set_stat_length(reply.body[0].get_stat_length() + 16 ); // sizeof(ofp_table_feature_prop_experimenter) + 4
        cur->set_type(OFPTFPT_EXPERIMENTER);
        cur->set_len( sizeof(ofp_prop_experimenter) );
        // experimenter miss tables
        props = props->get_next_padded();
        cur  = const_cast<ofp_tlv_base*>(props);
        reply.set_length( reply.get_length() + 16 );
        reply.body[0].set_stat_length(reply.body[0].get_stat_length() + 16 );
        cur->set_type(OFPTFPT_EXPERIMENTER_MISS);
        cur->set_len( sizeof(ofp_prop_experimenter) );

        return mVconn.Send(msg);
    }
    uint16_t type=OFPET_BAD_REQUEST;
    uint16_t code=OFPBRC_BAD_STAT;
    std::ostream null_out(0);
    if ( TranslateOfDpReturnToOFVal(response.return_code, type, code) ) {
        return report_bad_message(*temp, type, code, null_out);
    }
    return true;
}

/**
 * @see ofp_group_stats
 * @see OfDpGroupStatsT
 * @author sraihan 
 * 
 * @param stats 
 * @param response 
 * 
 * @return bool 
 */
bool OFV4Parser::GetStatReply(const OfDpGroupStatsT& stats, const OfDpResponseT& response)
{
    const ofp_message* temp = reinterpret_cast<const ofp_message*>(response.msg);
    if ( response.return_code == OFDP_OK ) {
        ofp_message lpcWriteBuffer[_OF_JUMBO_PACKET_SIZE/sizeof(ofp_message)];
        bzero(reinterpret_cast<char*>(lpcWriteBuffer), _OF_JUMBO_PACKET_SIZE);
        ofp_message& ret_msg = lpcWriteBuffer[0];
        OpenflowV4::ofp_group_stats& groupStats = static_cast<OpenflowV4::ofp_group_stats&>(ret_msg);
        groupStats.version = get_wire_version();
        groupStats.ofp_message::type = OFPT_STATS_REPLY;
        groupStats.ofp_message::set_length(groupStats.get_minimum_length());
        groupStats.xid = temp->xid;
        groupStats.ofp_stats_msg::set_stats_type(OFPST_GROUP);
        groupStats.ofp_stats_msg::set_flags(stats.flags);
        groupStats.body[0].set_stats_length( groupStats.body[0].get_minimum_length() );
        groupStats.body[0].set_group_id(stats.group_id);
        groupStats.body[0].set_ref_count(stats.ref_count);
        groupStats.body[0].set_packet_count(stats.packet_count);
        groupStats.body[0].set_byte_count(stats.byte_count);
        groupStats.body[0].set_duration_sec(stats.duration_sec);
        groupStats.body[0].set_duration_nsec(stats.duration_nsec);
        for ( unsigned i =0; i < stats.num_bucket_stats; i++ ) {
            ofp_bucket_counter& bucket_stats = groupStats.body[0].bucket_stats[i];
            groupStats.set_length(groupStats.get_length() + bucket_stats.get_minimum_length());
            groupStats.body[0].set_stats_length( groupStats.body[0].get_stats_length()  +
                                                 bucket_stats.get_minimum_length());
            bucket_stats.set_packet_count(stats.bucket_stats[i].packet_count);
            bucket_stats.set_byte_count(stats.bucket_stats[i].byte_count);
        }
        return mVconn.Send((const ofp_message& )(groupStats));
    }
    uint16_t type=OFPET_BAD_REQUEST;
    uint16_t code=OFPBRC_BAD_STAT;
    std::ostream null_out(0);
    if ( TranslateOfDpReturnToOFVal(response.return_code, type, code) ) {
        return report_bad_message(*temp, type, code, null_out);
    }
    return true;
}

/**
 * @see ofp_hello
 * 
 * @author sraihan 
 * 
 * @return uint8_t 
 */
uint8_t OFV4Parser::get_hello_type() const
{
    return OFPT_HELLO;
}
/**
 *  
 * @author sraihan 
 * 
 * @return uint16_t 
 */
uint16_t OFV4Parser::get_hello_failed_type() const
{
    return OFPET_HELLO_FAILED;
}

/**
 *  
 * @author sraihan 
 * 
 * @return uint16_t 
 */
uint16_t OFV4Parser::get_hello_failed_code() const
{
    return OFPHFC_INCOMPATIBLE;
}

/**
 *  
 * @author sraihan 
 * 
 * @return uint8_t 
 */
uint8_t OFV4Parser::get_echo_type() const
{
    return OFPT_ECHO_REQUEST;
}

/**
 *  
 * @author sraihan 
 * 
 * @return uint8_t 
 */
uint8_t OFV4Parser::get_echo_reply_type() const
{
    return OFPT_ECHO_REPLY;
}

/**
 *  
 * @author sraihan 
 * 
 * @return uint8_t 
 */
uint8_t OFV4Parser::get_error_type() const
{
    // Top level msg type
    return OFPT_ERROR;
}

/**
 *  
 * @author sraihan 
 * 
 * @return uint16_t 
 */
uint16_t OFV4Parser::get_generic_error_sub_type() const
{
    return OFPET_BAD_REQUEST;
}

/**
 *  
 * @author sraihan 
 * 
 * @return uint16_t 
 */
uint16_t OFV4Parser::get_generic_error_sub_code() const
{
    return OFPBRC_BAD_TYPE;
}
