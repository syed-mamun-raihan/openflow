/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include "openflow_1.3.h"

using namespace OpenflowV4;

static_assert(sizeof(ofp_instruction_meter)== 8, "unexpected size");
static_assert(sizeof(ofp_switch_features)== 32, "unexpected size");
static_assert(sizeof(ofp_meter_band_header)== 12, "unexpected size");
static_assert(sizeof(ofp_meter_mod)== 16, "unexpected size");
static_assert(sizeof(ofp_meter_band_drop)== 16, "unexpected size");
static_assert(sizeof(ofp_meter_band_dscp_remark)== 16, "unexpected size");
static_assert(sizeof(ofp_meter_band_experimenter)== 16, "unexpected size");
static_assert(sizeof(ofp_table_stats)== 24+16, "unexpected size");
static_assert(sizeof(ofp_table_feature_prop_header)== 8, "unexpected size");
static_assert(sizeof(ofp_table_features_stats)== 64 + 16, "unexpected size");
static_assert(sizeof(ofp_table_feature_prop_instructions)== 4, "unexpected size");
static_assert(sizeof(ofp_table_feature_prop_next_tables)== 4, "unexpected size");
static_assert(sizeof(ofp_table_feature_prop_actions)== 4, "unexpected size");
static_assert(sizeof(ofp_table_feature_prop_oxm)== 4, "unexpected size");
static_assert(sizeof(ofp_prop_experimenter)== 12, "unexpected size");
static_assert(sizeof(ofp_port_stats)== 112+16, "unexpected size");
static_assert(sizeof(ofp_queue_stats)== 40+16, "unexpected size");
static_assert(sizeof(ofp_group_stats)== 40+16, "unexpected size");
static_assert(sizeof(ofp_meter_multipart_request)== 8+16, "unexpected size");
static_assert(sizeof(ofp_meter_band_stats)== 16, "unexpected size");
static_assert(sizeof(ofp_meter_stats)== 40+16, "unexpected size");
static_assert(sizeof(ofp_meter_config)== 8+16, "unexpected size");
static_assert(sizeof(ofp_meter_features)== 16+16, "unexpected size");
static_assert(sizeof(ofp_async_config)== 32, "unexpected size");
static_assert(sizeof(ofp_packet_in)== 32, "unexpected size");
static_assert(sizeof(ofp_packet_out)== 24, "unexpected size");
static_assert(sizeof(ofp_flow_mod)== 56, "unexpected size");
static_assert(sizeof(::ofp_message)== 8, "unexpected size");
static_assert(sizeof(ofp_switch_config)== 12, "unexpected size");
static_assert(sizeof(ofp_table_mod)== 16, "unexpected size");
static_assert(sizeof(ofp_phy_port)== 64, "unexpected size");
static_assert(sizeof(ofp_switch_features)== 32, "unexpected size");
static_assert(sizeof(ofp_port_status)== 80, "unexpected size");
static_assert(sizeof(ofp_port_mod)== 40, "unexpected size");
static_assert(sizeof(ofp_match)== 8, "unexpected size");
static_assert(sizeof(oxm_experimenter_fields)== 4, "unexpected size");
static_assert(sizeof(ofp_action_output)== 16, "unexpected size");
static_assert(sizeof(ofp_action_mpls_ttl)== 8, "unexpected size");
static_assert(sizeof(ofp_action_push)== 8, "unexpected size");
static_assert(sizeof(ofp_action_group)== 8, "unexpected size");
static_assert(sizeof(ofp_action_nw_ttl)== 8, "unexpected size");
static_assert(sizeof(ofp_action_set_field)== 8, "unexpected size");
static_assert(sizeof(ofp_action_experimenter_header)== 8, "unexpected size");
static_assert(sizeof(ofp_action_header)== 8, "unexpected size");
static_assert(sizeof(ofp_action_pop)== 8, "unexpected size");
static_assert(sizeof(ofp_action_push)== 8, "unexpected size");
static_assert(sizeof(ofp_instruction_goto_table)== 8, "unexpected size");
static_assert(sizeof(ofp_instruction_write_metadata)== 24, "unexpected size");
static_assert(sizeof(ofp_instruction_actions)== 8, "unexpected size");
static_assert(sizeof(ofp_instruction_experimenter)== 8, "unexpected size");
static_assert(sizeof(ofp_bucket)== 16, "unexpected size");
static_assert(sizeof(ofp_group_mod)== 16, "unexpected size");
static_assert(sizeof(ofp_flow_removed)== 56, "unexpected size");
static_assert(sizeof(::ofp_error_msg)== 12, "unexpected size");
static_assert(sizeof(ofp_error_experimenter_msg)== 16, "unexpected size");
static_assert(sizeof(ofp_stats_msg)== 16, "unexpected size");
static_assert(sizeof(ofp_desc_stats)== 1056+16, "unexpected size");
static_assert(sizeof(ofp_flow_stats_request)== 40+16, "unexpected size");
static_assert(sizeof(ofp_flow_stats)== 56+16, "unexpected size");
static_assert(sizeof(ofp_aggregate_stats_request)== 40+16, "unexpected size");
static_assert(sizeof(ofp_aggregate_stats)== 24+16, "unexpected size");
static_assert(sizeof(ofp_port_stats_request)== 8+16, "unexpected size");
static_assert(sizeof(ofp_group_stats_request)== 8+16, "unexpected size");
static_assert(sizeof(ofp_bucket_counter)== 16, "unexpected size");
static_assert(sizeof(ofp_group_stats)== 40+16, "unexpected size");
static_assert(sizeof(ofp_group_desc_stats)== 8+16, "unexpected size");
static_assert(sizeof(ofp_group_features_stats)== 40+16, "unexpected size");
static_assert(sizeof(ofp_experimenter_stats_header)== 8+16, "unexpected size");
static_assert(sizeof(ofp_experimenter_header)== 16, "unexpected size");
static_assert(sizeof(ofp_queue_prop_header)== 8, "unexpected size");
static_assert(sizeof(ofp_queue_prop_rate)== 16, "unexpected size");
static_assert(sizeof(ofp_queue_prop_experimenter)== 16, "unexpected size");
static_assert(sizeof(ofp_packet_queue)== 16, "unexpected size");
static_assert(sizeof(ofp_queue_get_config_request)== 16, "unexpected size");
static_assert(sizeof(ofp_queue_get_config_reply)== 16, "unexpected size");
static_assert(sizeof(ofp_action_set_queue)== 8, "unexpected size");
static_assert(sizeof(ofp_queue_stats_request)== 8+16, "unexpected size");
static_assert(sizeof(ofp_queue_stats)== 40+16, "unexpected size");
static_assert(sizeof(ofp_role_request)== 24, "unexpected size");
static_assert(sizeof(oxm_fields_t)==4, " size mismatch");
static_assert(sizeof(oxm_odu_sigtype)== 5, " size mismatch ");
static_assert(sizeof(oxm_odu_sigid)== 8, " size mismatch ");
static_assert(sizeof(oxm_och_sigid)== 10, " size mismatch ");
static_assert(sizeof(optical_port_layer_stack_entry)==8,"size mismatch");
static_assert(sizeof(optical_port_layer_stack)==4,"size mismatch");
static_assert(sizeof(optical_port_layer_stack_entry)==8,"size mismatch");

/**
 *  
 * @author sraihan 
 * 
 * @param out 
 * @param msg 
 * 
 * @return std::ostream& 
 */
std::ostream& operator<<(std::ostream &out, const ofp_role_request &msg) {
    out << "\n\tVersion: " << (unsigned)msg.version << " Role:  " << msg.get_role() << " Generation_Id: " << msg.get_generation_id() << std::endl;
    return out;
}

/**
 *  
 * @author sraihan 
 * 
 * @param out 
 * @param obj 
 * 
 * @return std::ostream& 
 */
std::ostream& operator<<(std::ostream &out, const ofp_desc_stats &obj) {
    out << "\ndesc stats : "
        << "\n\t mfr desc: " << obj.mfr_desc
        << "\n\t hw desc: " << obj.hw_desc
        << "\n\t sw desc: " << obj.sw_desc
        << "\n\t serial num: " << obj.serial_num
        << "\n\t dp desc: " << obj.dp_desc << " ";
    return out;
}

/**
 *  
 * @author sraihan 
 * 
 * @param out 
 * @param obj 
 * 
 * @return std::ostream& 
 */
std::ostream& operator<<(std::ostream &out, const ofp_flow_stats &obj) {
    out << "\nflow stats : "
        << "\n flow_stats_length: " << obj.get_stats_length()
        << "\n table_id: " << (unsigned)(obj.table_id)
        << "\n duration_sec: " << obj.get_duration_sec()
        << "\n duration_nsec: " << obj.get_duration_nsec()
        << "\n priority: " << obj.get_priority()
        << "\n idle_timeout: " << obj.get_idle_timeout()
        << "\n hard_timeout: " << obj.get_hard_timeout()
        << "\n cookie: " << obj.get_cookie()
        << "\n packet_count: " << obj.get_packet_count()
        << "\n byte_count: " << obj.get_byte_count()
        << "\n ";
    return out;
}

/**
 *  
 * @author sraihan 
 * 
 * @param out 
 * @param obj 
 * 
 * @return std::ostream& 
 */
std::ostream& operator<<(std::ostream &out, const ofp_group_desc_stats &obj) {
    out << "\ngroup desc stats : "
        << "\n\t group_stats_length: " << obj.body[0].get_length()
        << "\n\t group_type: " << (unsigned)(obj.body[0].group_type)
        << "\n\t group_id: " << obj.body[0].get_group_id()
        << "\n ";
    return out;
}

/**
 * 
 * 
 * @author sraihan 
 * 
 * @param out 
 * @param obj 
 * 
 * @return std::ostream& 
 */
std::ostream& operator<<(std::ostream &out, const ofp_group_features_stats &obj) {
    out << "\ngroup feature stats : "
        << "\n\t types: " << obj.get_types()
        << "\n\t capabilities: " << obj.get_capabilities()
        << "\n\t max_groups: " << ntohl(obj.max_groups[0])
        << " " << ntohl(obj.max_groups[1])
        << " " << ntohl(obj.max_groups[2])
        << " " << ntohl(obj.max_groups[3])
        << "\n\t actions: " << ntohl(obj.actions[0])
        << " " <<  ntohl(obj.actions[1])
        << " " <<  ntohl(obj.actions[2])
        << " " <<  ntohl(obj.actions[3])
        << "\n ";
    return out;
}

/**
 * 
 * 
 * @author sraihan 
 * 
 * @param out 
 * @param obj 
 * 
 * @return std::ostream& 
 */
std::ostream& operator<<(std::ostream &out, const OpenflowV4::ofp_group_stats &obj) {
    out << "\ngroup stats : "
        << "\n\t group_stats_length: " << obj.body[0].get_stats_length()
        << "\n\t group_id: " << obj.body[0].get_group_id()
        << "\n\t ref_count: " << obj.body[0].get_ref_count()
        << "\n\t packet_count: " << obj.body[0].get_packet_count()
        << "\n\t byte_count: " << obj.body[0].get_byte_count()
        << "\n\t duration_sec: " << obj.body[0].get_duration_sec()
        << "\n\t duration_nsec: " << obj.body[0].get_duration_nsec()
        << "\n ";
    return out;
}

/**
 * Implements utility functions to encode / decode openflow 
 * messages 
 * 
 * @author sraihan 
 */
namespace OpenflowV4 {
/**
 * 
 * 
 * @author sraihan 
 * 
 * @param instructions 
 * @param instructions_type 
 * @param cur 
 * @param reply 
 * 
 * @return bool 
 */
bool EncodeTableFeatureInstructions(const uint32_t instructions, const uint16_t instructions_type, ofp_tlv_base *cur, OpenflowV4::ofp_table_features_stats &reply) {
    // count instructions
    if (instructions &
            ((1 << OFDP_APPLY_INSTRUCTION) |
                 (1 << OFDP_GOTO_TABLE_INSTRUCTION) |
                 (1 << OFDP_WRITE_METADATA_INSTRUCTION) |
                 (1 << OFDP_WRITE_INSTRUCTION) |
                 (1 << OFDP_CLEAR_INSTRUCTION) |
                 (1 << OFDP_METER_INSTRUCTION))) {
        unsigned property_len = sizeof(ofp_table_feature_prop_instructions);
        const unsigned apply_len = (instructions & (1 << OFDP_APPLY_INSTRUCTION)) ? sizeof(ofp_tlv_base) : 0;
        const unsigned write_act_len = (instructions & (1 << OFDP_WRITE_INSTRUCTION)) ? sizeof(ofp_tlv_base) : 0;
        const unsigned goto_act_len = (instructions & (1 << OFDP_GOTO_TABLE_INSTRUCTION)) ? sizeof(ofp_tlv_base) : 0;
        const unsigned write_meta_len = (instructions & (1 << OFDP_WRITE_METADATA_INSTRUCTION)) ? sizeof(ofp_tlv_base) : 0;
        const unsigned clear_len = (instructions & (1 << OFDP_CLEAR_INSTRUCTION)) ? sizeof(ofp_tlv_base) : 0;
        const unsigned meter_len = (instructions & (1 << OFDP_METER_INSTRUCTION)) ? sizeof(ofp_tlv_base) : 0;
        property_len +=  (apply_len + write_act_len + goto_act_len + write_meta_len + clear_len + meter_len);
        const unsigned property_len_padded = ofp_tlv_base::get_size_64bit_aligned(property_len);
        const unsigned new_msg_len = reply.get_length() + property_len_padded;
        reply.set_length(new_msg_len);
        reply.body[0].set_stat_length(reply.body[0].get_stat_length() + property_len_padded);
        cur->set_type(instructions_type);
        cur->set_len(property_len);
        cur = static_cast<ofp_table_feature_prop_instructions *>(cur)->instruction_ids;
        if (apply_len) {
            cur->set_type(OFPIT_APPLY_ACTIONS);
            cur->set_len(apply_len);
            cur = cur->get_next();
        }
        if (write_act_len) {
            cur->set_type(OFPIT_WRITE_ACTIONS);
            cur->set_len(write_act_len);
            cur = cur->get_next();
        }
        if (goto_act_len) {
            cur->set_type(OFPIT_GOTO_TABLE);
            cur->set_len(goto_act_len);
            cur = cur->get_next();
        }
        if (write_meta_len) {
            cur->set_type(OFPIT_WRITE_METADATA);
            cur->set_len(write_meta_len);
            cur = cur->get_next();
        }
        if (clear_len) {
            cur->set_type(OFPIT_CLEAR_ACTIONS);
            cur->set_len(clear_len);
            cur = cur->get_next();
        }
        if (meter_len) {
            cur->set_type(OFPIT_METER);
            cur->set_len(meter_len);
            cur = cur->get_next();
        }
    } else {
        const unsigned property_len = ofp_tlv_base::get_size_64bit_aligned(sizeof(ofp_table_feature_prop_instructions));
        const unsigned new_msg_len = reply.get_length() + property_len;
        reply.set_length(new_msg_len);
        reply.body[0].set_stat_length(reply.body[0].get_stat_length() + property_len);
        cur->set_type(instructions_type);
        cur->set_len(4);
    }
    return true;
}

/**
 * 
 * 
 * @author sraihan 
 * 
 * @param actions 
 * @param actions_type 
 * @param cur 
 * @param reply 
 * 
 * @return bool 
 */
bool EncodeTableFeatureActions(const uint32_t actions, const uint16_t actions_type, ofp_tlv_base *cur, OpenflowV4::ofp_table_features_stats &reply) {
    if (actions & ((1 << OFDP_OUTPUT) |
                       (1 << OFDP_POP_VLAN) |
                       (1 << OFDP_PUSH_VLAN_VID) |
                       (1 << OFDP_POP_MPLS) |
                       (1 << OFDP_PUSH_MPLS) |
                       (1 << OFDP_DEC_MPLS_TTL) |
                       (1 << OFDP_SET_MPLS_TTL) |
                       (1 << OFDP_GROUP) |
                       (1 << OFDP_SET_VLAN_VID) |
                       (1 << OFDP_SET_VLAN_PCP) |
                       (1 << OFDP_SET_DL_SRC) |
                       (1 << OFDP_SET_DL_DST) |
                       (1 << OFDP_SET_NW_SRC) |
                       (1 << OFDP_SET_NW_DST) |
                       (1 << OFDP_SET_TP_SRC) |
                       (1 << OFDP_SET_TP_DST) |
                       (1 << OFDP_SET_MPLS_LABEL))) {
        unsigned property_len = sizeof(ofp_table_feature_prop_actions);
        const unsigned output_len = (actions & (1 << OFDP_OUTPUT)) ? sizeof(ofp_tlv_base) : 0;
        const unsigned pop_vlan_len = (actions & (1 << OFDP_POP_VLAN)) ? sizeof(ofp_tlv_base) : 0;
        const unsigned push_vlan_len = (actions & (1 << OFDP_PUSH_VLAN_VID)) ? sizeof(ofp_tlv_base) : 0;
        const unsigned pop_mpls_len = (actions & (1 << OFDP_POP_MPLS)) ? sizeof(ofp_tlv_base) : 0;
        const unsigned push_mpls_len = (actions & (1 << OFDP_PUSH_MPLS)) ? sizeof(ofp_tlv_base) : 0; // same as instruction header
        const unsigned dec_mpls_len = (actions & (1 << OFDP_DEC_MPLS_TTL)) ? sizeof(ofp_tlv_base) : 0;
        const unsigned set_mpls_len = (actions & (1 << OFDP_SET_MPLS_TTL)) ? sizeof(ofp_tlv_base) : 0;
        const unsigned group_len = (actions & (1 << OFDP_GROUP)) ? sizeof(ofp_tlv_base) : 0;
        const unsigned vlan_len = (actions & (1 << OFDP_SET_VLAN_VID)) ? sizeof(oxm_fields_t) : 0;
        const unsigned pcp_len = (actions & (1 << OFDP_SET_VLAN_PCP)) ? sizeof(oxm_fields_t) : 0;
        const unsigned dl_src_len = (actions & (1 << OFDP_SET_DL_SRC)) ? sizeof(oxm_fields_t) : 0;
        const unsigned dl_dst_len = (actions & (1 << OFDP_SET_DL_DST)) ? sizeof(oxm_fields_t) : 0;
        const unsigned ip_src = (actions & (1 << OFDP_SET_NW_SRC)) ? sizeof(oxm_fields_t) : 0;
        const unsigned ip_dst = (actions & (1 << OFDP_SET_NW_DST)) ? sizeof(oxm_fields_t) : 0;
        const unsigned port_src = (actions & (1 << OFDP_SET_TP_SRC)) ? 2 * sizeof(oxm_fields_t) : 0;
        const unsigned port_dst = (actions & (1 << OFDP_SET_TP_DST)) ? 2 * sizeof(oxm_fields_t) : 0;
        property_len += (vlan_len + pcp_len + dl_src_len + dl_dst_len + ip_src +  ip_dst + port_src + port_dst);
        property_len +=  (output_len + pop_vlan_len + push_vlan_len + pop_mpls_len + push_mpls_len + dec_mpls_len + set_mpls_len + group_len);
        const unsigned property_len_padded = ofp_tlv_base::get_size_64bit_aligned(property_len);
        const unsigned new_msg_len = reply.get_length() + property_len_padded;
        reply.set_length(new_msg_len);
        reply.body[0].set_stat_length(reply.body[0].get_stat_length() + property_len_padded);
        cur->set_type(actions_type);
        cur->set_len(property_len);
        cur = static_cast<ofp_table_feature_prop_actions *>(cur)->action_ids;
        if (output_len) {
            cur->set_type(OFPAT_OUTPUT);
            cur->set_len(output_len);
            cur = cur->get_next();
        }
        if (pop_vlan_len) {
            cur->set_type(OFPAT_POP_VLAN);
            cur->set_len(pop_vlan_len);
            cur = cur->get_next();
        }
        if (push_vlan_len) {
            cur->set_type(OFPAT_PUSH_VLAN);
            cur->set_len(push_vlan_len);
            cur = cur->get_next();
        }
        if (pop_mpls_len) {
            cur->set_type(OFPAT_POP_MPLS);
            cur->set_len(pop_mpls_len);
            cur = cur->get_next();
        }
        if (push_mpls_len) {
            cur->set_type(OFPAT_PUSH_MPLS);
            cur->set_len(push_mpls_len);
            cur = cur->get_next();
        }
        if (dec_mpls_len) {
            cur->set_type(OFPAT_DEC_MPLS_TTL);
            cur->set_len(dec_mpls_len);
            cur = cur->get_next();
        }
        if (group_len) {
            cur->set_type(OFPAT_GROUP);
            cur->set_len(group_len);
            cur = cur->get_next();
        }
        oxm_fields_t *oxm = reinterpret_cast<oxm_fields_t *>(cur);
        if (vlan_len) {
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_VLAN_VID);
            oxm = oxm->get_next();
        }
        if (pcp_len) {
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_VLAN_PCP);
            oxm = oxm->get_next();
        }
        if (dl_src_len) {
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ETH_SRC);
            oxm = oxm->get_next();
        }
        if (set_mpls_len) {
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_MPLS_LABEL);
            oxm = oxm->get_next();
        }
        if (dl_dst_len) {
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ETH_DST);
            oxm = oxm->get_next();
        }
        if (ip_src) {
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IPV4_SRC);
            oxm = oxm->get_next();
        }
        if (ip_dst) {
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IPV4_DST);
            oxm = oxm->get_next();
        }
        if (port_src) {
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_TCP_SRC);
            oxm = oxm->get_next();
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_UDP_SRC);
            oxm = oxm->get_next();
        }
        if (port_dst) {
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_TCP_DST);
            oxm = oxm->get_next();
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_UDP_DST);
            oxm = oxm->get_next();
        }

    } else {
        const unsigned property_len = ofp_tlv_base::get_size_64bit_aligned(sizeof(ofp_table_feature_prop_instructions));
        const unsigned new_msg_len = reply.get_length() + property_len;
        reply.set_length(new_msg_len);
        reply.body[0].set_stat_length(reply.body[0].get_stat_length() + property_len);
        cur->set_type(actions_type);
        cur->set_len(4);
    }
    return true;
}

/**
 *  
 * @author sraihan 
 * 
 * @param oxm_ids 
 * @param oxm_ids_type 
 * @param cur 
 * @param reply 
 * 
 * @return bool 
 */
bool EncodeTableFeatureOXM(const uint32_t oxm_ids, const uint16_t oxm_ids_type, ofp_tlv_base *cur, OpenflowV4::ofp_table_features_stats &reply) {
    if (oxm_ids & ((OFDP_DL_VLAN) |
                       (OFDP_DL_SRC) |
                       (OFDP_DL_DST) |
                       (OFDP_DL_TYPE) |
                       (OFDP_NW_PROTO) |
                       (OFDP_NW_TOS) |
                       (OFDP_DL_VLAN_PCP) |
                       (OFDP_MPLS_L1) |
                       (OFDP_IN_PORT) |
                       (OFDP_TP_SRC | OFDP_TP_DST))) {
        unsigned property_len = sizeof(ofp_table_feature_prop_oxm);
        const unsigned nw_proto_len = (oxm_ids & (OFDP_NW_PROTO)) ? sizeof(oxm_fields_t) : 0;
        const unsigned nw_tos_len = (oxm_ids & (OFDP_NW_TOS)) ? sizeof(oxm_fields_t) : 0;
        const unsigned vlan_len = (oxm_ids & (OFDP_DL_VLAN)) ? sizeof(oxm_fields_t) : 0;
        const unsigned pcp_len = (oxm_ids & (OFDP_DL_VLAN_PCP)) ? sizeof(oxm_fields_t) : 0;
        const unsigned dl_src_len = (oxm_ids & (OFDP_DL_SRC)) ? sizeof(oxm_fields_t) : 0;
        const unsigned dl_dst_len = (oxm_ids & (OFDP_DL_DST)) ? sizeof(oxm_fields_t) : 0;
        const unsigned label_len = (oxm_ids & (OFDP_MPLS_L1)) ? sizeof(oxm_fields_t) : 0;
        const unsigned eth_type_len = (oxm_ids & (OFDP_DL_TYPE)) ? sizeof(oxm_fields_t) : 0;
        const unsigned inport_len = (oxm_ids & (OFDP_IN_PORT)) ? sizeof(oxm_fields_t) : 0;
        const unsigned port_src_len = (oxm_ids & (OFDP_TP_SRC)) ? 3 * sizeof(oxm_fields_t) : 0;
        const unsigned port_dst_len = (oxm_ids & (OFDP_TP_DST)) ? 3 * sizeof(oxm_fields_t) : 0;
        property_len += (vlan_len + pcp_len + dl_src_len + dl_dst_len + label_len + eth_type_len + inport_len + port_src_len + port_dst_len + nw_proto_len + nw_tos_len);
        const unsigned property_len_padded = ofp_tlv_base::get_size_64bit_aligned(property_len);
        const unsigned new_msg_len = reply.get_length() + property_len_padded;
        reply.set_length(new_msg_len);
        reply.body[0].set_stat_length(reply.body[0].get_stat_length() + property_len_padded);
        cur->set_type(oxm_ids_type);
        cur->set_len(property_len);
        oxm_fields_t *oxm = static_cast<ofp_table_feature_prop_oxm *>(cur)->oxm_ids;
        if (vlan_len) {
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_VLAN_VID);
            oxm = oxm->get_next();
        }
        if (pcp_len) {
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_VLAN_PCP);
            oxm = oxm->get_next();
        }
        if (dl_src_len) {
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ETH_SRC);
            oxm = oxm->get_next();
        }
        if (label_len) {
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_MPLS_LABEL);
            oxm = oxm->get_next();
        }
        if (dl_dst_len) {
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ETH_DST);
            oxm = oxm->get_next();
        }
        if (eth_type_len) {
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ETH_TYPE);
            oxm = oxm->get_next();
        }
        if (inport_len) {
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IN_PORT);
            oxm = oxm->get_next();
        }
        if (nw_proto_len) {
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IP_PROTO);
            oxm = oxm->get_next();
        }
        if (nw_tos_len) {
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IP_DSCP);
            oxm = oxm->get_next();
        }
        if (port_src_len) {
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IPV4_SRC);
            oxm = oxm->get_next();
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_TCP_SRC);
            oxm = oxm->get_next();
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_UDP_SRC);
            oxm = oxm->get_next();
        }
        if (port_dst_len) {
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IPV4_DST);
            oxm = oxm->get_next();
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_TCP_DST);
            oxm = oxm->get_next();
            oxm->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_UDP_DST);
            oxm = oxm->get_next();
        }
    } else {
        const unsigned property_len = ofp_tlv_base::get_size_64bit_aligned(sizeof(ofp_table_feature_prop_oxm));
        const unsigned new_msg_len = reply.get_length() + property_len;
        reply.set_length(new_msg_len);
        reply.body[0].set_stat_length(reply.body[0].get_stat_length() + property_len);
        cur->set_type(oxm_ids_type);
        cur->set_len(4);

    }
    return true; // success
}

/**
 * Encodes match field description to
 * OXM_MATCH descriptions
 * @author sraihan 
 * 
 * @param struct_with_ofp_match 
 * @param msg 
 * @param matchFrom 
 * 
 * @return bool 
 */
template<typename struct_with_ofp_match>
bool EncodeOXMFields(ofp_message &msg, const OfDpMatchT &matchFrom) {
    std::array<uint8_t, OFDP_MAC_LEN> null_mac; null_mac.fill(0);
    const size_t in_port_oxm_len = (matchFrom.in_port) ? sizeof(oxm_fields_t) + sizeof(uint32_t) : 0;
    const size_t dl_dst_oxm_len = (null_mac == matchFrom.dl_dst) ? 0 : sizeof(oxm_fields_t) + OFDP_MAC_LEN;
    const size_t dl_src_oxm_len = (null_mac == matchFrom.dl_src) ? 0 : sizeof(oxm_fields_t) + OFDP_MAC_LEN;
    const size_t dl_type_oxm_len = (matchFrom.dl_type) ? sizeof(oxm_fields_t) + sizeof(uint16_t) : 0;
    const size_t vlan_oxm_len =   (matchFrom.dl_vlan) ? sizeof(oxm_fields_t) + sizeof(uint16_t) : 0;
    const size_t vlan_pcp_oxm_len = (matchFrom.dl_vlan_pcp && vlan_oxm_len) ? sizeof(oxm_fields_t) + sizeof(uint8_t) : 0;
    const size_t ip_proto_oxm_len = (matchFrom.nw_proto && matchFrom.nw_proto != 0xFF) ? sizeof(oxm_fields_t) + sizeof(uint8_t) : 0;
    const size_t ipv4_src_oxm_len = (matchFrom.nw_src) ? sizeof(oxm_fields_t) + 2 * sizeof(uint32_t) : 0;
    const size_t ipv4_dst_oxm_len = (matchFrom.nw_dst) ? sizeof(oxm_fields_t) + 2 * sizeof(uint32_t) : 0;
    const size_t tcp_src_oxm_len = (matchFrom.tp_src && matchFrom.nw_proto == 6) ? sizeof(oxm_fields_t) + sizeof(uint16_t) : 0;
    const size_t tcp_dst_oxm_len = (matchFrom.tp_dst && matchFrom.nw_proto == 6) ? sizeof(oxm_fields_t) + sizeof(uint16_t) : 0;
    const size_t udp_src_oxm_len = (matchFrom.tp_src && matchFrom.nw_proto == 17) ? sizeof(oxm_fields_t) + sizeof(uint16_t) : 0;
    const size_t udp_dst_oxm_len = (matchFrom.tp_dst && matchFrom.nw_proto == 17) ? sizeof(oxm_fields_t) + sizeof(uint16_t) : 0;
    const size_t nw_tos_oxm_len = (matchFrom.nw_tos) ? sizeof(oxm_fields_t) + sizeof(uint8_t) : 0;
    const size_t mpls_label1_oxm_len = (matchFrom.mpls_label1 && matchFrom.mpls_label1 != 0xFFFFFFFF) ? sizeof(oxm_fields_t) + sizeof(uint32_t) : 0;
    const size_t odu_sig_type_oxm_len = (matchFrom.signal_type) ? sizeof(oxm_fields_t) + sizeof(oxm_odu_sigtype) : 0;
    const size_t odu_sig_id_oxm_len = (matchFrom.tpn) ? sizeof(oxm_fields_t) + sizeof(oxm_odu_sigid) + ((matchFrom.tslen <=8)?1: matchFrom.tslen/TS_PER_BYTE): 0;

    const size_t match_len_no_padding = sizeof(ofp_tlv_base)
        + in_port_oxm_len
        + dl_dst_oxm_len
        + dl_src_oxm_len
        + dl_type_oxm_len
        + vlan_oxm_len
        + vlan_pcp_oxm_len
        + ip_proto_oxm_len
        + ipv4_src_oxm_len
        + ipv4_dst_oxm_len
        + tcp_src_oxm_len
        + tcp_dst_oxm_len
        + udp_src_oxm_len
        + udp_dst_oxm_len
        + nw_tos_oxm_len
        + mpls_label1_oxm_len
        + odu_sig_type_oxm_len
        + odu_sig_id_oxm_len;

    const size_t match_len_with_padding = ::ofp_tlv_base::get_size_64bit_aligned(match_len_no_padding);
    msg.set_length(msg.get_length() - sizeof(ofp_match) + match_len_with_padding);
    if ((msg.get_length() % 8) != 0) {
        std::cout << __PRETTY_FUNCTION__ << " Error: length is not 8 byte aligned\n";
        assert((msg.get_length() % 8) == 0);
        return false;
    }
    // ofp_match header
    ofp_match &match = static_cast<struct_with_ofp_match&>(msg).match;
    match.set_len(match_len_no_padding);
    match.set_type(OFPMT_OXM);
    // oxm_field header
    oxm_fields_t *cur_ptr = match.oxm_fields;
    if (in_port_oxm_len) {
        cur_ptr->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IN_PORT, sizeof(uint32_t));
        cur_ptr->set_value<uint32_t>(matchFrom.in_port);
        cur_ptr = const_cast<oxm_fields_t *>(cur_ptr->get_next());
    }
    if (dl_dst_oxm_len) {
        cur_ptr->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ETH_DST, OFDP_MAC_LEN);
        uint8_t *data;
        cur_ptr->get_value_as_uint8ptr(data);
        memcpy(data, matchFrom.dl_dst.data(), OFDP_MAC_LEN);
        cur_ptr = const_cast<oxm_fields_t *>(cur_ptr->get_next());
    }
    if (dl_src_oxm_len) {
        cur_ptr->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ETH_SRC, OFDP_MAC_LEN);
        uint8_t *data;
        cur_ptr->get_value_as_uint8ptr(data);
        memcpy(data, matchFrom.dl_src.data(), OFDP_MAC_LEN);
        cur_ptr = const_cast<oxm_fields_t *>(cur_ptr->get_next());
    }
    if (dl_type_oxm_len) {
        cur_ptr->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ETH_TYPE, sizeof(uint16_t));
        cur_ptr->set_value<uint16_t>(matchFrom.dl_type);
        cur_ptr = const_cast<oxm_fields_t *>(cur_ptr->get_next());
    }
    if (vlan_oxm_len) { // Not wildcard
        cur_ptr->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_VLAN_VID, sizeof(uint16_t));
        uint16_t value = FROM_OFDP_VLAN_NONE_TO_0(matchFrom.dl_vlan);
        if (matchFrom.dl_vlan & OFPVID_PRESENT) { // VID PRESENT
            value |= OFPVID_PRESENT;
        }
        cur_ptr->set_value<uint16_t>(value);
        cur_ptr = const_cast<oxm_fields_t *>(cur_ptr->get_next());
    }
    if (vlan_pcp_oxm_len) {
        cur_ptr->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_VLAN_PCP, sizeof(uint8_t));
        cur_ptr->set_value<uint8_t>(matchFrom.dl_vlan_pcp);
        cur_ptr = const_cast<oxm_fields_t *>(cur_ptr->get_next());
    }
    if (ip_proto_oxm_len) {
        cur_ptr->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IP_PROTO, sizeof(uint8_t));
        cur_ptr->set_value<uint8_t>(matchFrom.nw_proto);
        cur_ptr = const_cast<oxm_fields_t *>(cur_ptr->get_next());
    }
    if (ipv4_src_oxm_len) {
        cur_ptr->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IPV4_SRC, sizeof(uint32_t));
        cur_ptr->set_value<uint32_t>(matchFrom.nw_src);
        cur_ptr = const_cast<oxm_fields_t *>(cur_ptr->get_next());
    }
    if (ipv4_dst_oxm_len) {
        cur_ptr->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IPV4_DST, sizeof(uint32_t));
        cur_ptr->set_value<uint32_t>(matchFrom.nw_dst);
        cur_ptr = const_cast<oxm_fields_t *>(cur_ptr->get_next());
    }
    if (tcp_src_oxm_len) {
        cur_ptr->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_TCP_SRC, sizeof(uint16_t));
        cur_ptr->set_value<uint16_t>(matchFrom.tp_src);
        cur_ptr = const_cast<oxm_fields_t *>(cur_ptr->get_next());
    }
    if (tcp_dst_oxm_len) {
        cur_ptr->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_TCP_DST, sizeof(uint16_t));
        cur_ptr->set_value<uint16_t>(matchFrom.tp_dst);
        cur_ptr = const_cast<oxm_fields_t *>(cur_ptr->get_next());
    }
    if (udp_src_oxm_len) {
        cur_ptr->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_UDP_SRC, sizeof(uint16_t));
        cur_ptr->set_value<uint16_t>(matchFrom.tp_src);
        cur_ptr = const_cast<oxm_fields_t *>(cur_ptr->get_next());
    }
    if (udp_dst_oxm_len) {
        cur_ptr->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_UDP_DST, sizeof(uint16_t));
        cur_ptr->set_value<uint16_t>(matchFrom.tp_dst);
        cur_ptr = const_cast<oxm_fields_t *>(cur_ptr->get_next());
    }
    if (nw_tos_oxm_len) {
        cur_ptr->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IP_DSCP, sizeof(uint8_t));
        cur_ptr->set_value<uint8_t>(matchFrom.nw_tos);
        cur_ptr = const_cast<oxm_fields_t *>(cur_ptr->get_next());
    }
    if (mpls_label1_oxm_len) {
        cur_ptr->setHeader(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_MPLS_LABEL, sizeof(uint32_t));
        cur_ptr->set_value<uint32_t>(matchFrom.mpls_label1);
        cur_ptr = const_cast<oxm_fields_t *>(cur_ptr->get_next());
    }
///--- OTWG Extensions ----
    if (odu_sig_type_oxm_len) {
        oxm_odu_sigtype *value = reinterpret_cast<oxm_odu_sigtype *>(cur_ptr->data_);
        cur_ptr->setHeader(OFPXMC_EXPERIMENTER, OFPXMT_EXP_ODU_SIGTYPE, value->get_minimum_length());
        value->set_experimenter(OFP_EXPERIMENTER_OUI);
        value->sigtype = matchFrom.signal_type;
        cur_ptr = const_cast<oxm_fields_t *>(cur_ptr->get_next_exp());
    }
    if (odu_sig_id_oxm_len) {
        oxm_odu_sigid *value = reinterpret_cast<oxm_odu_sigid *>(cur_ptr->data_);
        const unsigned no_of_bytes =  ((matchFrom.tslen <=8)?1: matchFrom.tslen/TS_PER_BYTE);
        cur_ptr->setHeader(OFPXMC_EXPERIMENTER, OFPXMT_EXP_ODU_SIGID, value->get_minimum_length()+no_of_bytes);
        value->set_experimenter(OFP_EXPERIMENTER_OUI);
        value->set_tpn(matchFrom.tpn);
        value->set_tslen(matchFrom.tslen);
        memcpy(value->tsmap, matchFrom.timeslots.data(), no_of_bytes);
        cur_ptr = const_cast<oxm_fields_t *>(cur_ptr->get_next_exp());
    }

    return true;
}

template bool
    EncodeOXMFields<OpenflowV4::ofp_flow_removed>(ofp_message &msg, const OfDpMatchT &matchFrom);

template bool
    EncodeOXMFields<OpenflowV4::ofp_packet_in>(ofp_message &msg, const OfDpMatchT &matchFrom);

template bool
    EncodeOXMFields<OpenflowV4::ofp_flow_stats>(ofp_message &msg, const OfDpMatchT &matchFrom);

template bool
EncodeOXMFields<OpenflowV4::ofp_flow_mod>(ofp_message& msg, const OfDpMatchT& matchFrom);

/**
 * 
 * 
 * @author sraihan 
 * 
 * @param msg 
 * @param actions 
 * @param num_actions 
 * @param bWithInstructionHeader 
 * @param action_instruction_type 
 * 
 * @return bool 
 */
bool EncodeActionItem(ofp_message &msg,
                      const OfDpActionT *actions,
                      const uint16_t num_actions,
                      const bool bWithInstructionHeader,
                      const int action_instruction_type) {
    ofp_tlv_base *action_head = 0;
    ofp_tlv_base *instruction_head = 0;
    if (!(msg.get_length() >= msg.get_minimum_length())) {
        std::cout << "Given OFP mesage error: minimum length not met\n";
        assert(msg.get_length() >= msg.get_minimum_length());
        return false;
    }
    if (bWithInstructionHeader) {
        instruction_head =  reinterpret_cast<ofp_tlv_base *>(reinterpret_cast<uint8_t *>(&msg) + msg.get_length());
        msg.set_length(msg.get_length() + sizeof(ofp_instruction_actions));
        static_cast<ofp_instruction_actions *>(instruction_head)->set_type(action_instruction_type);
        static_cast<ofp_instruction_actions *>(instruction_head)->set_len(sizeof(ofp_instruction_actions)); // set initial length
        action_head = static_cast<ofp_instruction_actions *>(instruction_head)->actions;
    } else {
        action_head = reinterpret_cast<ofp_tlv_base *>(reinterpret_cast<uint8_t *>(&msg) + msg.get_length());
    }
    for (int i = 0; i < num_actions; i++) {
        switch (actions[i].command) {
        case OFDP_OUTPUT:
            {
                ofp_action_output &action =  *((ofp_action_output *)action_head);
                action.set_type(OFPAT_OUTPUT);
                action.set_len(sizeof(ofp_action_output));
                action.set_max_len(actions[i].value.output.max_len);
                action.set_port(actions[i].value.output.port);
                break;
            }
        case OFDP_GROUP:
            {
                ofp_action_group &action =  *((ofp_action_group *)action_head);
                action.set_type(OFPAT_GROUP);
                action.set_len(sizeof(ofp_action_group));
                action.set_group_id(actions[i].value.group_id);
                break;
            }
        case OFDP_SET_VLAN_VID:
            {
                ofp_action_set_field &action =  *((ofp_action_set_field *)action_head);
                action.set_type(OFPAT_SET_FIELD);
                action.set_len(ofp_action_set_field::get_size<uint16_t>());
                const uint16_t value = FROM_OFDP_VLAN_NONE_TO_0(actions[i].value.vlan.vid) | OFPVID_PRESENT;
                action.set_data<uint16_t>(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_VLAN_VID, value);
                break;
            }
        case OFDP_SET_VLAN_PCP:
            {
                ofp_action_set_field &action =  *((ofp_action_set_field *)action_head);
                action.set_type(OFPAT_SET_FIELD);
                action.set_len(ofp_action_set_field::get_size<uint8_t>());
                action.set_data<uint8_t>(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_VLAN_PCP, actions[i].value.vlan.pcp);
                break;
            }
        case OFDP_SET_DL_SRC:
            {
                ofp_action_set_field &action =  *((ofp_action_set_field *)action_head);
                action.set_type(OFPAT_SET_FIELD);
                action.set_len(ofp_action_set_field::get_size<dl_mac_type>());
                action.set_byte_array(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ETH_SRC, OFDP_MAC_LEN, actions[i].value.dl_addr);
                break;
            }
        case OFDP_SET_DL_DST:
            {
                ofp_action_set_field &action =  *((ofp_action_set_field *)action_head);
                action.set_type(OFPAT_SET_FIELD);
                action.set_len(ofp_action_set_field::get_size<dl_mac_type>());
                action.set_byte_array(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_ETH_DST, OFDP_MAC_LEN, actions[i].value.dl_addr);
                break;
            }
        case OFDP_SET_NW_SRC:
            {
                ofp_action_set_field &action =  *((ofp_action_set_field *)action_head);
                action.set_type(OFPAT_SET_FIELD);
                action.set_len(ofp_action_set_field::get_size<uint32_t>());
                action.set_data<uint32_t>(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IPV4_SRC, actions[i].value.nw_addr);
                break;
            }
        case OFDP_SET_NW_DST:
            {
                ofp_action_set_field &action =  *((ofp_action_set_field *)action_head);
                action.set_type(OFPAT_SET_FIELD);
                action.set_len(ofp_action_set_field::get_size<uint32_t>());
                action.set_data<uint32_t>(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_IPV4_DST, actions[i].value.nw_addr);
                break;
            }
        case OFDP_SET_TP_SRC:
            {
                ofp_action_set_field &action =  *((ofp_action_set_field *)action_head);
                action.set_type(OFPAT_SET_FIELD);
                action.set_len(ofp_action_set_field::get_size<uint16_t>());
                action.set_data<uint16_t>(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_TCP_SRC, actions[i].value.tp_port);
                break;
            }
        case OFDP_SET_TP_DST:
            {
                ofp_action_set_field &action =  *((ofp_action_set_field *)action_head);
                action.set_type(OFPAT_SET_FIELD);
                action.set_len(ofp_action_set_field::get_size<uint16_t>());
                action.set_data<uint16_t>(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_TCP_DST, actions[i].value.tp_port);
                break;
            }
        case OFDP_SET_MPLS_LABEL:
            {
                ofp_action_set_field &action =  *((ofp_action_set_field *)action_head);
                action.set_type(OFPAT_SET_FIELD);
                action.set_len(ofp_action_set_field::get_size<uint32_t>());
                action.set_data<uint32_t>(OFPXMC_OPENFLOW_BASIC, OFPXMT_OFB_MPLS_LABEL, actions[i].value.push_mpls.label);
                break;
            }
        case OFDP_POP_MPLS:
            {
                ofp_action_pop &action =  *((ofp_action_pop *)action_head);
                action.set_type(OFPAT_POP_MPLS);
                action.set_len(sizeof(ofp_action_pop));
                action.set_ethertype(actions[i].value.push_mpls.ethertype);
                break;
            }
        case OFDP_PUSH_MPLS:
            { // need consecutive 2 action be encoded
                ofp_action_push &action =  *((ofp_action_push *)action_head);
                action.set_type(OFPAT_PUSH_MPLS);
                action.set_len(sizeof(ofp_action_push));
                action.set_ethertype(actions[i].value.push_mpls.ethertype);
                break;
            }
        case OFDP_POP_VLAN:
            {
                ofp_action_header &action =  *((ofp_action_header *)action_head);
                action.set_type(OFPAT_POP_VLAN);
                action.set_len(sizeof(ofp_action_header));
                break;
            }
        case OFDP_PUSH_VLAN_VID:
            {
                ofp_action_push &action =  *((ofp_action_push *)action_head);
                action.set_type(OFPAT_PUSH_VLAN);
                action.set_len(sizeof(ofp_action_push));
                action.set_ethertype(actions[i].value.vlan.ethertype);
                break;
            }
        case OFDP_SET_SIG_TYPE:
            {
                ofp_action_set_field &action =  *((ofp_action_set_field *)action_head);
                action.set_type(OFPAT_SET_FIELD);
                action.set_len(ofp_action_set_field::get_size<oxm_odu_sigtype>());
                // field object
                oxm_odu_sigtype *value = reinterpret_cast<oxm_odu_sigtype *>(action.fields.data_);
                action.fields.setHeader(OFPXMC_EXPERIMENTER, OFPXMT_EXP_ODU_SIGTYPE, value->get_minimum_length());
                value->sigtype = actions[i].value.signal_type;
                value->set_experimenter(OFP_EXPERIMENTER_OUI);
                break;
            }
        case OFDP_SET_SIG_ID:
            {
                ofp_action_set_field &action =  *((ofp_action_set_field *)action_head);
                const auto& sigid = actions[i].value.sigid;
                action.set_type(OFPAT_SET_FIELD);
                const unsigned no_of_bytes = (sigid.tslen <=8)?1 : (sigid.tslen/TS_PER_BYTE);
                action.set_len(ofp_action_set_field::get_size<oxm_odu_sigid>(no_of_bytes));
                oxm_odu_sigid *value = reinterpret_cast<oxm_odu_sigid *>(action.fields.data_);
                action.fields.setHeader(OFPXMC_EXPERIMENTER, OFPXMT_EXP_ODU_SIGID, value->get_minimum_length() + no_of_bytes);
                // field object
                value->set_experimenter(OFP_EXPERIMENTER_OUI);
                value->set_tpn(sigid.tpn);
                value->set_tslen(sigid.tslen);
                for (unsigned x = 0; x < no_of_bytes; x++) {
                    value->tsmap[x] = sigid.timeslots[x];
                }
                break;
            }
        default:
            break;
        }
        if ((action_head->get_len() % 8) != 0) {
            std::cout << "Action Encoding Error: len not 8 byte aligned\n";
            assert((action_head->get_len() % 8) == 0);
            return false;
        }
        if (instruction_head) instruction_head->set_len(instruction_head->get_len() + action_head->get_len());
        msg.set_length(msg.get_length() + action_head->get_len());
        action_head = action_head->get_next();
    } // end for

    return true;
}

/**
 * convert oxm_fields_t element to OfDpMatchT element
 * @see FlowMod
 * @author sraihan 
 * 
 * @param cur_field 
 * @param match 
 * @param out 
 * @param code 
 * 
 * @return bool 
 */
bool ParseOXMField(const oxm_fields_t *&cur_field,
                   OfDpMatchT &match,
                   std::ostream &out,
                   uint16_t &code) {
    if (!cur_field->isOpenflowBasic()) {
        out << "\n** oxm_header: " << ntohl(cur_field->header_) << "\n";
        code = OFPBMC_BAD_TAG;
        return false;
    }
    if (cur_field->getClass() == ::OFPXMC_EXPERIMENTER) {
        return ParseOXMExperimenter(cur_field, match, out, code);
    }
    switch (cur_field->getField()) {
    case OFPXMT_OFB_IN_PORT:
        {
            if (cur_field->hasMask()) {
                code = OFPBMC_BAD_MASK;
                return false;
            }
            if (match.in_port) {
                code = OFPBMC_DUP_FIELD;
                return false;
            }
            uint32_t value = 0;
            if (!cur_field->get_value<uint32_t>(value)) { // read data
                code = OFPBMC_BAD_LEN;
                return false;
            }
            match.in_port = value;
            // forwarding to next element
            cur_field = cur_field->get_next();
            out << " match[OFPXMT_OFB_IN_PORT] " << value << " ";
            return true;
        }
    case OFPXMT_OFB_IN_PHY_PORT:
        {
            if (cur_field->hasMask()) {
                code = OFPBMC_BAD_MASK;
                return false;
            }
            if (match.in_port) {
                code = OFPBMC_DUP_FIELD;
                return false;
            }
            uint32_t value = 0;
            if (!cur_field->get_value<uint32_t>(value)) { // read data
                code = OFPBMC_BAD_LEN;
                return false;
            }
            match.in_port = value;
            // forwarding to next element
            cur_field = cur_field->get_next();
            out << " match[OFPXMT_OFB_IN_PHY_PORT] " << match.in_port << " ";
            return true;
        }
    case OFPXMT_OFB_ETH_DST:
        {
            if (cur_field->hasMask()) {
                code = OFPBMC_BAD_MASK;
                return false;
            }
            uint8_t *value = 0;
            if (!cur_field->get_value_as_uint8ptr(value) || cur_field->getLength() != OFDP_MAC_LEN) { // read data
                code = OFPBMC_BAD_LEN;
                return false;
            }
            memcpy(match.dl_dst.data(), value, OFDP_MAC_LEN);
            cur_field = cur_field->get_next(); // forwarding to next element
            out << " match[OFPXMT_OFB_ETH_DST] "
                << (unsigned)value[0] << ":" <<  (unsigned)value[1] << ":" <<  (unsigned)value[2]
                << ":" <<  (unsigned)value[3] << ":" <<  (unsigned)value[4] << ":" <<  (unsigned)value[5];
            return true;
        }
    case OFPXMT_OFB_ETH_SRC:
        {
            if (cur_field->hasMask()) {
                code = OFPBMC_BAD_MASK;
                return false;
            }
            uint8_t *value = 0;
            if (!cur_field->get_value_as_uint8ptr(value) || cur_field->getLength() != OFDP_MAC_LEN) { // read data
                code = OFPBMC_BAD_LEN;
                return false;
            }
            memcpy(match.dl_src.data(), value, OFDP_MAC_LEN);
            cur_field = cur_field->get_next(); // forwarding to next element
            out << " match[OFPXMT_OFB_ETH_SRC] "
                << (unsigned)value[0] << ":" <<  (unsigned)value[1] << ":" <<  (unsigned)value[2]
                << ":" <<  (unsigned)value[3] << ":" <<  (unsigned)value[4] << ":" <<  (unsigned)value[5];
            return true;
        }
    case OFPXMT_OFB_ETH_TYPE:
        {
            if (cur_field->hasMask()) {
                code = OFPBMC_BAD_MASK;
                return false;
            }
            uint16_t value = 0;
            if (!cur_field->get_value<uint16_t>(value)) { // read data
                code = OFPBMC_BAD_LEN;
                return false;
            }
            match.dl_type = value;
            cur_field = cur_field->get_next(); // forwarding to next element
            out << " match[OFPXMT_OFB_ETH_TYPE] " << match.dl_type << " ";
            return true;
        }
    case OFPXMT_OFB_VLAN_VID:
        {
            if (cur_field->hasMask()) {
                code = OFPBMC_BAD_MASK;
                return false;
            }
            if (match.dl_vlan) {
                code = OFPBMC_DUP_FIELD;
                return false;
            }
            uint16_t value = 0;
            if (!cur_field->get_value<uint16_t>(value)) { // read data
                code = OFPBAC_BAD_LEN;
                return false;
            }
            // Dataplane abstraction must reset OFPVID_PRESENT bit
            match.dl_vlan = value; // FROM_0_TO_OFDP_VLAN_NONE(value)
            cur_field = cur_field->get_next(); // forwarding to next element
            out << " match[OFPXMT_OFB_VLAN_VID] " << match.dl_vlan << " ";
            return true;
        }
    case OFPXMT_OFB_VLAN_PCP:
        {
            if (cur_field->hasMask()) {
                code = OFPBMC_BAD_MASK;
                return false;
            }
            if (match.dl_vlan_pcp) {
                code = OFPBMC_DUP_FIELD;
                return false;
            }
            uint8_t value = 0;
            if (!cur_field->get_value<uint8_t>(value)) { // read data
                code = OFPBMC_BAD_LEN;
                return false;
            }

            if (!(match.dl_vlan & OFPVID_PRESENT)) {
                code = OFPBMC_BAD_PREREQ;
                return false;
            }
            match.dl_vlan_pcp = value;
            cur_field = cur_field->get_next(); // forwarding to next element
            out << " match[OFPXMT_OFB_VLAN_PCP] " << unsigned(match.dl_vlan_pcp) << " ";
            return true;
        }
    case OFPXMT_OFB_IP_PROTO:
        {
            if (cur_field->hasMask()) {
                code = OFPBMC_BAD_MASK;
                return false;
            }
            if (match.nw_proto) {
                code = OFPBMC_DUP_FIELD;
                return false;
            }
            if (!(match.dl_type == 0x0800 ||
                  match.dl_type ==  0x86dd)) { // Test for prerequisite eth_type == 0x0800 or 0x86dd
                code = OFPBMC_BAD_PREREQ;
                return false;
            }
            uint8_t value = 0;
            if (!cur_field->get_value<uint8_t>(value)) { // read data
                code = OFPBMC_BAD_LEN;
                return false;
            }
            match.nw_proto = value;
            cur_field = cur_field->get_next(); // forwarding to next element
            out << " match[OFPXMT_OFB_IP_PROTO] " << unsigned(match.nw_proto) << " ";
            return true;
        }
    case OFPXMT_OFB_IPV4_SRC:
        {
            if (match.nw_src) {
                code = OFPBMC_DUP_FIELD;
                return false;
            }
            if (match.dl_type != 0x0800) { // Test for prerequisite eth_type == 0x0800
                code = OFPBMC_BAD_PREREQ;
                return false;
            }
            uint32_t value = 0;
            if (!cur_field->get_value<uint32_t>(value)) { // read data
                code = OFPBMC_BAD_LEN;
                return false;
            }
            match.nw_src = value;

            out << " match[OFPXMT_OFB_IPV4_SRC] " << match.nw_src << " ";
            cur_field = cur_field->get_next(); // forwarding to next element
            return true;
        }
    case OFPXMT_OFB_IPV4_DST:
        {
            if (match.nw_dst) {
                code = OFPBMC_DUP_FIELD;
                return false;
            } else if (match.dl_type != 0x0800) { // Test for prerequisite eth_type == 0x0800
                code = OFPBMC_BAD_PREREQ;
                return false;
            }
            uint32_t value = 0;
            if (!cur_field->get_value<uint32_t>(value)) { // read data
                code = OFPBMC_BAD_LEN;
                return false;
            }
            match.nw_dst = value;
            out << " match[OFPXMT_OFB_IPV4_DST] " << match.nw_dst << " ";
            cur_field = cur_field->get_next(); // forwarding to next element
            return true;
        }
    case OFPXMT_OFB_TCP_SRC:
        {
            if (cur_field->hasMask()) {
                code = OFPBMC_BAD_MASK;
                return false;
            }
            if (match.tp_src) {
                code = OFPBMC_DUP_FIELD;
                return false;
            } else if (match.nw_proto != 6) { // Test for prerequisite ip_proto == 6
                code = OFPBMC_BAD_PREREQ;
                return false;
            }
            uint16_t value;
            if (!cur_field->get_value<uint16_t>(value)) { // read data
                code = OFPBMC_BAD_LEN;
                return false;
            }
            match.tp_src = value;
            cur_field = cur_field->get_next(); // forwarding to next element
            out << " match[OFPXMT_OFB_TCP_SRC] " << match.tp_src << " ";
            return true;
        }
    case OFPXMT_OFB_TCP_DST:
        {
            if (cur_field->hasMask()) {
                code = OFPBMC_BAD_MASK;
                return false;
            }
            if (match.tp_dst) {
                code = OFPBMC_DUP_FIELD;
                return false;
            } else if (match.nw_proto != 6) { // Test for prerequisite ip_proto == 6
                code = OFPBMC_BAD_PREREQ;
                return false;
            }
            uint16_t value;
            if (!cur_field->get_value<uint16_t>(value)) { // read data
                code = OFPBMC_BAD_LEN;
                return false;
            }
            match.tp_dst = value;
            cur_field = cur_field->get_next(); // forwarding to next element
            out << " match[OFPXMT_OFB_TCP_DST] " << match.tp_dst << " ";
            return true;
        }
    case OFPXMT_OFB_MPLS_LABEL:
        {
            if (cur_field->hasMask()) {
                code = OFPBMC_BAD_MASK;
                return false;
            }
            if (!(match.dl_type == 0x8847 ||
                  match.dl_type == 0x8848)) {
                code = OFPBMC_BAD_PREREQ;
                return false;
            }
            uint32_t value;
            if (!cur_field->get_value<uint32_t>(value)) { // read data
                code = OFPBMC_BAD_LEN;
                return false;
            }
            if (!match.mpls_label1) {
                match.mpls_label1 = value;
            } else {
                code = OFPBMC_DUP_FIELD;
                return false;
            }
            cur_field = cur_field->get_next(); // forwarding to next element
            out << " match[OFPXMT_OFB_MPLS_LABEL] " << match.mpls_label1 << " ";
            return true;
        }
    case OFPXMT_OFB_UDP_SRC:
        {
            if (cur_field->hasMask()) {
                code = OFPBMC_BAD_MASK;
                return false;
            }
            if (match.tp_src) {
                code = OFPBMC_DUP_FIELD;
                return false;
            } else if (match.nw_proto != 17) { // Test for prerequisite ip_proto == 6
                code = OFPBMC_BAD_PREREQ;
                return false;
            }
            uint16_t value;
            if (!cur_field->get_value<uint16_t>(value)) { // read data
                code = OFPBMC_BAD_LEN;
                return false;
            }
            match.tp_src = value;
            cur_field = cur_field->get_next(); // forwarding to next element
            out << " match[OFPXMT_OFB_UDP_SRC] " << match.tp_src << " ";
            return true;
        }
    case OFPXMT_OFB_UDP_DST:
        {
            if (cur_field->hasMask()) {
                code = OFPBMC_BAD_MASK;
                return false;
            }
            if (match.tp_dst) {
                code = OFPBMC_DUP_FIELD;
                return false;
            } else if (match.nw_proto != 17) { // Test for prerequisite ip_proto == 17
                code = OFPBMC_BAD_PREREQ;
                return false;
            }
            uint16_t value;
            if (!cur_field->get_value<uint16_t>(value)) { // read data
                code = OFPBMC_BAD_LEN;
                return false;
            }
            match.tp_dst = value;
            cur_field = cur_field->get_next(); // forwarding to next element
            out << " match[OFPXMT_OFB_UDP_DST] " << match.tp_dst << " ";
            return true;
        }
    case OFPXMT_OFB_IP_DSCP:
        {
            if (cur_field->hasMask()) {
                code = OFPBMC_BAD_MASK;
                return false;
            }
            if (match.nw_tos) {
                code = OFPBMC_DUP_FIELD;
                return false;
            }
            if (!(match.dl_type == 0x0800 ||
                  match.dl_type ==  0x86dd)) { // Test for prerequisite eth_type == 0x0800 or 0x86dd
                code = OFPBMC_BAD_PREREQ;
                return false;
            }
            uint8_t value = 0;
            if (!cur_field->get_value<uint8_t>(value)) { // read data
                code = OFPBMC_BAD_LEN;
                return false;
            }
            match.nw_tos = value;
            cur_field = cur_field->get_next(); // forwarding to next element
            out << " match[OFPXMT_OFB_IP_DSCP] " << unsigned(match.nw_tos) << " ";
            return true;
        }
    case OFPXMT_OFB_METADATA:
    case OFPXMT_OFB_IP_ECN:
    case OFPXMT_OFB_SCTP_SRC:
    case OFPXMT_OFB_SCTP_DST:
    case OFPXMT_OFB_ICMPV4_TYPE:
    case OFPXMT_OFB_ICMPV4_CODE:
    case OFPXMT_OFB_ARP_OP:
    case OFPXMT_OFB_ARP_SPA:
    case OFPXMT_OFB_ARP_TPA:
    case OFPXMT_OFB_ARP_SHA:
    case OFPXMT_OFB_ARP_THA:
    case OFPXMT_OFB_IPV6_SRC:
    case OFPXMT_OFB_IPV6_DST:
    case OFPXMT_OFB_IPV6_FLABEL:
    case OFPXMT_OFB_ICMPV6_TYPE:
    case OFPXMT_OFB_ICMPV6_CODE:
    case OFPXMT_OFB_IPV6_ND_TARGET:
    case OFPXMT_OFB_IPV6_ND_SLL:
    case OFPXMT_OFB_IPV6_ND_TLL:
    case OFPXMT_OFB_MPLS_TC:
        // 1.3+
    case OFPXMT_OFP_MPLS_BOS:
    case OFPXMT_OFB_PBB_ISID:
    case OFPXMT_OFB_TUNNEL_ID:
    case OFPXMT_OFB_IPV6_EXTHDR:
    default:
        break;
    } // end switch
    code = OFPBMC_BAD_TYPE;
    out << "Error: Bad/Unsupported OXM_MATCH Type\n";
    return false;
}

/**
 * Implements OTWG extensions
 * 
 * @author sraihan 
 * 
 * @param cur_field 
 * @param match 
 * @param out 
 * @param code 
 * 
 * @return bool 
 */
bool ParseOXMExperimenter(const oxm_fields_t *&cur_field,
                          OfDpMatchT &match,
                          std::ostream &out,
                          uint16_t &code) {
    const oxm_experimenter_fields *exp = reinterpret_cast<const oxm_experimenter_fields *>(cur_field->data_);
    if (exp->get_experimenter() != OFP_EXPERIMENTER_OUI) {
        out << "\n bad experimenter id: " << exp->get_experimenter() << "\n";
        code = OFPBMC_BAD_FIELD;
        return false;
    }
    switch (cur_field->getField()) {
    case OFPXMT_EXP_ODU_SIGID:
        {
            if (cur_field->hasMask()) {
                code = OFPBMC_BAD_MASK;
                return false;
            }
            if (match.tpn) {
                code = OFPBMC_DUP_FIELD;
                return false;
            }
            const oxm_odu_sigid *value = reinterpret_cast<const oxm_odu_sigid *>(cur_field->data_);
            const unsigned no_of_bytes = cur_field->getLength()-value->get_minimum_length();
            if (cur_field->getLength() < value->get_minimum_length() ||
                no_of_bytes > match.timeslots.size()
               ) {
                code = OFPBMC_BAD_LEN;
                return false;
            }
            match.tpn = value->get_tpn();
            match.tslen = value->get_tslen();
            memcpy(match.timeslots.data(), value->tsmap, no_of_bytes);
            // forwarding to next element
            cur_field = cur_field->get_next_exp();
            out << " match[OFPXMT_EXP_ODU_SIGID] " << value->get_tpn() << " ";
            return true;
        }
        break;
    case OFPXMT_EXP_ODU_SIGTYPE:
        {
            if (cur_field->hasMask()) {
                code = OFPBMC_BAD_MASK;
                return false;
            }
            if (match.signal_type) {
                code = OFPBMC_DUP_FIELD;
                return false;
            }
            const oxm_odu_sigtype *value = reinterpret_cast<const oxm_odu_sigtype *>(cur_field->data_);
            if (cur_field->getLength() != value->get_minimum_length()) {
                code = OFPBMC_BAD_LEN;
                return false;
            }
            match.signal_type = value->sigtype;
            // forwarding to next element
            cur_field = cur_field->get_next_exp();
            out << " match[OFPXMT_EXP_ODU_SIGTYPE] " << value->sigtype << " ";
            return true;
        }
        break;
    default:
        break;
    }; // end switch
    code = OFPBMC_BAD_TYPE;
    out << "Error: Bad/Unsupported OXM_MATCH Type\n";
    return false;
}

/**
 * convert ofp_tlv_base element to OfDpActionT element
 * @see              ParseInstructionItem
 * @author sraihan 
 * 
 * @param action_head 
 * @param actions 
 * @param num_actions 
 * @param out 
 * @param code 
 * 
 * @return bool 
 */
bool ParseActionItem(const ofp_tlv_base *&action_head,
                     OfDpActionT *actions,
                     uint16_t &num_actions,
                     std::ostream &out,
                     uint16_t &code) {
    switch (action_head->get_type()) {
    case OFPAT_OUTPUT:
        {
            actions[num_actions].command = OFDP_OUTPUT;
            if (action_head->get_len() != sizeof(ofp_action_output)) {
                out << "\n** OFPAT_OUTPUT expected len: " << sizeof(ofp_action_output) << " got: " << action_head->get_len() << "\n";
                code = OFPBAC_BAD_LEN;
                return false;
            }
            const uint32_t value = static_cast<const ofp_action_output *>(action_head)->get_port();
            actions[num_actions].value.output.port = value;
            actions[num_actions].value.output.max_len = static_cast<const ofp_action_output *>(action_head)->get_max_len();
            out << " actions[" << ++num_actions << "] : OFPAT_OUTPUT: " << value;
            break;
        }
    case OFPAT_GROUP:
        {
            actions[num_actions].command = OFDP_GROUP;
            const ofp_action_group *data = (const ofp_action_group *)(action_head);
            if (action_head->get_len() != data->get_minimum_length()) {
                out << "\n** OFPAT_GROUP expected len: " << data->get_minimum_length() << " got: " << action_head->get_len() << "\n";
                code = OFPBAC_BAD_LEN;
                return false;
            }
            const uint32_t value =  data->get_group_id();
            actions[num_actions].value.group_id = value;
            out << " actions[" << ++num_actions << "] : OFPAT_GROUP " << value;
            break;
        }
    case OFPAT_SET_FIELD:
        {
            const ofp_action_set_field *data = (const ofp_action_set_field *)(action_head);

            if (!data->fields.isOpenflowBasic() ||  data->fields.hasMask()) {
                code = OFPBAC_BAD_SET_TYPE;
                return false;
            }
            if (data->fields.getClass() == ::OFPXMC_EXPERIMENTER) {
                if (!ParseSetActionExperimenter(data,
                                                actions,
                                                num_actions,
                                                out,
                                                code)) {
                    out << "\n Set Experimenter Field Failed \n";
                }
                break;
            }
            switch (data->fields.getField()) {
            case OFPXMT_OFB_VLAN_VID:
                {
                    actions[num_actions].command = OFDP_SET_VLAN_VID;
                    uint16_t value = 4096;
                    if (!data->fields.get_value<uint16_t>(value)) { // read data
                        code = OFPBAC_BAD_SET_LEN;
                        return false;
                    }
                    value &= ~(OFPVID_PRESENT);
                    if (value < 4096) {
                        actions[num_actions].value.vlan.vid = value; //FROM_0_TO_OFDP_VLAN_NONE(value);// read data
                        out << " actions[" << ++num_actions << "] : OFPAT_SET_VLAN_VID " << value;
                    } else {
                        code = OFPBAC_BAD_SET_ARGUMENT;
                        return false;
                    }
                }
                break;
            case OFPXMT_OFB_VLAN_PCP:
                {
                    actions[num_actions].command = OFDP_SET_VLAN_PCP;
                    uint8_t value;
                    if (!data->fields.get_value<uint8_t>(value)) { // read data
                        code = OFPBAC_BAD_SET_LEN;
                        return false;
                    }
                    actions[num_actions].value.vlan.pcp = value; // read data
                    out << " actions[" << ++num_actions << "] : OFPAT_SET_VLAN_PCP ";
                }
                break;
            case OFPXMT_OFB_ETH_SRC:
                {
                    uint8_t *value;
                    if (data->fields.getLength() != OFDP_MAC_LEN ||
                        !data->fields.get_value_as_uint8ptr(value)) {
                        out << " SetField Type " << data->fields.getField() << ",";
                        code = OFPBAC_BAD_SET_LEN;
                        return false;
                    }
                    actions[num_actions].command = OFDP_SET_DL_SRC;
                    memcpy(&actions[num_actions].value.dl_addr, value, OFDP_MAC_LEN);
                    out << " actions[" << ++num_actions << "] : OFPAT_SET_DL_SRC "
                        << (unsigned)value[0] << ":" <<  (unsigned)value[1] << ":" <<  (unsigned)value[2]
                        << ":" <<  (unsigned)value[3] << ":" <<  (unsigned)value[4] << ":" <<  (unsigned)value[5];
                    break;
                }
            case OFPXMT_OFB_ETH_DST:
                {
                    uint8_t *value;
                    if (data->fields.getLength() != OFDP_MAC_LEN ||
                        !data->fields.get_value_as_uint8ptr(value)) {
                        out << " SetField Type " << data->fields.getField() << ",";
                        code = OFPBAC_BAD_SET_LEN;
                        return false;
                    }
                    actions[num_actions].command = OFDP_SET_DL_DST;
                    memcpy(&actions[num_actions].value.dl_addr, value, OFDP_MAC_LEN);
                    out << " actions[" << ++num_actions << "] : OFPAT_SET_DL_DST "
                        << (unsigned)value[0] << ":" <<  (unsigned)value[1] << ":" <<  (unsigned)value[2]
                        << ":" <<  (unsigned)value[3] << ":" <<  (unsigned)value[4] << ":" <<  (unsigned)value[5];
                    break;
                }
            case OFPXMT_OFB_IPV4_SRC:
                {
                    uint32_t value;
                    if (!data->fields.get_value<uint32_t>(value)) { // read data
                        code = OFPBAC_BAD_SET_LEN;
                        return false;
                    }
                    actions[num_actions].command = OFDP_SET_NW_SRC;
                    actions[num_actions].value.nw_addr = value;
                    out << " actions[" << ++num_actions << "] : OFPAT_SET_NW_SRC " << value;
                    break;
                }
            case OFPXMT_OFB_IPV4_DST:
                {
                    uint32_t value;
                    if (!data->fields.get_value<uint32_t>(value)) { // read data
                        code = OFPBAC_BAD_SET_LEN;
                        return false;
                    }
                    actions[num_actions].command = OFDP_SET_NW_DST;
                    actions[num_actions].value.nw_addr = value;
                    out << " actions[" << ++num_actions << "] : OFPAT_SET_NW_DST " << value;
                    break;
                }
            case OFPXMT_OFB_TCP_SRC:
                {
                    uint16_t value;
                    if (!data->fields.get_value<uint16_t>(value)) { // read data
                        code = OFPBAC_BAD_SET_LEN;
                        return false;
                    }
                    actions[num_actions].command = OFDP_SET_TP_SRC;
                    actions[num_actions].value.tp_port = value;
                    out << " actions[" << ++num_actions << "] : OFPAT_SET_TP_SRC " << value;
                    break;
                }
            case OFPXMT_OFB_TCP_DST:
                {
                    uint16_t value;
                    if (!data->fields.get_value<uint16_t>(value)) { // read data
                        code = OFPBAC_BAD_SET_LEN;
                        return false;
                    }
                    actions[num_actions].command = OFDP_SET_TP_DST;
                    actions[num_actions].value.tp_port = value;
                    out << " actions[" << ++num_actions << "] : OFPAT_SET_TP_DST " << value;
                    break;
                }
            case OFPXMT_OFB_MPLS_LABEL:
                {
                    uint32_t value;
                    if (!data->fields.get_value<uint32_t>(value)) { // read data
                        code = OFPBAC_BAD_SET_LEN;
                        return false;
                    }
                    actions[num_actions].command = OFDP_SET_MPLS_LABEL;
                    actions[num_actions].value.push_mpls.label = value;
                    out << " actions[" << ++num_actions << "] : OFPAT_SET_MPLS_LABEL " << value;
                    break;
                }
            default:
                code = OFPBAC_BAD_SET_TYPE;
                return false;
            };
            break;
        }
    case OFPAT_PUSH_MPLS:
        {
            actions[num_actions].command = OFDP_PUSH_MPLS;
            if (action_head->get_len() != sizeof(ofp_action_push)) {
                code = OFPBAC_BAD_LEN;
                return false;
            }
            const uint16_t value = static_cast<const ofp_action_push *>(action_head)->get_ethertype();
            actions[num_actions].value.push_mpls.ethertype = value;
            out << " actions[" << ++num_actions << "] : OFPAT_PUSH_MPLS " << value;
            break;
        }
    case OFPAT_POP_MPLS:
        {
            actions[num_actions].command = OFDP_POP_MPLS;
            if (action_head->get_len() != sizeof(ofp_action_pop)) {
                code = OFPBAC_BAD_LEN;
                return false;
            }
            const uint16_t value = static_cast<const ofp_action_push *>(action_head)->get_ethertype();
            actions[num_actions].value.push_mpls.ethertype = value;
            out << " actions[" << ++num_actions << "] : OFPAT_POP_MPLS " << value;
            break;
        }
    case OFPAT_PUSH_VLAN:
        {
            actions[num_actions].command = OFDP_PUSH_VLAN_VID;
            if (action_head->get_len() != sizeof(ofp_action_push)) {
                code = OFPBAC_BAD_LEN;
                return false;
            }
            const uint16_t value = static_cast<const ofp_action_push *>(action_head)->get_ethertype();
            actions[num_actions].value.vlan.ethertype = value;
            out << " actions[" << ++num_actions << "] : OFPAT_PUSH_VLAN " << value;
            break;
        }
    case OFPAT_POP_VLAN:
        {
            actions[num_actions].command = OFDP_POP_VLAN;
            if (action_head->get_len() != sizeof(ofp_action_pop)) {
                code = OFPBAC_BAD_LEN;
                return false;
            }
            out << " actions[" << ++num_actions << "] : OFPAT_POP_VLAN ";
            break;
        }
    case OFPAT_DEC_MPLS_TTL:
        {
            actions[num_actions].command = OFDP_DEC_MPLS_TTL;
            if (action_head->get_len() != sizeof(ofp_action_header)) {
                code = OFPBAC_BAD_LEN;
                return false;
            }
            out << " actions[" << ++num_actions << "] : OFPAT_DEC_MPLS_TTL ";
            break;
        }
    case OFPAT_SET_MPLS_TTL:
        {
            actions[num_actions].command = OFDP_SET_MPLS_TTL;
            if (action_head->get_len() != sizeof(ofp_action_mpls_ttl)) {
                code = OFPBAC_BAD_LEN;
                return false;
            }
            actions[num_actions].value.push_mpls.ttl = static_cast<const ofp_action_mpls_ttl *>(action_head)->mpls_ttl;
            out << " actions[" << ++num_actions << "] : OFPAT_SET_MPLS_TTL " << (unsigned)static_cast<const ofp_action_mpls_ttl *>(action_head)->mpls_ttl << " ";
            break;
        }
    case OFPAT_COPY_TTL_OUT:
    case OFPAT_COPY_TTL_IN:
    case OFPAT_SET_QUEUE:
    case OFPAT_SET_NW_TTL:
    case OFPAT_DEC_NW_TTL:
    case OFPAT_EXPERIMENTER:
    default:
        out << "\n** bad action type: " << action_head->get_type() << " length: " << action_head->get_len() << "\n";
        code = OFPBAC_BAD_TYPE;
//                assert(false);
        return false;
    }; // end switch
    if (!action_head->get_len()) {
        out << "\n** action type: " << action_head->get_type() << " bad length: " << action_head->get_len() << "\n";
        code = OFPBAC_BAD_LEN;
//            assert(false);
        return false;
    }
    action_head = action_head->get_next();
    return true;
}

/**
 *  
 * @author sraihan 
 * 
 * @param data 
 * @param actions 
 * @param num_actions 
 * @param out 
 * @param code 
 * 
 * @return bool 
 */
bool ParseSetActionExperimenter(const ofp_action_set_field *data,
                                OfDpActionT *actions,
                                uint16_t &num_actions,
                                std::ostream &out,
                                uint16_t &code) {
    const oxm_experimenter_fields *exp = reinterpret_cast<const oxm_experimenter_fields *>(data->fields.data_);
    if (exp->get_experimenter() != OFP_EXPERIMENTER_OUI) {
        code = OFPBAC_BAD_EXPERIMENTER;
        out << " Unsupported Experimenter \n";
    }
    switch (data->fields.getField()) {
    case OFPXMT_EXP_ODU_SIGID:
        {
            actions[num_actions].command = OFDP_SET_SIG_ID;
            const oxm_odu_sigid *value = reinterpret_cast<const oxm_odu_sigid *>(data->fields.data_);
            const unsigned no_of_bytes = data->fields.getLength()-value->get_minimum_length();
            auto& sigid = actions[num_actions].value.sigid;
            if (data->fields.getLength() < value->get_minimum_length() ||
                no_of_bytes > sizeof(sigid.timeslots)
               ) {
                code = OFPBAC_BAD_SET_LEN;
                return false;
            }
            sigid.tpn = value->get_tpn();
            sigid.tslen = value->get_tslen();
            for (unsigned x = 0; x < no_of_bytes; x++) {
                sigid.timeslots[x] = value->tsmap[x];
            }
            out << " actions[" << ++num_actions << "] : OFPXMT_EXP_ODU_SIGID " << value->get_tpn()
                << " ";
        }
        break;
    case OFPXMT_EXP_ODU_SIGTYPE:
        {
            actions[num_actions].command = OFDP_SET_SIG_TYPE;
            const oxm_odu_sigtype *value = reinterpret_cast<const oxm_odu_sigtype *>(data->fields.data_);
            if (data->fields.getLength() != value->get_minimum_length()) {
                code = OFPBAC_BAD_SET_LEN;
                return false;
            }
            actions[num_actions].value.signal_type = value->sigtype;
            out << " actions[" << ++num_actions << "] : OFPXMT_EXP_ODU_SIGTYPE " << value->sigtype << " ";
        }
        break;
    default:
        code = OFPBAC_BAD_EXP_TYPE;
        return false;
        break;
    }; // end switch
    return true;
}

/**
 * convert ofp_tlv_base element to OfDpActionT element
 * @see FlowMod
 * @author sraihan 
 * 
 * @param instruction_head 
 * @param actions 
 * @param num_actions 
 * @param max_actions 
 * @param out 
 * @param type 
 * @param code 
 * 
 * @return bool 
 */
bool ParseInstructionItem(const ofp_tlv_base *&instruction_head,
                          OfDpActionT *actions,
                          uint16_t &num_actions,
                          const size_t max_actions,
                          std::ostream &out,
                          uint16_t &type,
                          uint16_t &code) {
    switch (instruction_head->get_type()) {
    case OFPIT_APPLY_ACTIONS:
        {
            out << " instructions[OFPIT_APPLY_ACTIONS] length: " << instruction_head->get_len() << "\n";
            size_t current_len = sizeof(ofp_instruction_actions);
            const ofp_tlv_base *action_head = static_cast<const ofp_instruction_actions *>(instruction_head)->actions;
            while (current_len <  instruction_head->get_len()) {
                type = OFPET_BAD_INSTRUCTION;
                code = OFPBIC_BAD_LEN;
                if (TEST_GOOD_ENCODING(instruction_head, instruction_head->get_len(), action_head)) {
                    current_len += action_head->get_len();
                    if (num_actions == max_actions) {
                        type = OFPET_BAD_ACTION;
                        code = OFPBAC_TOO_MANY;
                        out << "\n** too much actions **\n";
                        return false;
                    } else if (!ParseActionItem(action_head,
                                                actions,
                                                num_actions,
                                                out, code)) {
                        type = OFPET_BAD_ACTION;
                        out << "\n** bad apply action **\n";
                        return false;
                    }
                } else {
                    out << "\n** Processing done **\n";
                    break;
                }
            }
            break;
        }
    case OFPIT_WRITE_ACTIONS:
        {
            out << " instructions[OFPIT_WRITE_ACTIONS] length: " << instruction_head->get_len() << "\n";
            size_t current_len = sizeof(ofp_instruction_actions);
            const ofp_tlv_base *action_head = static_cast<const ofp_instruction_actions *>(instruction_head)->actions;
            while (current_len <  instruction_head->get_len()) {
                type = OFPET_BAD_INSTRUCTION;
                code = OFPBIC_BAD_LEN;
                if (TEST_GOOD_ENCODING(instruction_head, instruction_head->get_len(), action_head)) {
                    current_len += action_head->get_len();
                    if (num_actions == max_actions) {
                        type = OFPET_BAD_ACTION;
                        code = OFPBAC_TOO_MANY;
                        out << "\n** too much actions **\n";
                        return false;
                    } else if (!ParseActionItem(action_head,
                                                actions,
                                                num_actions,
                                                out, code)) {
                        type = OFPET_BAD_ACTION;
                        out << "\n** bad write action **\n";
                        return false;
                    }
                } else {
                    out << "\n** processing done **\n";
                    break;
                }
            }
            break;
        }
    case OFPIT_CLEAR_ACTIONS:
    case OFPIT_GOTO_TABLE:
    case OFPIT_WRITE_METADATA:
    case OFPIT_EXPERIMENTER:
    default :
        out << "\n** bad instructions type: " <<  instruction_head->get_type() << " length: " << instruction_head->get_len() << "\n";
        type = OFPET_BAD_INSTRUCTION;
        code = OFPBIC_UNSUP_INST;
//                assert(false);
        return false;
    };
    if (!instruction_head->get_len()) {
        out << "\n** instructions type: " <<  instruction_head->get_type() << " bad length: " << instruction_head->get_len() << "\n";
        code = OFPBIC_BAD_LEN;
//            assert(false);
        return false;
    }
    instruction_head = instruction_head->get_next();
    return true;
}

/**
 * 1.3+
 * 
 * @author sraihan 
 * 
 * @param cur_bucket 
 * @param buckets 
 * @param num_buckets 
 * @param max_buckets 
 * @param out 
 * @param type 
 * @param code 
 * 
 * @return bool 
 */
bool ParseBuckets(const ofp_bucket *&cur_bucket,
                  OfDpBucketT *buckets,
                  uint16_t &num_buckets,
                  const size_t max_buckets,
                  std::ostream &out,
                  uint16_t &type,
                  uint16_t &code) {
    buckets->weight = cur_bucket->get_weight();
    buckets->watch_port = cur_bucket->get_watch_port();
    buckets->watch_group = cur_bucket->get_watch_group();

    out << " bucket[" << ++num_buckets << "]\n";

    buckets->num_actions = 0; // init before parsing actions
    const ofp_tlv_base *action_head = cur_bucket->actions;
    size_t current_len = sizeof(ofp_bucket);
    const size_t max_actions = sizeof(buckets->actions) / sizeof(buckets->actions[0]);

    if (!cur_bucket->get_len()) {
        out << "Error: 0 len for cur_bucket\n";
//        assert( cur_bucket->get_len() );
        return false;
    }
    while (current_len < cur_bucket->get_len()) {
        if (TEST_GOOD_ENCODING(cur_bucket, cur_bucket->get_len(), action_head)) {
            current_len += action_head->get_len();
            if (buckets->num_actions == max_actions) {
                type = OFPET_BAD_ACTION;
                code = OFPBAC_TOO_MANY;
                out << "\n** too much actions **\n";
                return false;
            } else if (!ParseActionItem(action_head,
                                        buckets->actions.data(),
                                        buckets->num_actions,
                                        out, code)) {
                type = OFPET_BAD_ACTION;
                out << "\n** bad bucket action **\n";
                return false;
            }
        } else {
            out << "\n** processing done **\n";
            break;
        }
    }
    out << "\n actions# " << buckets->num_actions << "\n";
    cur_bucket = cur_bucket->get_next();
    return true;
}

/**
 *  
 * @author sraihan 
 * 
 * @param msg 
 * @param cur_bucket 
 * @param buckets 
 * @param num_buckets 
 * @param out 
 * 
 * @return bool 
 */
bool EncodeBuckets(ofp_message &msg,
                   ofp_bucket *cur_bucket,
                   const OfDpBucketT *buckets,
                   const uint16_t num_buckets,
                   std::ostream &out) {
    for (uint16_t i = 0; i < num_buckets; i++) {
        const uint16_t msg_len = msg.get_length(); // without current bucket len and without actions
        msg.set_length(msg_len + cur_bucket->get_minimum_length()); // add bucket length to the message
        cur_bucket->set_weight(buckets[i].weight);
        cur_bucket->set_watch_port(buckets[i].watch_port);
        cur_bucket->set_watch_group(buckets[i].watch_group);
        if (!EncodeActionItem(msg, buckets[i].actions.data(), buckets[i].num_actions, false, -1)) {
            out << "\n** bad bucket action **\n";
            return false;
        }
        cur_bucket->set_len(msg.get_length() - msg_len); // compute actions length and set bucket len
        cur_bucket = cur_bucket->get_next();
    }
    return true;
} // end EncodeBuckets
} // end namespace OpenflowV4
