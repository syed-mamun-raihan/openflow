#ifndef OPENFLOWV4_OPENFLOW_1_3_H
#define OPENFLOWV4_OPENFLOW_1_3_H
/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */

#include "OfpHeader.h"
#include "OfConfig.hpp"
#include "OfDpDefine.hpp"
#pragma pack(push, 1)

namespace OpenflowV4 {

//Port numbering. Ports are numbered starting from 1.
enum ofp_port_no {
    OFPP_MAX =    0xffffff00,
    OFPP_IN_PORT =    0xfffffff8,
    OFPP_TABLE =    0xfffffff9,
    OFPP_NORMAL =    0xfffffffa,
    OFPP_FLOOD =    0xfffffffb,
    OFPP_ALL =    0xfffffffc,
    OFPP_CONTROLLER =    0xfffffffd,
    OFPP_LOCAL =    0xfffffffe,
    OFPP_ANY =    0xffffffff
};

//******* Common Structures *********************
enum ofp_type {
    /* Immutable messages. */
    OFPT_HELLO = 0,        /* Symmetric message */
    OFPT_ERROR = 1,        /* Symmetric message */
    OFPT_ECHO_REQUEST = 2, /* Symmetric message */
    OFPT_ECHO_REPLY = 3,   /* Symmetric message */
    OFPT_EXPERIMENTER = 4, /* Symmetric message */
    /* Switch configuration messages. */
    OFPT_FEATURES_REQUEST = 5,   /* Controller/switch message */
    OFPT_FEATURES_REPLY = 6,     /* Controller/switch message */
    OFPT_GET_CONFIG_REQUEST = 7, /* Controller/switch message */
    OFPT_GET_CONFIG_REPLY = 8,   /* Controller/switch message */
    OFPT_SET_CONFIG = 9,         /* Controller/switch message */
    /* Asynchronous messages. */
    OFPT_PACKET_IN = 10,    /* Async message */
    OFPT_FLOW_REMOVED = 11, /* Async message */
    OFPT_PORT_STATUS = 12,  /* Async message */
    /* Controller command messages. */
    OFPT_PACKET_OUT = 13,  /* Controller/switch message */
    OFPT_FLOW_MOD = 14,    /* Controller/switch message */
    OFPT_GROUP_MOD = 15,   /* Controller/switch message */
    OFPT_PORT_MOD = 16,    /* Controller/switch message */
    OFPT_TABLE_MOD = 17,   /* Controller/switch message */
    /* Statistics messages. */
    OFPT_STATS_REQUEST      = 18, /* Controller/switch message */
    OFPT_STATS_REPLY        = 19, /* Controller/switch message */
    /* Barrier messages. */
    OFPT_BARRIER_REQUEST = 20, /* Controller/switch message */
    OFPT_BARRIER_REPLY = 21,   /* Controller/switch message */
    /* Queue Configuration messages. */
    OFPT_QUEUE_GET_CONFIG_REQUEST = 22, /* Controller/switch message */
    OFPT_QUEUE_GET_CONFIG_REPLY = 23,   /* Controller/switch message */
    /* Controller role change request messages. */
    OFPT_ROLE_REQUEST = 24, /* Controller/switch message */
    OFPT_ROLE_REPLY = 25,   /* Controller/switch message */
    /* Asynchronous message configuration */
    OFPT_GET_ASYNC_REQUEST = 26, /* Controller/switch message */
    OFPT_GET_ASYNC_REPLY   = 27, /* Controller/switch message */
    OFPT_SET_ASYNC = 28, /* Controller/switch message */
    /* Meters and rate limiters configuration messages. */
    OFPT_METER_MOD = 29, /* Controller/switch message */
};

/* OXM Flow match field types for OpenFlow basic class. */
enum ofp_config_flags {
    OFPC_FRAG_NORMAL =    0,
    OFPC_FRAG_DROP =    1 << 0,
    OFPC_FRAG_REASM =    1 << 1,
    OFPC_FRAG_MASK =    3,
    OFPC_INVALID_TTL_TO_CONTROLLER =    1 << 2 // Send packets with invalid TTL to the controller
};
//Switch configuration.
//OFP_ASSERT(sizeof(struct ofp_switch_config) == 12);
struct ofp_switch_config : public ::ofp_message {
    uint16_t flags = 0;

    uint16_t miss_send_len = 0;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};

    inline uint16_t get_flags() const;

    inline void set_flags(const uint16_t value);

    inline uint16_t get_miss_send_len() const;

    inline void set_miss_send_len(const uint16_t value);

};
inline uint16_t ofp_switch_config::get_flags() const {
    return ntohs(flags);
}

inline void ofp_switch_config::set_flags(const uint16_t value) {
    flags = htons(value);
}

inline uint16_t ofp_switch_config::get_miss_send_len() const {
    return ntohs(miss_send_len);
}

inline void ofp_switch_config::set_miss_send_len(const uint16_t value) {
    miss_send_len = htons(value);
}

// Flags to indicate behavior of the flow table for unmatched packets.
//   These flags are used in ofp_table_stats messages to describe the current
//   configuration and in ofp_table_mod messages to configure table behavior.

enum ofp_table_config {
    OFPTC_TABLE_MISS_CONTROLLER =    0,
    OFPTC_TABLE_MISS_CONTINUE =    1 << 0,
    OFPTC_TABLE_MISS_DROP =    1 << 1,
    OFPTC_TABLE_MISS_MASK =    3
};
//Table numbering. Tables can use any number up to OFPT_MAX.

enum ofp_table {
    OFPTT_MAX =    0xfe,
    OFPTT_ALL =    0xff //Fake tables. Wildcard table used for table config,
                        //flow stats and flow deletes.

};
//Configure/Modify behavior of a flow table
//OFP_ASSERT(sizeof(struct ofp_table_mod) == 16);
struct ofp_table_mod : public ::ofp_message {
    uint8_t table_id = 0;

    uint8_t pad[3] = {0,0,0};

    uint32_t config = 0;

    inline uint32_t get_config() const;

    inline void set_config(const uint32_t value);

};
inline uint32_t ofp_table_mod::get_config() const {
    return ntohl(config);
}

inline void ofp_table_mod::set_config(const uint32_t value) {
    config = htons(value);
}

//Capabilities supported by the datapath.

enum ofp_capabilities {
    OFPC_FLOW_STATS =    1 << 0,
    OFPC_TABLE_STATS =    1 << 1,
    OFPC_PORT_STATS =    1 << 2,
    OFPC_GROUP_STATS =    1 << 3,
    OFPC_IP_REASM =    1 << 5,
    OFPC_QUEUE_STATS =    1 << 6,
    OFPC_PORT_BLOCKED =    1 << 8


};
//Current state of the physical port.  These are not configurable from
//the controller.

enum ofp_port_state {
    OFPPS_LINK_DOWN =    1 << 0,
    OFPPS_BLOCKED =    1 << 1,
    OFPPS_LIVE =    1 << 2

};
//Features of ports available in a datapath.

enum ofp_port_features {
    OFPPF_10MB_HD =    1 << 0,
    OFPPF_10MB_FD =    1 << 1,
    OFPPF_100MB_HD =    1 << 2,
    OFPPF_100MB_FD =    1 << 3,
    OFPPF_1GB_HD =    1 << 4,
    OFPPF_1GB_FD =    1 << 5,
    OFPPF_10GB_FD =    1 << 6,
    OFPPF_40GB_FD =    1 << 7,
    OFPPF_100GB_FD =    1 << 8,
    OFPPF_1TB_FD =    1 << 9,
    OFPPF_OTHER =    1 << 10,
    OFPPF_COPPER =    1 << 11,
    OFPPF_FIBER =    1 << 12,
    OFPPF_AUTONEG =    1 << 13,
    OFPPF_PAUSE =    1 << 14,
    OFPPF_PAUSE_ASYM =    1 << 15


};
//Description of a port
//OFP_ASSERT(sizeof(struct ofp_phy_port) == 64);
struct ofp_phy_port {
    uint32_t port_no = 0;

    uint32_t pad = 0;

    uint8_t hw_addr[OFP_ETH_ALEN] = {0,0,0,0,0,0};

    uint16_t pad2 = 0;

    char name[OFP_MAX_PORT_NAME_LEN];

    uint32_t config = 0;

    uint32_t state = 0;

    uint32_t curr = 0;

    uint32_t advertised = 0;

    uint32_t supported = 0;

    uint32_t peer = 0;

    uint32_t curr_speed = 0;

    uint32_t max_speed = 0;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};

    inline uint32_t get_port_no() const;

    inline void set_port_no(const uint32_t value);

    inline uint32_t get_config() const;

    inline void set_config(const uint32_t value);

    inline uint32_t get_state() const;

    inline void set_state(const uint32_t value);

    inline uint32_t get_curr() const;

    inline void set_curr(const uint32_t value);

    inline uint32_t get_advertised() const;

    inline void set_advertised(const uint32_t value);

    inline uint32_t get_supported() const;

    inline void set_supported(const uint32_t value);

    inline uint32_t get_peer() const;

    inline void set_peer(const uint32_t value);

    inline uint32_t get_curr_speed() const;

    inline void set_curr_speed(const uint32_t value);

    inline uint32_t get_max_speed() const;

    inline void set_max_speed(const uint32_t value);

};
inline uint32_t ofp_phy_port::get_port_no() const {
    return ntohl(port_no);
}

inline void ofp_phy_port::set_port_no(const uint32_t value) {
    port_no = htonl(value);
}

inline uint32_t ofp_phy_port::get_config() const {
    return ntohl(config);
}

inline void ofp_phy_port::set_config(const uint32_t value) {
    config = htonl(value);
}

inline uint32_t ofp_phy_port::get_state() const {
    return ntohl(state);
}

inline void ofp_phy_port::set_state(const uint32_t value) {
    state = htonl(value);
}

inline uint32_t ofp_phy_port::get_curr() const {
    return ntohl(curr);
}

inline void ofp_phy_port::set_curr(const uint32_t value) {
    curr = htonl(value);
}

inline uint32_t ofp_phy_port::get_advertised() const {
    return ntohl(advertised);
}

inline void ofp_phy_port::set_advertised(const uint32_t value) {
    advertised = htonl(value);
}

inline uint32_t ofp_phy_port::get_supported() const {
    return ntohl(supported);
}

inline void ofp_phy_port::set_supported(const uint32_t value) {
    supported = htonl(value);
}

inline uint32_t ofp_phy_port::get_peer() const {
    return ntohl(peer);
}

inline void ofp_phy_port::set_peer(const uint32_t value) {
    peer = htonl(value);
}

inline uint32_t ofp_phy_port::get_curr_speed() const {
    return ntohl(curr_speed);
}

inline void ofp_phy_port::set_curr_speed(const uint32_t value) {
    curr_speed = htonl(value);
}

inline uint32_t ofp_phy_port::get_max_speed() const {
    return ntohl(max_speed);
}

inline void ofp_phy_port::set_max_speed(const uint32_t value) {
    max_speed = htonl(value);
}

// What changed about the physical port

enum ofp_port_reason {
    OFPPR_ADD =    0,
    OFPPR_DELETE =    1,
    OFPPR_MODIFY =    2

};
//A physical port has changed in the datapath
//OFP_ASSERT(sizeof(struct ofp_port_status) == 80);
struct ofp_port_status : public ::ofp_message {
    uint8_t reason = 0;

    uint8_t pad[7] = {0,0,0,0,0,0,0};

    struct ofp_phy_port desc;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};

};
//Modify behavior of the physical port
//OFP_ASSERT(sizeof(struct ofp_port_mod) == 40);
struct ofp_port_mod : public ::ofp_message {
    uint32_t port_no = 0;

    uint32_t pad = 0;

    uint8_t hw_addr[OFP_ETH_ALEN] = {0,0,0,0,0,0};

    uint16_t pad2 = 0;

    uint32_t config = 0;

    uint32_t mask = 0;

    uint32_t advertise = 0;

    uint32_t pad3 = 0;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};

    inline uint32_t get_port_no() const;

    inline void set_port_no(const uint32_t value);

    inline uint32_t get_config() const;

    inline void set_config(const uint32_t value);

    inline uint32_t get_mask() const;

    inline void set_mask(const uint32_t value);

    inline uint32_t get_advertise() const;

    inline void set_advertise(const uint32_t value);

};
inline uint32_t ofp_port_mod::get_port_no() const {
    return ntohl(port_no);
}

inline void ofp_port_mod::set_port_no(const uint32_t value) {
    port_no = htonl(value);
}

inline uint32_t ofp_port_mod::get_config() const {
    return ntohl(config);
}

inline void ofp_port_mod::set_config(const uint32_t value) {
    config = htonl(value);
}

inline uint32_t ofp_port_mod::get_mask() const {
    return ntohl(mask);
}

inline void ofp_port_mod::set_mask(const uint32_t value) {
    mask = htonl(value);
}

inline uint32_t ofp_port_mod::get_advertise() const {
    return ntohl(advertise);
}

inline void ofp_port_mod::set_advertise(const uint32_t value) {
    advertise = htonl(value);
}


// ## OpenFlow Extensible Match. ##
// The match type indicates the match structure (set of fields that compose the
//  match) in use. The match type is placed in the type field at the beginning
//  of all match structures. The "OpenFlow Extensible Match" type corresponds
//  to OXM TLV format described below and must be supported by all OpenFlow
//  switches. Extensions that define other match types may be published on the
//  ONF wiki. Support for extensions is optional.
enum ofp_match_type {
    OFPMT_STANDARD =    0,
    OFPMT_OXM =    1,
};

// OpenFlow Extensible Match fields to match against flows
struct ofp_match : public ::ofp_tlv_base {
    oxm_fields_t oxm_fields[1];
    inline size_t get_minimum_length() { return (sizeof(*this));}
    inline const ::ofp_tlv_base* get_next_element() const {
        const size_t match_len_with_padding = ::ofp_tlv_base::get_size_64bit_aligned(get_len());
        const ofp_tlv_base *base_ptr = static_cast<const ::ofp_tlv_base *>(this);
        const uint8_t *next_ptr = reinterpret_cast<const uint8_t *>(base_ptr) + match_len_with_padding;
        return reinterpret_cast<const ::ofp_tlv_base *>(next_ptr);
    }
};

/* OXM Flow match field types for OpenFlow basic class. */
enum oxm_ofb_match_fields {
    OFPXMT_OFB_IN_PORT = 0,         /* Switch input port. */
    OFPXMT_OFB_IN_PHY_PORT = 1,     /* Switch physical input port. */
    OFPXMT_OFB_METADATA = 2,        /* Metadata passed between tables. */
    OFPXMT_OFB_ETH_DST = 3,         /* Ethernet destination address. */
    OFPXMT_OFB_ETH_SRC = 4,         /* Ethernet source address. */
    OFPXMT_OFB_ETH_TYPE = 5,        /* Ethernet frame type. */
    OFPXMT_OFB_VLAN_VID = 6,        /* VLAN id. */
    OFPXMT_OFB_VLAN_PCP = 7,        /* VLAN priority. */
    OFPXMT_OFB_IP_DSCP = 8,         /* IP DSCP (6 bits in ToS field). */
    OFPXMT_OFB_IP_ECN = 9,          /* IP ECN (2 bits in ToS field). */
    OFPXMT_OFB_IP_PROTO = 10,       /* IP protocol. */
    OFPXMT_OFB_IPV4_SRC = 11,       /* IPv4 source address. */
    OFPXMT_OFB_IPV4_DST = 12,       /* IPv4 destination address. */
    OFPXMT_OFB_TCP_SRC = 13,        /* TCP source port. */
    OFPXMT_OFB_TCP_DST = 14,        /* TCP destination port. */
    OFPXMT_OFB_UDP_SRC = 15,        /* UDP source port. */
    OFPXMT_OFB_UDP_DST = 16,        /* UDP destination port. */
    OFPXMT_OFB_SCTP_SRC = 17,       /* SCTP source port. */
    OFPXMT_OFB_SCTP_DST = 18,       /* SCTP destination port. */
    OFPXMT_OFB_ICMPV4_TYPE = 19,    /* ICMP type. */
    OFPXMT_OFB_ICMPV4_CODE = 20,    /* ICMP code. */
    OFPXMT_OFB_ARP_OP = 21,         /* ARP opcode. */
    OFPXMT_OFB_ARP_SPA = 22,        /* ARP source IPv4 address. */
    OFPXMT_OFB_ARP_TPA = 23,        /* ARP target IPv4 address. */
    OFPXMT_OFB_ARP_SHA = 24,        /* ARP source hardware address. */
    OFPXMT_OFB_ARP_THA = 25,        /* ARP target hardware address. */
    OFPXMT_OFB_IPV6_SRC = 26,       /* IPv6 source address. */
    OFPXMT_OFB_IPV6_DST = 27,       /* IPv6 destination address. */
    OFPXMT_OFB_IPV6_FLABEL = 28,    /* IPv6 Flow Label */
    OFPXMT_OFB_ICMPV6_TYPE = 29,    /* ICMPv6 type. */
    OFPXMT_OFB_ICMPV6_CODE = 30,    /* ICMPv6 code. */
    OFPXMT_OFB_IPV6_ND_TARGET = 31, /* Target address for ND. */
    OFPXMT_OFB_IPV6_ND_SLL = 32,    /* Source link-layer for ND. */
    OFPXMT_OFB_IPV6_ND_TLL = 33,    /* Target link-layer for ND. */
    OFPXMT_OFB_MPLS_LABEL = 34,     /* MPLS label. */
    OFPXMT_OFB_MPLS_TC = 35,        /* MPLS TC. */
    OFPXMT_OFP_MPLS_BOS = 36,       /* MPLS BoS bit. */
    OFPXMT_OFB_PBB_ISID = 37,       /* PBB I-SID. */
    OFPXMT_OFB_TUNNEL_ID = 38,      /* Logical Port Metadata. */
    OFPXMT_OFB_IPV6_EXTHDR = 39,    /* IPv6 Extension Header pseudo-field */
    OFPXMT_OFB_VENDOR = 0x7f        /* Vendor extension for match field */

};

enum ofp_vlan_id {
    OFPVID_PRESENT =    0x1000,
    OFPVID_NONE =    0x0000

};


enum ofp_instruction_type {
    OFPIT_GOTO_TABLE = 1,       /* Setup the next table in the lookup */
    OFPIT_WRITE_METADATA = 2,   /* Setup the metadata field for use later in
                                 pipeline */
    OFPIT_WRITE_ACTIONS = 3,    /* Write the action(s) onto the datapath action
                                set */
    OFPIT_APPLY_ACTIONS = 4,    /* Applies the action(s) immediately */
    OFPIT_CLEAR_ACTIONS = 5,    /* Clears all actions from the datapath
                                   action set */
    OFPIT_METER = 6,            /* 1.3+ only Apply meter (rate limiter) */

    OFPIT_EXPERIMENTER = 0xFFFF /* Experimenter instruction */
};

enum ofp_action_type {
    OFPAT_OUTPUT = 0,        /* Output to switch port. */
    OFPAT_COPY_TTL_OUT = 11, /* Copy TTL "outwards" -- from next-to-outermost
                                to outermost */
    OFPAT_COPY_TTL_IN = 12,  /* Copy TTL "inwards" -- from outermost to
                                next-to-outermost */
    OFPAT_SET_MPLS_TTL = 15, /* MPLS TTL */
    OFPAT_DEC_MPLS_TTL = 16, /* Decrement MPLS TTL */
    OFPAT_PUSH_VLAN = 17,    /* Push a new VLAN tag */
    OFPAT_POP_VLAN = 18,     /* Pop the outer VLAN tag */
    OFPAT_PUSH_MPLS = 19,    /* Push a new MPLS tag */
    OFPAT_POP_MPLS = 20,     /* Pop the outer MPLS tag */
    OFPAT_SET_QUEUE = 21,    /* Set queue id when outputting to a port */
    OFPAT_GROUP = 22,        /* Apply group. */
    OFPAT_SET_NW_TTL = 23,   /* IP TTL. */
    OFPAT_DEC_NW_TTL = 24,   /* Decrement IP TTL. */
    OFPAT_SET_FIELD = 25,    /* Set a header field using OXM TLV format. */
    OFPAT_PUSH_PBB = 26,     /*Push a new PBB service tag (I-TAG) */
    OFPAT_POP_PBB = 27,      /* Pop the outer PBB service tag (I-TAG) */
    OFPAT_EXPERIMENTER = 0xffff
};

// indicates that no buffering should be applied and the whole packet is to be sent to the controller.
// maximum max_len value which can be used to request a specific byte length.
// indicates that no buffering should be applied and the whole packet is to be sent to the controller.
// Action structure for OFPAT_OUTPUT, which sends packets out 'port'.
// When the 'port' is the OFPP_CONTROLLER, 'max_len' indicates the max
// number of bytes to send.  A 'max_len' of zero means no bytes of the
// packet should be sent. A 'max_len' of OFPCML_NO_BUFFER means that
// the packet is not buffered and the complete packet is to be sent to
// the controller.
struct ofp_action_output : public ::ofp_tlv_base {
    uint32_t port = 0;

    uint16_t max_len = 0;

    uint8_t pad[6] = {0,0,0,0,0,0};

    inline unsigned int get_minimum_length() const { return sizeof(*this);};

    inline uint32_t get_port() const;

    inline void set_port(const uint32_t value);

    inline uint16_t get_max_len() const;

    inline void set_max_len(const uint16_t value);

};
inline uint32_t ofp_action_output::get_port() const {
    return ntohl(port);
}

inline void ofp_action_output::set_port(const uint32_t value) {
    port = htonl(value);
}

inline uint16_t ofp_action_output::get_max_len() const {
    return ntohs(max_len);
}

inline void ofp_action_output::set_max_len(const uint16_t value) {
    max_len = htons(value);
}

// Action structure for OFPAT_SET_MPLS_TTL.
struct ofp_action_mpls_ttl : public ::ofp_tlv_base {
    uint8_t mpls_ttl = 0;

    uint8_t pad[3] = {0,0,0};
    inline unsigned int get_minimum_length() const { return sizeof(*this);};

};
// Action structure for OFPAT_PUSH_VLAN/MPLS or
//OFPAT_POP_MPLS/VLAN
struct ofp_action_push : public ::ofp_tlv_base {
    uint16_t ethertype = 0;

    uint16_t pad = 0;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint16_t get_ethertype() const;
    inline void set_ethertype(const uint16_t value);
};

inline uint16_t ofp_action_push::get_ethertype() const {
    return ntohs(ethertype);
}

inline void ofp_action_push::set_ethertype(const uint16_t value) {
    ethertype = htons(value);
}

struct ofp_action_pop : public ::ofp_tlv_base {
    uint16_t ethertype = 0;

    uint16_t pad = 0;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint16_t get_ethertype() const;
    inline void set_ethertype(const uint16_t value);
};

inline uint16_t ofp_action_pop::get_ethertype() const {
    return ntohs(ethertype);
}

inline void ofp_action_pop::set_ethertype(const uint16_t value) {
    ethertype = htons(value);
}

// Action structure for OFPAT_GROUP.
struct ofp_action_group : public ::ofp_tlv_base {
    uint32_t group_id = 0;

    inline uint32_t get_group_id() const;

    inline void set_group_id(const uint32_t value);
    inline unsigned int get_minimum_length() const { return sizeof(*this);};
};

inline uint32_t ofp_action_group::get_group_id() const {
    return ntohl(group_id);
}

inline void ofp_action_group::set_group_id(const uint32_t value) {
    group_id = htonl(value);
}

// Action structure for OFPAT_SET_NW_TTL.
struct ofp_action_nw_ttl : public ::ofp_tlv_base {
    uint8_t nw_ttl = 0;

    uint8_t pad[3] = {0,0,0};
    inline unsigned int get_minimum_length() const { return sizeof(*this);};

};

// Instruction structure for OFPIT_GOTO_TABLE
struct ofp_instruction_goto_table : public ::ofp_tlv_base {
    uint8_t table_id = 0;

    uint8_t pad[3] = {0,0,0};
    inline unsigned int get_minimum_length() const { return sizeof(*this);};

};
// Instruction structure for OFPIT_WRITE_METADATA
struct ofp_instruction_write_metadata : public ::ofp_tlv_base {
    uint32_t pad = 0;

    uint64_t metadata = 0;

    uint64_t metadata_mask = 0;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint64_t get_metadata() const;

    inline void set_metadata(const uint64_t value);

    inline uint64_t get_metadata_mask() const;

    inline void set_metadata_mask(const uint64_t value);

};
inline uint64_t ofp_instruction_write_metadata::get_metadata() const {
    return ofp_message::ntohll(metadata);
}

inline void ofp_instruction_write_metadata::set_metadata(const uint64_t value) {
    metadata = ofp_message::htonll(value);
}

inline uint64_t ofp_instruction_write_metadata::get_metadata_mask() const {
    return ofp_message::ntohll(metadata_mask);
}

inline void ofp_instruction_write_metadata::set_metadata_mask(const uint64_t value) {
    metadata_mask = ofp_message::htonll(value);
}

// Instruction structure for OFPIT_WRITE/APPLY/CLEAR_ACTIONS
struct ofp_instruction_actions : public ::ofp_tlv_base {
    uint32_t pad = 0;

    ::ofp_tlv_base actions[0]; // struct ofp_action_header (1.2) 64bit aligned
    inline unsigned int get_minimum_length() const { return sizeof(*this);};

};

struct ofp_switch_features : public ofp_message {
    uint64_t datapath_id = 0;     /* Datapath unique ID. The lower 48-bits are for
                                 a MAC address, while the upper 16-bits are
                                 implementer-defined. */
    uint32_t n_buffers = 0;       /* Max packets buffered at once. */
    uint8_t n_tables = 0;         /* Number of tables supported by datapath. */
    uint8_t auxiliary_id = 0;     /* Identify auxiliary connections. */
    uint16_t pad = 0;           /* Align to 64-bits. */
    /* Features. */
    uint32_t capabilities = 0;    /* Bitmap of support "ofp_capabilities". */
    uint32_t reserved = 0;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};

    inline uint64_t get_datapath_id() const;

    inline void set_datapath_id(const uint64_t value);

    inline uint32_t get_n_buffers() const;

    inline void set_n_buffers(const uint32_t value);

    inline uint32_t get_capabilities() const;

    inline void set_capabilities(const uint32_t value);

};
inline uint64_t ofp_switch_features::get_datapath_id() const {
    return ofp_message::ntohll(datapath_id);
}

inline void ofp_switch_features::set_datapath_id(const uint64_t value) {
    datapath_id = ofp_message::htonll(value);
}

inline uint32_t ofp_switch_features::get_n_buffers() const {
    return ntohl(n_buffers);
}

inline void ofp_switch_features::set_n_buffers(const uint32_t value) {
    n_buffers = htonl(value);
}

inline uint32_t ofp_switch_features::get_capabilities() const {
    return ntohl(capabilities);
}

inline void ofp_switch_features::set_capabilities(const uint32_t value) {
    capabilities = htonl(value);
}
// ## OpenFlow Flow Modification. ##

enum ofp_flow_mod_command {
    OFPFC_ADD =    0,
    OFPFC_MODIFY =    1,
    OFPFC_MODIFY_STRICT =    2,
    OFPFC_DELETE =    3,
    OFPFC_DELETE_STRICT =    4

};
//Used in the ofp_flow_mod message
enum ofp_flow_mod_flags {
    OFPFF_SEND_FLOW_REM = 1 << 0, /* Send flow removed message when flow
                                   * expires or is deleted. */
    OFPFF_CHECK_OVERLAP = 1 << 1, /* Check for overlapping entries first. */
    OFPFF_RESET_COUNTS = 1 << 2,   /* Reset flow packet and byte counts. */
    OFPFF_NO_PKT_COUNTS = 1 << 3,  /* Don�t keep track of packet count. */
    OFPFF_NO_BYT_COUNTS = 1 << 4  /*Don�t keep track of byte count. */
};

//Flow setup and teardown (controller -> datapath).
struct ofp_flow_mod : public ::ofp_message {
    uint64_t cookie = 0;

    uint64_t cookie_mask = 0;

    uint8_t table_id = 0;

    uint8_t command = 0;

    uint16_t idle_timeout = 0;

    uint16_t hard_timeout = 0;

    uint16_t priority = 0;

    uint32_t buffer_id = 0;

    uint32_t out_port = 0;

    uint32_t out_group = 0;

    uint16_t flags = 0;

    uint16_t pad[2] = 0;

    struct ofp_match match;
    //tlv_base instructions[0]; /* Instruction set */
    inline unsigned int get_minimum_length() const { return (sizeof(*this));};

    inline uint8_t get_command() const { return command;}

    inline uint64_t get_cookie() const;

    inline void set_cookie(const uint64_t value);

    inline uint64_t get_cookie_mask() const;

    inline void set_cookie_mask(const uint64_t value);

    inline uint16_t get_idle_timeout() const;

    inline void set_idle_timeout(const uint16_t value);

    inline uint16_t get_hard_timeout() const;

    inline void set_hard_timeout(const uint16_t value);

    inline uint16_t get_priority() const;

    inline void set_priority(const uint16_t value);

    inline uint32_t get_buffer_id() const;

    inline void set_buffer_id(const uint32_t value);

    inline uint32_t get_out_port() const;

    inline void set_out_port(const uint32_t value);

    inline uint32_t get_out_group() const;

    inline void set_out_group(const uint32_t value);

    inline uint16_t get_flags() const;

    inline void set_flags(const uint16_t value);

};
inline uint64_t ofp_flow_mod::get_cookie() const {
    return ofp_message::ntohll(cookie);
}

inline void ofp_flow_mod::set_cookie(const uint64_t value) {
    cookie = ofp_message::htonll(value);
}

inline uint64_t ofp_flow_mod::get_cookie_mask() const {
    return ofp_message::ntohll(cookie_mask);
}

inline void ofp_flow_mod::set_cookie_mask(const uint64_t value) {
    cookie_mask = ofp_message::htonll(value);
}

inline uint16_t ofp_flow_mod::get_idle_timeout() const {
    return ntohs(idle_timeout);
}

inline void ofp_flow_mod::set_idle_timeout(const uint16_t value) {
    idle_timeout = htons(value);
}

inline uint16_t ofp_flow_mod::get_hard_timeout() const {
    return ntohs(hard_timeout);
}

inline void ofp_flow_mod::set_hard_timeout(const uint16_t value) {
    hard_timeout = htons(value);
}

inline uint16_t ofp_flow_mod::get_priority() const {
    return ntohs(priority);
}

inline void ofp_flow_mod::set_priority(const uint16_t value) {
    priority = htons(value);
}

inline uint32_t ofp_flow_mod::get_buffer_id() const {
    return ntohl(buffer_id);
}

inline void ofp_flow_mod::set_buffer_id(const uint32_t value) {
    buffer_id = htonl(value);
}

inline uint32_t ofp_flow_mod::get_out_port() const {
    return ntohl(out_port);
}

inline void ofp_flow_mod::set_out_port(const uint32_t value) {
    out_port = htonl(value);
}

inline uint32_t ofp_flow_mod::get_out_group() const {
    return ntohl(out_group);
}

inline void ofp_flow_mod::set_out_group(const uint32_t value) {
    out_group = htonl(value);
}

inline uint16_t ofp_flow_mod::get_flags() const {
    return ntohs(flags);
}

inline void ofp_flow_mod::set_flags(const uint16_t value) {
    flags = htons(value);
}

// Instruction structure for OFPIT_METER
struct ofp_instruction_meter : public ::ofp_tlv_base {
    // Meter instance.
    uint32_t meter_id = 0;

    inline uint32_t get_meter_id() const;

    inline void set_meter_id(const uint32_t value);

    inline unsigned int get_minimum_length() const { return sizeof(*this);}

};
inline uint32_t ofp_instruction_meter::get_meter_id() const {
    return ntohl(meter_id);
}

inline void ofp_instruction_meter::set_meter_id(const uint32_t value) {
    meter_id = htonl(value);
}

// Common header for all meter bands
///* One of OFPMBT_*. */
struct ofp_meter_band_header : public ::ofp_tlv_base {
    // Rate for this band.
    uint32_t rate = 0;

    // Size of bursts.
    uint32_t burst_size = 0;

    inline uint32_t get_rate() const;

    inline void set_rate(const uint32_t value);

    inline uint32_t get_burst_size() const;

    inline void set_burst_size(const uint32_t value);

};
inline uint32_t ofp_meter_band_header::get_rate() const {
    return ntohl(rate);
}

inline void ofp_meter_band_header::set_rate(const uint32_t value) {
    rate = htonl(value);
}

inline uint32_t ofp_meter_band_header::get_burst_size() const {
    return ntohl(burst_size);
}

inline void ofp_meter_band_header::set_burst_size(const uint32_t value) {
    burst_size = htonl(value);
}
// Group numbering. Groups can use any number up to OFPG_MAX.
//
enum ofp_group {
    OFPG_MAX =    0xffffff00,
    OFPG_ALL =    0xfffffffc,
    OFPG_ANY =    0xffffffff
};
// Group commands
//
enum ofp_group_mod_command {
    OFPGC_ADD =    0,
    OFPGC_MODIFY =    1,
    OFPGC_DELETE =    2
};

// Group setup and teardown (controller -> datapath).
// OFP_ASSERT(sizeof(struct ofp_switch_config) == 16);
struct ofp_group_mod : public ::ofp_message {
    uint16_t command = 0;

    uint8_t group_type = 0;

    uint8_t pad = 0;

    uint32_t group_id = 0;

    struct ofp_bucket buckets[0];

    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint16_t get_command() const;

    inline void set_command(const uint16_t value);

    inline uint32_t get_group_id() const;

    inline void set_group_id(const uint32_t value);

};
inline uint16_t ofp_group_mod::get_command() const {
    return ntohs(command);
}

inline void ofp_group_mod::set_command(const uint16_t value) {
    command = htons(value);
}

inline uint32_t ofp_group_mod::get_group_id() const {
    return ntohl(group_id);
}

inline void ofp_group_mod::set_group_id(const uint32_t value) {
    group_id = htonl(value);
}

//Group types.  Values in the range [128, 255] are reserved for experimental use.
enum ofp_group_type {
    OFPGT_ALL =    0,
    OFPGT_SELECT =    1,
    OFPGT_INDIRECT =    2,
    OFPGT_FF =    3 //Fast failover group.
};

// Meter configuration. OFPT_METER_MOD.
struct ofp_meter_mod : public ::ofp_message {
    // One of OFPMC_*.
    uint16_t command = 0;

    // One of OFPMF_*.
    uint16_t flags = 0;

    // Meter instance.
    uint32_t meter_id = 0;

    // The bands length is
    // inferred from the length field
    // in the header. Types: ofp_meter_band_header
    // ofp_meter_band_drop, ofp_meter_band_dscp_remark, ofp_meter_band_experimenter
    ofp_tlv_base  bands[0];

    inline uint16_t get_command() const;

    inline void set_command(const uint16_t value);

    inline uint16_t get_flags() const;

    inline void set_flags(const uint16_t value);

    inline uint32_t get_meter_id() const;

    inline void set_meter_id(const uint32_t value);

};
inline uint16_t ofp_meter_mod::get_command() const {
    return ntohs(command);
}

inline void ofp_meter_mod::set_command(const uint16_t value) {
    command = htons(value);
}

inline uint16_t ofp_meter_mod::get_flags() const {
    return ntohs(flags);
}

inline void ofp_meter_mod::set_flags(const uint16_t value) {
    flags = htons(value);
}

inline uint32_t ofp_meter_mod::get_meter_id() const {
    return ntohl(meter_id);
}

inline void ofp_meter_mod::set_meter_id(const uint32_t value) {
    meter_id = htonl(value);
}

// Meter numbering. Flow meters can use any number up to OFPM_MAX.
enum ofp_meter {
    OFPM_MAX =  0xffff0000,
    OFPM_SLOWPATH =  0xfffffffd,
    OFPM_CONTROLLER =  0xfffffffe,
    OFPM_ALL =  0xffffffff

};
// Last usable meter.
// Virtual meters.
// Meter for slow datapath, if any.
// Meter for controller connection.
// Represents all meters for stat requests
//                           commands.
// Meter commands
enum ofp_meter_mod_command {
    OFPMC_ADD,
    OFPMC_MODIFY,
    OFPMC_DELETE

};
// New meter.
// Modify specified meter.
// Delete specified meter.
// Meter configuration flags
enum ofp_meter_flags {
    OFPMF_KBPS =  1 << 0,
    OFPMF_PKTPS =  1 << 1,
    OFPMF_BURST =  1 << 2,
    OFPMF_STATS =  1 << 3

};

/* Meter band types */
enum ofp_meter_band_type {
    OFPMBT_DROP = 1, /* Drop packet. */
    OFPMBT_DSCP_REMARK = 2, /* Remark DSCP in the IP header. */
    OFPMBT_EXPERIMENTER = 0xFFFF /* Experimenter meter band. */
};

// Drop packet.
// Remark DSCP in the IP header.
// Experimenter meter band.
// OFPMBT_DROP band - drop packets
// OFPMBT_DROP.
struct ofp_meter_band_drop : public ::ofp_tlv_base {
    // Rate for dropping packets.
    uint32_t rate = 0;

    // Size of bursts.
    uint32_t burst_size = 0;

    uint32_t pad = 0;

    inline uint32_t get_rate() const;

    inline void set_rate(const uint32_t value);

    inline uint32_t get_burst_size() const;

    inline void set_burst_size(const uint32_t value);

};
inline uint32_t ofp_meter_band_drop::get_rate() const {
    return ntohl(rate);
}

inline void ofp_meter_band_drop::set_rate(const uint32_t value) {
    rate = htonl(value);
}

inline uint32_t ofp_meter_band_drop::get_burst_size() const {
    return ntohl(burst_size);
}

inline void ofp_meter_band_drop::set_burst_size(const uint32_t value) {
    burst_size = htonl(value);
}

// OFPMBT_DSCP_REMARK band - Remark DSCP in the IP header
// OFPMBT_DSCP_REMARK.
struct ofp_meter_band_dscp_remark : public ::ofp_tlv_base {
    // Rate for remarking packets.
    uint32_t rate = 0;

    // Size of bursts.
    uint32_t burst_size = 0;

    // Number of precendence level to substract.
    uint8_t prec_level = 0;

    uint8_t pad[3] = {0,0,0};

    inline uint32_t get_rate() const;

    inline void set_rate(const uint32_t value);

    inline uint32_t get_burst_size() const;

    inline void set_burst_size(const uint32_t value);


};
inline uint32_t ofp_meter_band_dscp_remark::get_rate() const {
    return ntohl(rate);
}

inline void ofp_meter_band_dscp_remark::set_rate(const uint32_t value) {
    rate = htonl(value);
}

inline uint32_t ofp_meter_band_dscp_remark::get_burst_size() const {
    return ntohl(burst_size);
}

inline void ofp_meter_band_dscp_remark::set_burst_size(const uint32_t value) {
    burst_size = htonl(value);
}


// OFPMBT_EXPERIMENTER band - Write actions in action set
// One of OFPMBT_*.
struct ofp_meter_band_experimenter : public ::ofp_tlv_base {
    // Rate for this band.
    uint32_t rate = 0;

    // Size of bursts.
    uint32_t burst_size = 0;

    // Experimenter ID which takes the same
    // form as in struct
    // ofp_experimenter_header.
    uint32_t experimenter = 0;

    inline uint32_t get_rate() const;

    inline void set_rate(const uint32_t value);

    inline uint32_t get_burst_size() const;

    inline void set_burst_size(const uint32_t value);

    inline uint32_t get_experimenter() const;

    inline void set_experimenter(const uint32_t value);

};
inline uint32_t ofp_meter_band_experimenter::get_rate() const {
    return ntohl(rate);
}

inline void ofp_meter_band_experimenter::set_rate(const uint32_t value) {
    rate = htonl(value);
}

inline uint32_t ofp_meter_band_experimenter::get_burst_size() const {
    return ntohl(burst_size);
}

inline void ofp_meter_band_experimenter::set_burst_size(const uint32_t value) {
    burst_size = htonl(value);
}

inline uint32_t ofp_meter_band_experimenter::get_experimenter() const {
    return ntohl(experimenter);
}

inline void ofp_meter_band_experimenter::set_experimenter(const uint32_t value) {
    experimenter = htonl(value);
}

// Why was this flow removed?
//
enum ofp_flow_removed_reason {
    OFPRR_IDLE_TIMEOUT =    0,
    OFPRR_HARD_TIMEOUT =    1,
    OFPRR_DELETE =    2,
    OFPRR_GROUP_DELETE =    3 // Group was removed.
};
//Flow removed (datapath -> controller).
//OFP_ASSERT(sizeof(struct ofp_switch_config) == 56);
struct ofp_flow_removed : public ::ofp_message {
    uint64_t cookie = 0;

    uint16_t priority = 0;

    uint8_t reason = 0;

    uint8_t table_id = 0;

    uint32_t duration_sec = 0;

    uint32_t duration_nsec = 0;

    uint16_t idle_timeout = 0;

    uint16_t hard_timeout = 0;

    uint64_t packet_count = 0;

    uint64_t byte_count = 0;

    struct ofp_match match;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint64_t get_cookie() const;

    inline void set_cookie(const uint64_t value);

    inline uint16_t get_priority() const;

    inline void set_priority(const uint16_t value);

    inline uint32_t get_duration_sec() const;

    inline void set_duration_sec(const uint32_t value);

    inline uint32_t get_duration_nsec() const;

    inline void set_duration_nsec(const uint32_t value);

    inline uint16_t get_idle_timeout() const;

    inline void set_idle_timeout(const uint16_t value);

    inline uint16_t get_hard_timeout() const;

    inline void set_hard_timeout(const uint16_t value);

    inline uint64_t get_packet_count() const;

    inline void set_packet_count(const uint64_t value);

    inline uint64_t get_byte_count() const;

    inline void set_byte_count(const uint64_t value);

};
inline uint64_t ofp_flow_removed::get_cookie() const {
    return ofp_message::ntohll(cookie);
}

inline void ofp_flow_removed::set_cookie(const uint64_t value) {
    cookie = ofp_message::htonll(value);
}

inline uint16_t ofp_flow_removed::get_priority() const {
    return ntohs(priority);
}

inline void ofp_flow_removed::set_priority(const uint16_t value) {
    priority = htons(value);
}

inline uint32_t ofp_flow_removed::get_duration_sec() const {
    return ntohl(duration_sec);
}

inline void ofp_flow_removed::set_duration_sec(const uint32_t value) {
    duration_sec = htonl(value);
}

inline uint32_t ofp_flow_removed::get_duration_nsec() const {
    return ntohl(duration_nsec);
}

inline void ofp_flow_removed::set_duration_nsec(const uint32_t value) {
    duration_nsec = htonl(value);
}

inline uint16_t ofp_flow_removed::get_idle_timeout() const {
    return ntohs(idle_timeout);
}

inline void ofp_flow_removed::set_idle_timeout(const uint16_t value) {
    idle_timeout = htons(value);
}

inline uint16_t ofp_flow_removed::get_hard_timeout() const {
    return ntohs(hard_timeout);
}

inline void ofp_flow_removed::set_hard_timeout(const uint16_t value) {
    hard_timeout = htons(value);
}

inline uint64_t ofp_flow_removed::get_packet_count() const {
    return ofp_message::ntohll(packet_count);
}

inline void ofp_flow_removed::set_packet_count(const uint64_t value) {
    packet_count = ofp_message::htonll(value);
}

inline uint64_t ofp_flow_removed::get_byte_count() const {
    return ofp_message::ntohll(byte_count);
}

inline void ofp_flow_removed::set_byte_count(const uint64_t value) {
    byte_count = ofp_message::htonll(value);
}

// More requests to follow.
// Description of this OpenFlow switch.
//    * The request body is empty.
//    * The reply body is struct ofp_desc.
// Individual flow statistics.
//    * The request body is struct ofp_flow_multipart_request.
//    * The reply body is an array of struct ofp_flow_stats.
// Aggregate flow statistics.
//    * The request body is struct ofp_aggregate_stats_request.
//    * The reply body is struct ofp_aggregate_stats_reply.
// Flow table statistics.
//    * The request body is empty.
//    * The reply body is an array of struct ofp_table_stats.
// Port statistics.
//    * The request body is struct ofp_port_stats_request.
//    * The reply body is an array of struct ofp_port_stats.
// Queue statistics for a port
//    * The request body is struct ofp_queue_stats_request.
//    * The reply body is an array of struct ofp_queue_stats
// Group counter statistics.
//    * The request body is struct ofp_group_stats_request.
//    * The reply is an array of struct ofp_group_stats.
// Group description statistics.
//    * The request body is empty.
//    * The reply body is an array of struct ofp_group_desc_stats.
// Group features.
//    * The request body is empty.
//    * The reply body is struct ofp_group_features_stats.
// Meter statistics.
//     * The request body is struct ofp_meter_multipart_requests.
//     * The reply body is an array of struct ofp_meter_stats.
// Meter configuration.
//    * The request body is struct ofp_meter_multipart_requests.
//    * The reply body is an array of struct ofp_meter_config.
// Meter features.
//    * The request body is empty.
//    * The reply body is struct ofp_meter_features.
// Table features.
//    * The request body is either empty or contains an array of
//    * struct ofp_table_features containing the controller�s
//    * desired view of the switch. If the switch is unable to
//    * set the specified view an error is returned.
//    * The reply body is an array of struct ofp_table_features.
// Port description.
//    * The request body is empty.
//    * The reply body is an array of struct ofp_port.
// Experimenter extension.
//    * The request and reply bodies begin with
//    * struct ofp_experimenter_stats_header.
//    * The request and reply bodies are otherwise experimenter-defined.
enum ofp_stats_types {
    /* Description of this OpenFlow switch.
    * The request body is empty.
     * The reply body is struct ofp_desc_stats. */
    OFPST_DESC = 0,
    /* Individual flow statistics.
     * The request body is struct ofp_flow_stats_request.
    * The reply body is an array of struct ofp_flow_stats. */
    OFPST_FLOW = 1,
    /* Aggregate flow statistics.
    * The request body is struct ofp_aggregate_stats_request.
    * The reply body is struct ofp_aggregate_stats_reply. */
    OFPST_AGGREGATE = 2,
    /* Flow table statistics.
    * The request body is empty.
    * The reply body is an array of struct ofp_table_stats. */
    OFPST_TABLE = 3,
    /* Port statistics.
    * The request body is struct ofp_port_stats_request.
    * The reply body is an array of struct ofp_port_stats. */
    OFPST_PORT = 4,
    /* Queue statistics for a port
    * The request body is struct ofp_queue_stats_request.
    * The reply body is an array of struct ofp_queue_stats */
    OFPST_QUEUE = 5,
    /* Group counter statistics.
    * The request body is struct ofp_group_stats_request.
    * The reply is an array of struct ofp_group_stats. */
    OFPST_GROUP = 6,
    /* Group description statistics.
    * The request body is empty.
    * The reply body is an array of struct ofp_group_desc_stats. */
    OFPST_GROUP_DESC = 7,
    /* Group features.
    * The request body is empty.
    * The reply body is struct ofp_group_features_stats. */
    OFPST_GROUP_FEATURES = 8,
    /* Meter statistics.
     * The request body is struct ofp_meter_multipart_requests.
     * The reply body is an array of struct ofp_meter_stats. */
    OFPMP_METER = 9,
    /* Meter configuration.
    * The request body is struct ofp_meter_multipart_requests.
    * The reply body is an array of struct ofp_meter_config. */
    OFPMP_METER_CONFIG = 10,
    /* Meter features.
    * The request body is empty.
    * The reply body is struct ofp_meter_features. */
    OFPMP_METER_FEATURES = 11,
    /* Table features.
    * The request body is either empty or contains an array of
    * struct ofp_table_features containing the controller�s
    * desired view of the switch. If the switch is unable to
    * set the specified view an error is returned.
    * The reply body is an array of struct ofp_table_features. */
    OFPMP_TABLE_FEATURES = 12,
    /* Port description.
    * The request body is empty.
    * The reply body is an array of struct ofp_port. */
    OFPMP_PORT_DESC = 13,
    /* Experimenter extension.
    * The request and reply bodies begin with
    * struct ofp_experimenter_stats_header.
    * The request and reply bodies are otherwise experimenter-defined. */
    OFPST_EXPERIMENTER = 0xffff
};

struct ofp_port_list_stats : public ::ofp_stats_msg {
    ofp_phy_port body[1];

    inline unsigned int get_minimum_length() const { return sizeof(*this);};

};

// One of OFPTFPT_NEXT_TABLES,  OFPTFPT_NEXT_TABLES_MISS.
struct ofp_table_feature_prop_header : public ::ofp_tlv_base {
    uint32_t pad = 0;

};
// Body for ofp_multipart_request of type OFPMP_TABLE_FEATURES./
//* Body of reply to OFPMP_TABLE_FEATURES request.
struct ofp_table_features_body {
    // Length is padded to 64 bits.
    uint16_t stat_length = 0;

    // Identifier of table. Lower numbered tables
    //                         are consulted first.
    uint8_t table_id = 0;

    // Align to 64-bits.
    uint8_t pad[5] = {0,0,0,0,0};

    char name[OFP_MAX_TABLE_NAME_LEN];

    // Bits of metadata table can match.
    uint64_t metadata_match = 0;

    // Bits of metadata table can write.
    uint64_t metadata_write = 0;

    // Bitmap of OFPTC_* values
    uint32_t config = 0;

    // Max number of entries supported.
    uint32_t max_entries = 0;

    // ofp_table_feature_prop_header
    ::ofp_tlv_base properties[0];

    inline uint16_t get_stat_length() const { return ntohs(stat_length);}
    inline void set_stat_length(const uint16_t param) { stat_length = htons(param);}

    inline uint64_t get_metadata_match() const { return ofp_message::ntohll(metadata_match);}

    inline void set_metadata_match(const uint64_t value) { metadata_match = ofp_message::htonll(value);}

    inline uint64_t get_metadata_write() const { return ofp_message::ntohll(metadata_write);}

    inline void set_metadata_write(const uint64_t value) { metadata_write = ofp_message::htonll(value);}

    inline uint32_t get_config() const { return ntohl(config);}

    inline void set_config(const uint32_t value) { config = htonl(value);}

    inline uint32_t get_max_entries() const { return ntohl(max_entries);}

    inline void set_max_entries(const uint32_t value) { max_entries = htonl(value);}

};

struct ofp_table_features_stats : public ::ofp_stats_msg {
    ofp_table_features_body body[1];

    inline unsigned int get_minimum_length() const { return sizeof(*this);};

};
// Table Feature property types.
//* Low order bit cleared indicates a property for a regular Flow Entry.
//* Low order bit set indicates a property for the Table-Miss Flow Entry.
enum ofp_table_feature_prop_type {
    OFPTFPT_INSTRUCTIONS =  0,
    OFPTFPT_INSTRUCTIONS_MISS =  1,
    OFPTFPT_NEXT_TABLES =  2,
    OFPTFPT_NEXT_TABLES_MISS =  3,
    OFPTFPT_WRITE_ACTIONS =  4,
    OFPTFPT_WRITE_ACTIONS_MISS =  5,
    OFPTFPT_APPLY_ACTIONS =  6,
    OFPTFPT_APPLY_ACTIONS_MISS =  7,
    OFPTFPT_MATCH =  8,
    OFPTFPT_WILDCARDS =  10,
    OFPTFPT_WRITE_SETFIELD =  12,
    OFPTFPT_WRITE_SETFIELD_MISS =  13,
    OFPTFPT_APPLY_SETFIELD =  14,
    OFPTFPT_APPLY_SETFIELD_MISS =  15,
    OFPTFPT_EXPERIMENTER =  0xFFFE,
    OFPTFPT_EXPERIMENTER_MISS =  0xFFFF

};
// Instructions property.
// Instructions for table-miss.
// Next Table property.
// Next Table for table-miss.
// Write Actions property.
// Write Actions for table-miss.
// Apply Actions property.
// Apply Actions for table-miss.
// Match property.
// Wildcards property.
// Write Set-Field property.
// Write Set-Field for table-miss.
// Apply Set-Field property.
// Apply Set-Field for table-miss.
// Experimenter property.
// Experimenter for table-miss.
// Instructions property
// One of OFPTFPT_INSTRUCTIONS, OFPTFPT_INSTRUCTIONS_MISS.
struct ofp_table_feature_prop_instructions : public ::ofp_tlv_base {
    // List of instructions
    ::ofp_tlv_base instruction_ids[0];

};
// One of OFPTFPT_NEXT_TABLES, OFPTFPT_NEXT_TABLES_MISS.
struct ofp_table_feature_prop_next_tables : public ::ofp_tlv_base {
    uint8_t next_table_ids[0];

};
// Actions property
// One of OFPTFPT_WRITE_ACTIONS,
//                      OFPTFPT_WRITE_ACTIONS_MISS,
//                      OFPTFPT_APPLY_ACTIONS,
//                      OFPTFPT_APPLY_ACTIONS_MISS.
struct ofp_table_feature_prop_actions : public ::ofp_tlv_base {
    // List of actions
    ::ofp_tlv_base action_ids[0];

};
// Match, Wildcard or Set-Field property
// One of OFPTFPT_MATCH,
//                      OFPTFPT_WILDCARDS,
//                      OFPTFPT_WRITE_SETFIELD,
//                      OFPTFPT_WRITE_SETFIELD_MISS,
//                      OFPTFPT_APPLY_SETFIELD,
//                      OFPTFPT_APPLY_SETFIELD_MISS.
struct ofp_table_feature_prop_oxm : public ::ofp_tlv_base {
    // Array of OXM headers
    ::oxm_fields_t oxm_ids[0];

};

// Body of reply to OFPMP_PORT_STATS request. If a counter is unsupported, set
//* the field to all ones.
struct ofp_port_stats_body {
    uint32_t port_no = 0;

    uint32_t pad = 0;

    uint64_t rx_packets = 0;

    uint64_t tx_packets = 0;

    uint64_t rx_bytes = 0;

    uint64_t tx_bytes = 0;

    uint64_t rx_dropped = 0;

    uint64_t tx_dropped = 0;

    uint64_t rx_errors = 0;

    uint64_t tx_errors = 0;

    uint64_t rx_frame_err = 0;

    uint64_t rx_over_err = 0;

    uint64_t rx_crc_err = 0;

    uint64_t collisions = 0;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint32_t get_port_no() const;

    inline void set_port_no(const uint32_t value);

    inline uint64_t get_rx_packets() const;

    inline void set_rx_packets(const uint64_t value);

    inline uint64_t get_tx_packets() const;

    inline void set_tx_packets(const uint64_t value);

    inline uint64_t get_rx_bytes() const;

    inline void set_rx_bytes(const uint64_t value);

    inline uint64_t get_tx_bytes() const;

    inline void set_tx_bytes(const uint64_t value);

    inline uint64_t get_rx_dropped() const;

    inline void set_rx_dropped(const uint64_t value);

    inline uint64_t get_tx_dropped() const;

    inline void set_tx_dropped(const uint64_t value);

    inline uint64_t get_rx_errors() const;

    inline void set_rx_errors(const uint64_t value);

    inline uint64_t get_tx_errors() const;

    inline void set_tx_errors(const uint64_t value);

    inline uint64_t get_rx_frame_err() const;

    inline void set_rx_frame_err(const uint64_t value);

    inline uint64_t get_rx_over_err() const;

    inline void set_rx_over_err(const uint64_t value);

    inline uint64_t get_rx_crc_err() const;

    inline void set_rx_crc_err(const uint64_t value);

    inline uint64_t get_collisions() const;

    inline void set_collisions(const uint64_t value);

    // Time port has been alive in seconds.
    uint32_t duration_sec = 0;

    // Time port has been alive in nanoseconds beyond
    //                              duration_sec.
    uint32_t duration_nsec = 0;

    inline uint32_t get_duration_sec() const { return ntohl(duration_sec);};

    inline void set_duration_sec(const uint32_t value) { duration_sec = htonl(value);};

    inline uint32_t get_duration_nsec() const { return ntohl(duration_nsec);};

    inline void set_duration_nsec(const uint32_t value) { duration_nsec = htonl(value);};

};
inline uint32_t ofp_port_stats_body::get_port_no() const {
    return ntohl(port_no);
}

inline void ofp_port_stats_body::set_port_no(const uint32_t value) {
    port_no = htonl(value);
}

inline uint64_t ofp_port_stats_body::get_rx_packets() const {
    return ofp_message::ntohll(rx_packets);
}

inline void ofp_port_stats_body::set_rx_packets(const uint64_t value) {
    rx_packets = ofp_message::htonll(value);
}

inline uint64_t ofp_port_stats_body::get_tx_packets() const {
    return ofp_message::ntohll(tx_packets);
}

inline void ofp_port_stats_body::set_tx_packets(const uint64_t value) {
    tx_packets = ofp_message::htonll(value);
}

inline uint64_t ofp_port_stats_body::get_rx_bytes() const {
    return ofp_message::ntohll(rx_bytes);
}

inline void ofp_port_stats_body::set_rx_bytes(const uint64_t value) {
    rx_bytes = ofp_message::htonll(value);
}

inline uint64_t ofp_port_stats_body::get_tx_bytes() const {
    return ofp_message::ntohll(tx_bytes);
}

inline void ofp_port_stats_body::set_tx_bytes(const uint64_t value) {
    tx_bytes = ofp_message::htonll(value);
}

inline uint64_t ofp_port_stats_body::get_rx_dropped() const {
    return ofp_message::ntohll(rx_dropped);
}

inline void ofp_port_stats_body::set_rx_dropped(const uint64_t value) {
    rx_dropped = ofp_message::htonll(value);
}

inline uint64_t ofp_port_stats_body::get_tx_dropped() const {
    return ofp_message::ntohll(tx_dropped);
}

inline void ofp_port_stats_body::set_tx_dropped(const uint64_t value) {
    tx_dropped = ofp_message::htonll(value);
}

inline uint64_t ofp_port_stats_body::get_rx_errors() const {
    return ofp_message::ntohll(rx_errors);
}

inline void ofp_port_stats_body::set_rx_errors(const uint64_t value) {
    rx_errors = ofp_message::htonll(value);
}

inline uint64_t ofp_port_stats_body::get_tx_errors() const {
    return ofp_message::ntohll(tx_errors);
}

inline void ofp_port_stats_body::set_tx_errors(const uint64_t value) {
    tx_errors = ofp_message::htonll(value);
}

inline uint64_t ofp_port_stats_body::get_rx_frame_err() const {
    return ofp_message::ntohll(rx_frame_err);
}

inline void ofp_port_stats_body::set_rx_frame_err(const uint64_t value) {
    rx_frame_err = ofp_message::htonll(value);
}

inline uint64_t ofp_port_stats_body::get_rx_over_err() const {
    return ofp_message::ntohll(rx_over_err);
}

inline void ofp_port_stats_body::set_rx_over_err(const uint64_t value) {
    rx_over_err = ofp_message::htonll(value);
}

inline uint64_t ofp_port_stats_body::get_rx_crc_err() const {
    return ofp_message::ntohll(rx_crc_err);
}

inline void ofp_port_stats_body::set_rx_crc_err(const uint64_t value) {
    rx_crc_err = ofp_message::htonll(value);
}

inline uint64_t ofp_port_stats_body::get_collisions() const {
    return ofp_message::ntohll(collisions);
}

inline void ofp_port_stats_body::set_collisions(const uint64_t value) {
    collisions = ofp_message::htonll(value);
}

struct ofp_queue_stats_body  {
    uint32_t port_no = 0;

    uint32_t queue_id = 0;

    uint64_t tx_bytes = 0;

    uint64_t tx_packets = 0;

    uint64_t tx_errors = 0;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint32_t get_port_no() const;

    inline void set_port_no(const uint32_t value);

    inline uint32_t get_queue_id() const;

    inline void set_queue_id(const uint32_t value);

    inline uint64_t get_tx_bytes() const;

    inline void set_tx_bytes(const uint64_t value);

    inline uint64_t get_tx_packets() const;

    inline void set_tx_packets(const uint64_t value);

    inline uint64_t get_tx_errors() const;

    inline void set_tx_errors(const uint64_t value);

    // Time queue has been alive in seconds.
    uint32_t duration_sec = 0;

    // Time queue has been alive in nanoseconds beyond
    //                                duration_sec.
    uint32_t duration_nsec = 0;

    inline uint32_t get_duration_sec() const { return ntohl(duration_sec);};

    inline void set_duration_sec(const uint32_t value) { duration_sec = htonl(value);};

    inline uint32_t get_duration_nsec() const { return ntohl(duration_nsec);};

    inline void set_duration_nsec(const uint32_t value) { duration_nsec = htonl(value);};

};
inline uint32_t ofp_queue_stats_body::get_port_no() const {
    return ntohl(port_no);
}

inline void ofp_queue_stats_body::set_port_no(const uint32_t value) {
    port_no = htonl(value);
}

inline uint32_t ofp_queue_stats_body::get_queue_id() const {
    return ntohl(queue_id);
}

inline void ofp_queue_stats_body::set_queue_id(const uint32_t value) {
    queue_id = htonl(value);
}

inline uint64_t ofp_queue_stats_body::get_tx_bytes() const {
    return ofp_message::ntohll(tx_bytes);
}

inline void ofp_queue_stats_body::set_tx_bytes(const uint64_t value) {
    tx_bytes = ofp_message::htonll(value);
}

inline uint64_t ofp_queue_stats_body::get_tx_packets() const {
    return ofp_message::ntohll(tx_packets);
}

inline void ofp_queue_stats_body::set_tx_packets(const uint64_t value) {
    tx_packets = ofp_message::htonll(value);
}

inline uint64_t ofp_queue_stats_body::get_tx_errors() const {
    return ofp_message::ntohll(tx_errors);
}

inline void ofp_queue_stats_body::set_tx_errors(const uint64_t value) {
    tx_errors = ofp_message::htonll(value);
}

struct ofp_queue_stats : public ::ofp_stats_msg {
    ofp_queue_stats_body body[1];

    inline unsigned int get_minimum_length() const { return sizeof(*this);};

};
// Used in group stats replies.
struct ofp_bucket_counter {
    uint64_t packet_count = 0;

    uint64_t byte_count = 0;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};

    inline uint64_t get_packet_count() const;

    inline void set_packet_count(const uint64_t value);

    inline uint64_t get_byte_count() const;

    inline void set_byte_count(const uint64_t value);

};
inline uint64_t ofp_bucket_counter::get_packet_count() const {
    return ofp_message::ntohll(packet_count);
}

inline void ofp_bucket_counter::set_packet_count(const uint64_t value) {
    packet_count = ofp_message::htonll(value);
}

inline uint64_t ofp_bucket_counter::get_byte_count() const {
    return ofp_message::ntohll(byte_count);
}

inline void ofp_bucket_counter::set_byte_count(const uint64_t value) {
    byte_count = ofp_message::htonll(value);
}

// Body of reply to OFPMP_GROUP request.
// Note that bucket_stats in ofp_group_stats_body is shadowed intentionally
// Parser must explicitly use current one
struct ofp_group_stats_body {
    uint16_t stats_length = 0;

    uint16_t pad = 0;

    uint32_t group_id = 0;

    uint32_t ref_count = 0;

    uint32_t pad2 = 0;

    uint64_t packet_count = 0;

    uint64_t byte_count = 0;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};

    inline uint16_t get_stats_length() const;

    inline void set_stats_length(const uint16_t value);

    inline uint32_t get_group_id() const;

    inline void set_group_id(const uint32_t value);

    inline uint32_t get_ref_count() const;

    inline void set_ref_count(const uint32_t value);

    inline uint64_t get_packet_count() const;

    inline void set_packet_count(const uint64_t value);

    inline uint64_t get_byte_count() const;

    inline void set_byte_count(const uint64_t value);

    // Time group has been alive in seconds.
    uint32_t duration_sec = 0;

    // Time group has been alive in nanoseconds beyond
    //                                duration_sec.
    uint32_t duration_nsec = 0;

    struct ofp_bucket_counter bucket_stats[0];

    inline uint32_t get_duration_sec() const { return ntohl(duration_sec);};

    inline void set_duration_sec(const uint32_t value) { duration_sec = htonl(value);};

    inline uint32_t get_duration_nsec() const { return ntohl(duration_nsec);};

    inline void set_duration_nsec(const uint32_t value) { duration_nsec = htonl(value);};

};
inline uint16_t ofp_group_stats_body::get_stats_length() const {
    return ntohs(stats_length);
}

inline void ofp_group_stats_body::set_stats_length(const uint16_t value) {
    stats_length = htons(value);
}

inline uint32_t ofp_group_stats_body::get_group_id() const {
    return ntohl(group_id);
}

inline void ofp_group_stats_body::set_group_id(const uint32_t value) {
    group_id = htonl(value);
}

inline uint32_t ofp_group_stats_body::get_ref_count() const {
    return ntohl(ref_count);
}

inline void ofp_group_stats_body::set_ref_count(const uint32_t value) {
    ref_count = htonl(value);
}

inline uint64_t ofp_group_stats_body::get_packet_count() const {
    return ofp_message::ntohll(packet_count);
}

inline void ofp_group_stats_body::set_packet_count(const uint64_t value) {
    packet_count = ofp_message::htonll(value);
}

inline uint64_t ofp_group_stats_body::get_byte_count() const {
    return ofp_message::ntohll(byte_count);
}

inline void ofp_group_stats_body::set_byte_count(const uint64_t value) {
    byte_count = ofp_message::htonll(value);
}

struct ofp_group_stats : public ::ofp_stats_msg {
    ofp_group_stats_body body[1];

    inline unsigned int get_minimum_length() const { return sizeof(*this);};

    friend std::ostream& operator<<(std::ostream &out, const ofp_group_stats &obj);
};
// Group configuration flags
enum ofp_group_capabilities {
    OFPGFC_SELECT_WEIGHT =  1 << 0,
    OFPGFC_SELECT_LIVENESS =  1 << 1,
    OFPGFC_CHAINING =  1 << 2,
    OFPGFC_CHAINING_CHECKS =  1 << 3

};
// Support weight for select groups
// Support liveness for select groups
// Support chaining groups
// Check chaining for loops and delete
// Body of OFPMP_METER and OFPMP_METER_CONFIG requests.
struct ofp_meter_multipart_request : public ::ofp_stats_msg {
    // Meter instance, or OFPM_ALL.
    uint32_t meter_id = 0;

    // Align to 64 bits.
    uint32_t pad = 0;

    inline uint32_t get_meter_id() const;

    inline void set_meter_id(const uint32_t value);

};
inline uint32_t ofp_meter_multipart_request::get_meter_id() const {
    return ntohl(meter_id);
}

inline void ofp_meter_multipart_request::set_meter_id(const uint32_t value) {
    meter_id = htonl(value);
}

// Statistics for each meter band
struct ofp_meter_band_stats {
    // Number of packets in band.
    uint64_t packet_band_count = 0;

    // Number of bytes in band.
    uint64_t byte_band_count = 0;

    inline uint64_t get_packet_band_count() const;

    inline void set_packet_band_count(const uint64_t value);

    inline uint64_t get_byte_band_count() const;

    inline void set_byte_band_count(const uint64_t value);

};
inline uint64_t ofp_meter_band_stats::get_packet_band_count() const {
    return ofp_message::ntohll(packet_band_count);
}

inline void ofp_meter_band_stats::set_packet_band_count(const uint64_t value) {
    packet_band_count = ofp_message::htonll(value);
}

inline uint64_t ofp_meter_band_stats::get_byte_band_count() const {
    return ofp_message::ntohll(byte_band_count);
}

inline void ofp_meter_band_stats::set_byte_band_count(const uint64_t value) {
    byte_band_count = ofp_message::htonll(value);
}

// Body of reply to OFPMP_METER request. Meter statistics.
struct ofp_meter_stats_body {
    // Meter instance.
    uint32_t meter_id = 0;

    // Length in bytes of this stats.
    uint16_t meter_stat_len = 0;

    uint8_t pad[6] = {0,0,0,0,0,0};

    // Number of flows bound to meter.
    uint32_t flow_count = 0;

    // Number of packets in input.
    uint64_t packet_in_count = 0;

    // Number of bytes in input.
    uint64_t byte_in_count = 0;

    // Time meter has been alive in seconds.
    uint32_t duration_sec = 0;

    // Time meter has been alive in nanoseconds beyond
    //                               duration_sec.
    uint32_t duration_nsec = 0;

    // The band_stats length is
    //                                                  inferred from the length field.
    struct ofp_meter_band_stats band_stats[0];

    inline uint32_t get_meter_id() const;

    inline void set_meter_id(const uint32_t value);

    inline uint16_t get_len() const;

    inline void set_len(const uint16_t value);

    inline uint32_t get_flow_count() const;

    inline void set_flow_count(const uint32_t value);

    inline uint64_t get_packet_in_count() const;

    inline void set_packet_in_count(const uint64_t value);

    inline uint64_t get_byte_in_count() const;

    inline void set_byte_in_count(const uint64_t value);

    inline uint32_t get_duration_sec() const;

    inline void set_duration_sec(const uint32_t value);

    inline uint32_t get_duration_nsec() const;

    inline void set_duration_nsec(const uint32_t value);

};

inline uint32_t ofp_meter_stats_body::get_meter_id() const {
    return ntohl(meter_id);
}

inline void ofp_meter_stats_body::set_meter_id(const uint32_t value) {
    meter_id = htonl(value);
}

inline uint16_t ofp_meter_stats_body::get_len() const {
    return ntohs(meter_stat_len);
}

inline void ofp_meter_stats_body::set_len(const uint16_t value) {
    meter_stat_len = htons(value);
}

inline uint32_t ofp_meter_stats_body::get_flow_count() const {
    return ntohl(flow_count);
}

inline void ofp_meter_stats_body::set_flow_count(const uint32_t value) {
    flow_count = htonl(value);
}

inline uint64_t ofp_meter_stats_body::get_packet_in_count() const {
    return ofp_message::ntohll(packet_in_count);
}

inline void ofp_meter_stats_body::set_packet_in_count(const uint64_t value) {
    packet_in_count = ofp_message::htonll(value);
}

inline uint64_t ofp_meter_stats_body::get_byte_in_count() const {
    return ofp_message::ntohll(byte_in_count);
}

inline void ofp_meter_stats_body::set_byte_in_count(const uint64_t value) {
    byte_in_count = ofp_message::htonll(value);
}

inline uint32_t ofp_meter_stats_body::get_duration_sec() const {
    return ntohl(duration_sec);
}

inline void ofp_meter_stats_body::set_duration_sec(const uint32_t value) {
    duration_sec = htonl(value);
}

inline uint32_t ofp_meter_stats_body::get_duration_nsec() const {
    return ntohl(duration_nsec);
}

inline void ofp_meter_stats_body::set_duration_nsec(const uint32_t value) {
    duration_nsec = htonl(value);
}

struct ofp_meter_stats : public ::ofp_stats_msg {
    ofp_meter_stats_body body[1];

    inline unsigned int get_minimum_length() const { return sizeof(*this);};

};
// Body of reply to OFPMP_METER_CONFIG request. Meter configuration.
struct ofp_meter_config  : public ::ofp_stats_msg {
    // Length of this entry.
    uint16_t length = 0;

    // All OFPMC_* that apply.
    uint16_t flags = 0;

    // Meter instance.
    uint32_t meter_id = 0;

    // The bands length is
    //                                              inferred from the length field.
    struct ofp_meter_band_header bands[0];

    inline uint16_t get_length() const;

    inline void set_length(const uint16_t value);

    inline uint16_t get_flags() const;

    inline void set_flags(const uint16_t value);

    inline uint32_t get_meter_id() const;

    inline void set_meter_id(const uint32_t value);

};
inline uint16_t ofp_meter_config::get_length() const {
    return ntohs(length);
}

inline void ofp_meter_config::set_length(const uint16_t value) {
    length = htons(value);
}

inline uint16_t ofp_meter_config::get_flags() const {
    return ntohs(flags);
}

inline void ofp_meter_config::set_flags(const uint16_t value) {
    flags = htons(value);
}

inline uint32_t ofp_meter_config::get_meter_id() const {
    return ntohl(meter_id);
}

inline void ofp_meter_config::set_meter_id(const uint32_t value) {
    meter_id = htonl(value);
}

// Body of reply to OFPMP_METER_FEATURES request. Meter features.
struct ofp_meter_features  : public ::ofp_stats_msg {
    // Maximum number of meters.
    uint32_t max_meter = 0;

    // Bitmaps of OFPMBT_* values supported.
    uint32_t band_types = 0;

    // Bitmaps of "ofp_meter_flags".
    uint32_t capabilities = 0;

    // Maximum bands per meters
    uint8_t max_bands = 0;

    // Maximum color value
    uint8_t max_color = 0;

    uint16_t pad = 0;

    inline uint32_t get_max_meter() const;

    inline void set_max_meter(const uint32_t value);

    inline uint32_t get_band_types() const;

    inline void set_band_types(const uint32_t value);

    inline uint32_t get_capabilities() const;

    inline void set_capabilities(const uint32_t value);

};
inline uint32_t ofp_meter_features::get_max_meter() const {
    return ntohl(max_meter);
}

inline void ofp_meter_features::set_max_meter(const uint32_t value) {
    max_meter = htonl(value);
}

inline uint32_t ofp_meter_features::get_band_types() const {
    return ntohl(band_types);
}

inline void ofp_meter_features::set_band_types(const uint32_t value) {
    band_types = htonl(value);
}

inline uint32_t ofp_meter_features::get_capabilities() const {
    return ntohl(capabilities);
}

inline void ofp_meter_features::set_capabilities(const uint32_t value) {
    capabilities = htonl(value);
}


// uint8_t data[0];
// Packet data. The length is inferred
//	from the length field in the header.
//	(Only meaningful if buffer_id == -1.)
// Asynchronous message configuration.
// OFPT_GET_ASYNC_REPLY or OFPT_SET_ASYNC.
struct ofp_async_config : public ::ofp_message {
    // Bitmasks of OFPR_* values.
    uint32_t packet_in_mask[2];

    // Bitmasks of OFPPR_* values.
    uint32_t port_status_mask[2];

    // Bitmasks of OFPRR_* values.
    uint32_t flow_removed_mask[2];

};

//Send packet (controller -> datapath).
//OFP_ASSERT(sizeof(struct ofp_switch_config) == 24);
struct ofp_packet_out : public ::ofp_message {
    uint32_t buffer_id = 0;

    uint32_t in_port = 0;

    uint16_t actions_len = 0;

    uint8_t pad[6] = {0,0,0,0,0,0};

    ::ofp_tlv_base actions[0]; // struct ofp_action_header (1.2) 64bit aligned
                               // uint8_t data_[0];
    inline unsigned int get_minimum_length() const { return sizeof(*this);};

    inline uint32_t get_buffer_id() const;

    inline void set_buffer_id(const uint32_t value);

    inline uint32_t get_in_port() const;

    inline void set_in_port(const uint32_t value);

    inline uint16_t get_actions_len() const;

    inline void set_actions_len(const uint16_t value);

};

inline uint32_t ofp_packet_out::get_buffer_id() const {
    return ntohl(buffer_id);
}

inline void ofp_packet_out::set_buffer_id(const uint32_t value) {
    buffer_id = htonl(value);
}

inline uint32_t ofp_packet_out::get_in_port() const {
    return ntohl(in_port);
}

inline void ofp_packet_out::set_in_port(const uint32_t value) {
    in_port = htonl(value);
}

inline uint16_t ofp_packet_out::get_actions_len() const {
    return ntohs(actions_len);
}

inline void ofp_packet_out::set_actions_len(const uint16_t value) {
    actions_len = htons(value);
}

// Why is this packet being sent to the controller?
//
enum ofp_packet_in_reason {
    OFPR_NO_MATCH =    0,
    OFPR_ACTION =    1,
    OFPR_INVALID_TTL =    2 //Packet has invalid TTL


};

#define OFP_NO_BUFFER 0xffffffff
// Packet received on port (datapath -> controller).
struct ofp_packet_in : public ::ofp_message {
    // ID assigned by datapath.
    uint32_t buffer_id = 0;

    // Full length of frame.
    uint16_t total_len = 0;

    // Reason packet is being sent (one of OFPR_*)
    uint8_t reason = 0;

    // ID of the table that was looked up
    uint8_t table_id = 0;

    // Cookie of the flow entry that was looked up.
    uint64_t cookie = 0;

    // Packet metadata. Variable size.
    ofp_match match;

    // Followed by:
    //	* -Exactly 2 all-zero padding bytes,then
    //	* -An Ethernetframe whose length is inferred from header.length.
    //	* The padding bytes preceding the Ethernet frame ensure that the IP
    //	* header (if any) following the Ethernet header is 32-bit aligned.
    //
    //uint8_t pad[2]; /* Align to 64 bit + 16 bit */
    //uint8_t data[0]; /* Ethernet frame */
    inline uint32_t get_buffer_id() const { return ntohl(buffer_id);};

    inline void set_buffer_id(const uint32_t value) { buffer_id = htonl(value);};

    inline uint16_t get_total_len() const { return ntohs(total_len);};

    inline void set_total_len(const uint16_t value) { total_len = htons(value);};

    inline uint64_t get_cookie() const { return ofp_message::ntohll(cookie);};

    inline void set_cookie(const uint64_t value) { cookie = ofp_message::htonll(value);};

};

/* Values for 'type' in ofp_error_message.  These values are immutable: they
* will not change in future versions of the protocol (although new values may
* be added). */
enum ofp_error_type {
    OFPET_HELLO_FAILED = 0,          /* Hello protocol failed. */
    OFPET_BAD_REQUEST = 1,           /* Request was not understood. */
    OFPET_BAD_ACTION = 2,            /* Error in action description. */
    OFPET_BAD_INSTRUCTION = 3,       /* Error in instruction list. */
    OFPET_BAD_MATCH = 4,             /* Error in match. */
    OFPET_FLOW_MOD_FAILED = 5,       /* Problem modifying flow entry. */
    OFPET_GROUP_MOD_FAILED = 6,      /* Problem modifying group entry. */
    OFPET_PORT_MOD_FAILED = 7,       /* Port mod request failed. */
    OFPET_TABLE_MOD_FAILED = 8,      /* Table mod request failed. */
    OFPET_QUEUE_OP_FAILED = 9,       /* Queue operation failed. */
    OFPET_SWITCH_CONFIG_FAILED = 10, /* Switch config request failed. */
    OFPET_ROLE_REQUEST_FAILED = 11,  /* Controller Role request failed. */
    OFPET_METER_MOD_FAILED = 12,     /* Error in meter. */
    OFPET_TABLE_FEATURES_FAILED = 13, /* Setting table features failed. */
    OFPET_EXPERIMENTER = 0xffff      /* Experimenter error messages. */
};
//  ofp_error_msg 'code' values for OFPET_HELLO_FAILED.  'data' contains an
//  ASCII text string that may give failure details.
//
enum ofp_hello_failed_code {
    OFPHFC_INCOMPATIBLE =    0,
    OFPHFC_EPERM =    1
};

/* ofp_error_msg 'code' values for OFPET_BAD_REQUEST.  'data' contains at least
* the first 64 bytes of the failed request. */
enum ofp_bad_request_code {
    OFPBRC_BAD_VERSION = 0,      /* ofp_header.version not supported. */
    OFPBRC_BAD_TYPE = 1,         /* ofp_header.type not supported. */
    OFPBRC_BAD_STAT         = 2,  /* ofp_stats_request.type not supported. */
    OFPBRC_BAD_EXPERIMENTER = 3, /* Experimenter id not supported
                                  * (in ofp_experimenter_header or
                                   * ofp_stats_request or ofp_stats_reply). */
    OFPBRC_BAD_EXP_TYPE = 4,     /* Experimenter type not supported. */
    OFPBRC_EPERM = 5,            /* Permissions error. */
    OFPBRC_BAD_LEN = 6,          /* Wrong request length for type. */
    OFPBRC_BUFFER_EMPTY = 7,     /* Specified buffer has already been used. */
    OFPBRC_BUFFER_UNKNOWN = 8,   /* Specified buffer does not exist. */
    OFPBRC_BAD_TABLE_ID = 9,     /* Specified table-id invalid or does not
                                  * exist. */
    OFPBRC_IS_SLAVE = 10,        /* Denied because controller is slave. */
    OFPBRC_BAD_PORT = 11,        /* Invalid port. */
    OFPBRC_BAD_PACKET = 12,      /* Invalid packet in packet-out. */
    OFPBRC_MULTIPART_BUFFER_OVERFLOW = 13, /* ofp_multipart_request
                                              overflowed the assigned buffer. */

};
// ofp_error_msg 'code' values for OFPET_BAD_MATCH.  'data' contains at least
// the first 64 bytes of the failed request.

enum ofp_bad_match_code {
    OFPBMC_BAD_TYPE =    0,
    OFPBMC_BAD_LEN =    1,
    OFPBMC_BAD_TAG =    2,
    OFPBMC_BAD_DL_ADDR_MASK =    3,
    OFPBMC_BAD_NW_ADDR_MASK =    4,
    OFPBMC_BAD_WILDCARDS =    5,
    OFPBMC_BAD_FIELD =    6,
    OFPBMC_BAD_VALUE =    7,
    OFPBMC_BAD_MASK =    8,
    OFPBMC_BAD_PREREQ =    9,
    OFPBMC_DUP_FIELD =    10,
    OFPBMC_EPERM =    11
};
// ofp_error_msg 'code' values for OFPET_BAD_ACTION.  'data' contains at least
// the first 64 bytes of the failed request.

enum ofp_bad_action_code {
    OFPBAC_BAD_TYPE =    0,
    OFPBAC_BAD_LEN =    1,
    OFPBAC_BAD_EXPERIMENTER =    2,
    OFPBAC_BAD_EXP_TYPE =    3,
    OFPBAC_BAD_OUT_PORT =    4,
    OFPBAC_BAD_ARGUMENT =    5,
    OFPBAC_EPERM =    6,
    OFPBAC_TOO_MANY =    7,
    OFPBAC_BAD_QUEUE =    8,
    OFPBAC_BAD_OUT_GROUP =    9,
    OFPBAC_MATCH_INCONSISTENT =    10,
    OFPBAC_UNSUPPORTED_ORDER =    11,
    OFPBAC_BAD_TAG =    12,
    OFPBAC_BAD_SET_TYPE =    13,
    OFPBAC_BAD_SET_LEN =    14,
    OFPBAC_BAD_SET_ARGUMENT =    15
};
// ofp_error_msg 'code' values for OFPET_BAD_INSTRUCTION.  'data' contains at least
//  the first 64 bytes of the failed request.

enum ofp_bad_instruction_code {
    OFPBIC_UNKNOWN_INST =    0,
    OFPBIC_UNSUP_INST =    1,
    OFPBIC_BAD_TABLE_ID =    2,
    OFPBIC_UNSUP_METADATA =    3,
    OFPBIC_UNSUP_METADATA_MASK =    4,
    OFPBIC_BAD_EXPERIMENTER =    5,
    OFPBIC_BAD_EXP_TYPE =    6,
    OFPBIC_BAD_LEN =    7,
    OFPBIC_EPERM =    8
};

// ofp_error_msg 'code' values for OFPET_FLOW_MOD_FAILED.  'data' contains
// at least the first 64 bytes of the failed request.

enum ofp_flow_mod_failed_code {
    OFPFMFC_UNKNOWN =    0,
    OFPFMFC_TABLE_FULL =    1,
    OFPFMFC_BAD_TABLE_ID =    2,
    OFPFMFC_OVERLAP =    3,
    OFPFMFC_EPERM =    4,
    OFPFMFC_BAD_TIMEOUT =    5,
    OFPFMFC_BAD_COMMAND =    6,
    OFPFMFC_BAD_FLAGS =    7
};

/* ofp_error_msg 'code' values for OFPET_GROUP_MOD_FAILED.  'data' contains
* at least the first 64 bytes of the failed request. */
enum ofp_group_mod_failed_code {
    OFPGMFC_GROUP_EXISTS = 0,         /* Group not added because a group ADD
                                         attempted to replace an
                                         already-present group. */
    OFPGMFC_INVALID_GROUP = 1,        /* Group not added because Group */
    OFPGMFC_WEIGHT_UNSUPPORTED =    2,

    OFPGMFC_OUT_OF_GROUPS = 3,        /* The group table is full. */

    OFPGMFC_OUT_OF_BUCKETS = 4,       /* The maximum number of action buckets
                                         for a group has been exceeded. */
    OFPGMFC_CHAINING_UNSUPPORTED = 5, /* Switch does not support groups that
                                         forward to groups. */
    OFPGMFC_WATCH_UNSUPPORTED = 6,    /* This group cannot watch the watch_port
                                         or watch_group specified. */
    OFPGMFC_LOOP = 7,                 /* Group entry would cause a loop. */
    OFPGMFC_UNKNOWN_GROUP = 8,        /* Group not modified because a group
                                         MODIFY attempted to modify a
                                         non-existent group. */
    OFPGMFC_CHAINED_GROUP = 9,        /* Group not deleted because another
                                         group is forwarding to it. */
    OFPGMFC_BAD_TYPE = 10,            /* Unsupported or unknown group type. */
    OFPGMFC_BAD_COMMAND = 11,         /* Unsupported or unknown command. */
    OFPGMFC_BAD_BUCKET = 12,          /* Error in bucket. */
    OFPGMFC_BAD_WATCH = 13,           /* Error in watch port/group. */
    OFPGMFC_EPERM = 14,               /* Permissions error. */
};

// ofp_error_msg 'code' values for OFPET_PORT_MOD_FAILED.  'data' contains
// at least the first 64 bytes of the failed request.

enum ofp_port_mod_failed_code {
    OFPPMFC_BAD_PORT =    0,
    OFPPMFC_BAD_HW_ADDR =    1,
    OFPPMFC_BAD_CONFIG =    2,
    OFPPMFC_BAD_ADVERTISE =    3,
    OFPPMFC_EPERM =    4
};
// ofp_error_msg 'code' values for OFPET_TABLE_MOD_FAILED.  'data' contains
// at least the first 64 bytes of the failed request.

enum ofp_table_mod_failed_code {
    OFPTMFC_BAD_TABLE =    0,
    OFPTMFC_BAD_CONFIG =    1,
    OFPTMFC_EPERM =    2
};

// ofp_error_msg �code� values for OFPET_METER_MOD_FAILED. �data� contains
//* at least the first 64 bytes of the failed request.
enum ofp_meter_mod_failed_code {
    OFPMMFC_UNKNOWN =  0,
    OFPMMFC_METER_EXISTS =  1,
    OFPMMFC_INVALID_METER =  2,
    OFPMMFC_UNKNOWN_METER =  3,
    OFPMMFC_BAD_COMMAND =  4,
    OFPMMFC_BAD_FLAGS =  5,
    OFPMMFC_BAD_RATE =  6,
    OFPMMFC_BAD_BURST =  7,
    OFPMMFC_BAD_BAND =  8,
    OFPMMFC_BAD_BAND_VALUE =  9,
    OFPMMFC_OUT_OF_METERS =  10,
    OFPMMFC_OUT_OF_BANDS =  11

};
// Unspecified error.
// Meter not added because a Meter ADD
//                                * attempted to replace an existing Meter.
// Meter not added because Meter specified
//                                * is invalid.
// Meter not modified because a Meter
//                                  MODIFY attempted to modify a non-existent
//                                  Meter.
// Unsupported or unknown command.
// Flag configuration unsupported.
// Rate unsupported.
// Burst size unsupported.
// Band unsupported.
// Band value unsupported.
// No more meters available.
// The maximum number of properties
//                                 * for a meter has been exceeded.
// ofp_error_msg �code� values for OFPET_TABLE_FEATURES_FAILED. �data� contains
//* at least the first 64 bytes of the failed request.
enum ofp_table_features_failed_code {
    OFPTFFC_BAD_TABLE =  0,
    OFPTFFC_BAD_METADATA =  1,
    OFPTFFC_BAD_TYPE =  2,
    OFPTFFC_BAD_LEN =  3,
    OFPTFFC_BAD_ARGUMENT =  4,
    OFPTFFC_EPERM =  5

};

//ofp_error msg 'code' values for OFPET_QUEUE_OP_FAILED. 'data' contains
//at least the first 64 bytes of the failed request

enum ofp_queue_op_failed_code {
    OFPQOFC_BAD_PORT =    0,
    OFPQOFC_BAD_QUEUE =    1,
    OFPQOFC_EPERM =    2
};
//ofp_error_msg 'code' values for OFPET_SWITCH_CONFIG_FAILED. 'data' contains
//at least the first 64 bytes of the failed request.

enum ofp_switch_config_failed_code {
    OFPSCFC_BAD_FLAGS =    0,
    OFPSCFC_BAD_LEN =    1,
    OFPQCFC_EPERM =    2





};
// ofp_error_msg 'code' values for OFPET_ROLE_REQUEST_FAILED. 'data' contains
// at least the first 64 bytes of the failed request.

enum ofp_role_request_failed_code {
    OFPRRFC_STALE =    0,
    OFPRRFC_UNSUP =    1,
    OFPRRFC_BAD_ROLE =    2
};
// Body of reply to OFPST_DESC request.  Each entry is a NULL-terminated
// ASCII string.
//OFP_ASSERT(sizeof(struct ofp_switch_config) == 1056+16);
struct ofp_desc_stats : public ::ofp_stats_msg {
    char mfr_desc[DESC_STR_LEN];

    char hw_desc[DESC_STR_LEN];

    char sw_desc[DESC_STR_LEN];

    char serial_num[SERIAL_NUM_LEN];

    char dp_desc[DESC_STR_LEN];
    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    friend std::ostream& operator<<(std::ostream &out, const ofp_desc_stats &obj);
};
// Body for ofp_stats_request of type OFPST_FLOW.
// OFP_ASSERT(sizeof(ofp_flow_stats_request) == 40+16);
struct ofp_flow_stats_request : public ::ofp_stats_msg {
    uint8_t table_id = 0;

    uint8_t pad[3] = {0,0,0};

    uint32_t out_port = 0;

    uint32_t out_group = 0;

    uint32_t pad2 = 0;

    uint64_t cookie = 0;

    uint64_t cookie_mask = 0;

    struct ofp_match match;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint32_t get_out_port() const;

    inline void set_out_port(const uint32_t value);

    inline uint32_t get_out_group() const;

    inline void set_out_group(const uint32_t value);

    inline uint64_t get_cookie() const;

    inline void set_cookie(const uint64_t value);

    inline uint64_t get_cookie_mask() const;

    inline void set_cookie_mask(const uint64_t value);
};
inline uint32_t ofp_flow_stats_request::get_out_port() const {
    return ntohl(out_port);
}

inline void ofp_flow_stats_request::set_out_port(const uint32_t value) {
    out_port = htonl(value);
}

inline uint32_t ofp_flow_stats_request::get_out_group() const {
    return ntohl(out_group);
}

inline void ofp_flow_stats_request::set_out_group(const uint32_t value) {
    out_group = htonl(value);
}

inline uint64_t ofp_flow_stats_request::get_cookie() const {
    return ofp_message::ntohll(cookie);
}

inline void ofp_flow_stats_request::set_cookie(const uint64_t value) {
    cookie = ofp_message::htonll(value);
}

inline uint64_t ofp_flow_stats_request::get_cookie_mask() const {
    return ofp_message::ntohll(cookie_mask);
}

inline void ofp_flow_stats_request::set_cookie_mask(const uint64_t value) {
    cookie_mask = ofp_message::htonll(value);
}

// Body of reply to OFPST_FLOW request.
// OFP_ASSERT(sizeof(ofp_flow_stats) == 56+16);
struct ofp_flow_stats : public ::ofp_stats_msg {
    uint16_t stats_length = 0;

    uint8_t table_id = 0;

    uint8_t pad = 0;

    uint32_t duration_sec = 0;

    uint32_t duration_nsec = 0;

    uint16_t priority = 0;

    uint16_t idle_timeout = 0;

    uint16_t hard_timeout = 0;

    uint8_t pad2[6] = {0,0,0,0,0,0};

    uint64_t cookie = 0;

    uint64_t packet_count = 0;

    uint64_t byte_count = 0;

    struct ofp_match match;
    //struct ofp_instruction instructions[0]; /* Instruction set. */
    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint16_t get_stats_length() const;

    inline void set_stats_length(const uint16_t value);

    inline uint32_t get_duration_sec() const;

    inline void set_duration_sec(const uint32_t value);

    inline uint32_t get_duration_nsec() const;

    inline void set_duration_nsec(const uint32_t value);

    inline uint16_t get_priority() const;

    inline void set_priority(const uint16_t value);

    inline uint16_t get_idle_timeout() const;

    inline void set_idle_timeout(const uint16_t value);

    inline uint16_t get_hard_timeout() const;

    inline void set_hard_timeout(const uint16_t value);

    inline uint64_t get_cookie() const;

    inline void set_cookie(const uint64_t value);

    inline uint64_t get_packet_count() const;

    inline void set_packet_count(const uint64_t value);

    inline uint64_t get_byte_count() const;

    inline void set_byte_count(const uint64_t value);

    friend std::ostream& operator<<(std::ostream &out, const ofp_flow_stats &obj);
};
inline uint16_t ofp_flow_stats::get_stats_length() const {
    return ntohs(stats_length);
}

inline void ofp_flow_stats::set_stats_length(const uint16_t value) {
    stats_length = htons(value);
}

inline uint32_t ofp_flow_stats::get_duration_sec() const {
    return ntohl(duration_sec);
}

inline void ofp_flow_stats::set_duration_sec(const uint32_t value) {
    duration_sec = htonl(value);
}

inline uint32_t ofp_flow_stats::get_duration_nsec() const {
    return ntohl(duration_nsec);
}

inline void ofp_flow_stats::set_duration_nsec(const uint32_t value) {
    duration_nsec = htonl(value);
}

inline uint16_t ofp_flow_stats::get_priority() const {
    return ntohs(priority);
}

inline void ofp_flow_stats::set_priority(const uint16_t value) {
    priority = htons(value);
}

inline uint16_t ofp_flow_stats::get_idle_timeout() const {
    return ntohs(idle_timeout);
}

inline void ofp_flow_stats::set_idle_timeout(const uint16_t value) {
    idle_timeout = htons(value);
}

inline uint16_t ofp_flow_stats::get_hard_timeout() const {
    return ntohs(hard_timeout);
}

inline void ofp_flow_stats::set_hard_timeout(const uint16_t value) {
    hard_timeout = htons(value);
}

inline uint64_t ofp_flow_stats::get_cookie() const {
    return ofp_message::ntohll(cookie);
}

inline void ofp_flow_stats::set_cookie(const uint64_t value) {
    cookie = ofp_message::htonll(value);
}

inline uint64_t ofp_flow_stats::get_packet_count() const {
    return ofp_message::ntohll(packet_count);
}

inline void ofp_flow_stats::set_packet_count(const uint64_t value) {
    packet_count = ofp_message::htonll(value);
}

inline uint64_t ofp_flow_stats::get_byte_count() const {
    return ofp_message::ntohll(byte_count);
}

inline void ofp_flow_stats::set_byte_count(const uint64_t value) {
    byte_count = ofp_message::htonll(value);
}

// Body for ofp_stats_request of type OFPST_AGGREGATE.
// OFP_ASSERT(sizeof(ofp_aggregate_stats_request) == 40+16);
struct ofp_aggregate_stats_request : public ::ofp_stats_msg {
    uint8_t table_id = 0;

    uint8_t pad[3] = {0,0,0};

    uint32_t out_port = 0;

    uint32_t out_group = 0;

    uint32_t pad2 = 0;

    uint64_t cookie = 0;

    uint64_t cookie_mask = 0;

    struct ofp_match match;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint32_t get_out_port() const;

    inline void set_out_port(const uint32_t value);

    inline uint32_t get_out_group() const;

    inline void set_out_group(const uint32_t value);

    inline uint64_t get_cookie() const;

    inline void set_cookie(const uint64_t value);

    inline uint64_t get_cookie_mask() const;

    inline void set_cookie_mask(const uint64_t value);

};
inline uint32_t ofp_aggregate_stats_request::get_out_port() const {
    return ntohl(out_port);
}

inline void ofp_aggregate_stats_request::set_out_port(const uint32_t value) {
    out_port = htonl(value);
}

inline uint32_t ofp_aggregate_stats_request::get_out_group() const {
    return ntohl(out_group);
}

inline void ofp_aggregate_stats_request::set_out_group(const uint32_t value) {
    out_group = htonl(value);
}

inline uint64_t ofp_aggregate_stats_request::get_cookie() const {
    return ofp_message::ntohll(cookie);
}

inline void ofp_aggregate_stats_request::set_cookie(const uint64_t value) {
    cookie = ofp_message::htonll(value);
}

inline uint64_t ofp_aggregate_stats_request::get_cookie_mask() const {
    return ofp_message::ntohll(cookie_mask);
}

inline void ofp_aggregate_stats_request::set_cookie_mask(const uint64_t value) {
    cookie_mask = ofp_message::htonll(value);
}

// Body of reply to OFPST_AGGREGATE request.
// OFP_ASSERT(sizeof(ofp_aggregate_stats_reply) == 24+16);
struct ofp_aggregate_stats : public ::ofp_stats_msg {
    uint64_t packet_count = 0;

    uint64_t byte_count = 0;

    uint32_t flow_count = 0;

    uint32_t pad = 0;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint64_t get_packet_count() const;

    inline void set_packet_count(const uint64_t value);

    inline uint64_t get_byte_count() const;

    inline void set_byte_count(const uint64_t value);

    inline uint32_t get_flow_count() const;

    inline void set_flow_count(const uint32_t value);

};
inline uint64_t ofp_aggregate_stats::get_packet_count() const {
    return ofp_message::ntohll(packet_count);
}

inline void ofp_aggregate_stats::set_packet_count(const uint64_t value) {
    packet_count = ofp_message::htonll(value);
}

inline uint64_t ofp_aggregate_stats::get_byte_count() const {
    return ofp_message::ntohll(byte_count);
}

inline void ofp_aggregate_stats::set_byte_count(const uint64_t value) {
    byte_count = ofp_message::htonll(value);
}

inline uint32_t ofp_aggregate_stats::get_flow_count() const {
    return ntohl(flow_count);
}

inline void ofp_aggregate_stats::set_flow_count(const uint32_t value) {
    flow_count = htonl(value);
}

// Body of reply to OFPST_TABLE request.
// OFP_ASSERT(sizeof(ofp_table_stats) == 128+16);
struct ofp_table_stats_body {
    uint8_t table_id = 0;

    uint8_t pad[3] = {0,0,0};

    uint32_t active_count = 0;

    uint64_t lookup_count = 0;

    uint64_t matched_count = 0;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};

    inline uint32_t get_active_count() const;

    inline void set_active_count(const uint32_t value);

    inline uint64_t get_lookup_count() const;

    inline void set_lookup_count(const uint64_t value);

    inline uint64_t get_matched_count() const;

    inline void set_matched_count(const uint64_t value);

};

struct ofp_table_stats : public ::ofp_stats_msg {
    ofp_table_stats_body body[1];
    inline unsigned int get_minimum_length() const { return sizeof(*this);};
};

inline uint32_t ofp_table_stats_body::get_active_count() const {
    return ntohl(active_count);
}

inline void ofp_table_stats_body::set_active_count(const uint32_t value) {
    active_count = htonl(value);
}

inline uint64_t ofp_table_stats_body::get_lookup_count() const {
    return ofp_message::ntohll(lookup_count);
}

inline void ofp_table_stats_body::set_lookup_count(const uint64_t value) {
    lookup_count = ofp_message::htonll(value);
}

inline uint64_t ofp_table_stats_body::get_matched_count() const {
    return ofp_message::ntohll(matched_count);
}

inline void ofp_table_stats_body::set_matched_count(const uint64_t value) {
    matched_count = ofp_message::htonll(value);
}

// Body for ofp_stats_request of type OFPST_PORT.
// OFP_ASSERT(sizeof(ofp_port_stats_request) == 8+16);
struct ofp_port_stats_request : public ::ofp_stats_msg {
    uint32_t port_no = 0;

    uint32_t pad = 0;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint32_t get_port_no() const;

    inline void set_port_no(const uint32_t value);

};
inline uint32_t ofp_port_stats_request::get_port_no() const {
    return ntohl(port_no);
}

inline void ofp_port_stats_request::set_port_no(const uint32_t value) {
    port_no = htonl(value);
}

//Body of reply to OFPST_PORT request. If a counter is unsupported, set the field to all ones.
//OFP_ASSERT(sizeof(ofp_port_stats) == 104+16);
struct  ofp_port_stats : public ::ofp_stats_msg {
    ofp_port_stats_body body[1];
    inline unsigned int get_minimum_length() const { return sizeof(*this);};
};
// Body of OFPST_GROUP request.
// OFP_ASSERT(sizeof(ofp_group_stats_request) == 8+16);
struct ofp_group_stats_request : public ::ofp_stats_msg {
    uint32_t group_id = 0;

    uint32_t pad = 0;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint32_t get_group_id() const;

    inline void set_group_id(const uint32_t value);

};
inline uint32_t ofp_group_stats_request::get_group_id() const {
    return ntohl(group_id);
}

inline void ofp_group_stats_request::set_group_id(const uint32_t value) {
    group_id = htonl(value);
}



typedef ::ofp_stats_msg ofp_group_desc_stats_request;
// Body of reply to OFPST_GROUP_DESC request
// OFP_ASSERT(sizeof(ofp_group_desc_stats) == 8+16);
struct ofp_group_desc_stats_body {
    uint16_t group_desc_length = 0;

    uint8_t group_type = 0;

    uint8_t pad = 0;

    uint32_t group_id = 0;

    struct ofp_bucket buckets[0];

    inline unsigned int get_minimum_length() const { return sizeof(*this);};

    inline uint16_t get_length() const;

    inline void set_length(const uint16_t value);

    inline uint32_t get_group_id() const;

    inline void set_group_id(const uint32_t value);

};
struct ofp_group_desc_stats : public ::ofp_stats_msg {
    ofp_group_desc_stats_body body[1];
    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    friend std::ostream& operator<<(std::ostream &out, const ofp_group_desc_stats &obj);
};


inline uint16_t ofp_group_desc_stats_body::get_length() const {
    return ntohs(group_desc_length);
}

inline void ofp_group_desc_stats_body::set_length(const uint16_t value) {
    group_desc_length = htons(value);
}

inline uint32_t ofp_group_desc_stats_body::get_group_id() const {
    return ntohl(group_id);
}

inline void ofp_group_desc_stats_body::set_group_id(const uint32_t value) {
    group_id = htonl(value);
}

typedef ::ofp_stats_msg ofp_group_feature_stats_request;

// Body of reply to OFPST_GROUP_FEATURES request. Group features.
// OFP_ASSERT(sizeof(ofp_group_features_stats) == 40+16);
struct ofp_group_features_stats : public ::ofp_stats_msg {
    uint32_t types = 0;

    uint32_t capabilities = 0;

    uint32_t max_groups[4] = {0,0,0,0};

    uint32_t actions[4] = {0,0,0,0};

    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint32_t get_types() const;

    inline void set_types(const uint32_t value);

    inline uint32_t get_capabilities() const;

    inline void set_capabilities(const uint32_t value);
    friend std::ostream& operator<<(std::ostream &out, const ofp_group_features_stats &obj);

};
inline uint32_t ofp_group_features_stats::get_types() const {
    return ntohl(types);
}

inline void ofp_group_features_stats::set_types(const uint32_t value) {
    types = htonl(value);
}

inline uint32_t ofp_group_features_stats::get_capabilities() const {
    return ntohl(capabilities);
}

inline void ofp_group_features_stats::set_capabilities(const uint32_t value) {
    capabilities = htonl(value);
}


/* All ones is used to indicate all queues in a port (for stats retrieval). */
#define OFPQ_ALL      0xffffffff

/* Min rate > 1000 means not configured. */
#define OFPQ_MIN_RATE_UNCFG      0xffff

// Minimum datarate guaranteed.
// Maximum datarate.
// Experimenter defined property.
enum ofp_queue_properties {
    OFPQT_NONE = 0,
    OFPQT_MIN_RATE =    1,
    OFPQT_MAX_RATE =    2,
    OFPQT_EXPERIMENTER =    0xffff
};
// Common description for a queue.
// OFP_ASSERT(sizeof(ofp_queue_prop_header) == 8);
// /* One of OFPQT_. */
struct ofp_queue_prop_header {
    uint16_t property = 0;

    uint16_t len = 0;

    uint32_t pad = 0;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint16_t get_property() const;

    inline void set_property(const uint16_t value);

    inline uint16_t get_len() const;

    inline void set_len(const uint16_t value);

};
inline uint16_t ofp_queue_prop_header::get_property() const {
    return ntohs(property);
}

inline void ofp_queue_prop_header::set_property(const uint16_t value) {
    property = htons(value);
}

inline uint16_t ofp_queue_prop_header::get_len() const {
    return ntohs(len);
}

inline void ofp_queue_prop_header::set_len(const uint16_t value) {
    len = htons(value);
}

// Min or Max Rate queue property description.
// OFP_ASSERT(sizeof(ofp_queue_prop_rate) == 16);
struct ofp_queue_prop_rate : public ofp_queue_prop_header {
    uint16_t rate = 0;

    uint8_t pad[6] = {0,0,0,0,0,0};

    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint16_t get_rate() const;

    inline void set_rate(const uint16_t value);

};
inline uint16_t ofp_queue_prop_rate::get_rate() const {
    return ntohs(rate);
}

inline void ofp_queue_prop_rate::set_rate(const uint16_t value) {
    rate = htons(value);
}

/* Min-Rate queue property description. */
typedef ofp_queue_prop_rate ofp_queue_prop_min_rate;
/* Max-Rate queue property description. */
typedef ofp_queue_prop_rate ofp_queue_prop_max_rate;

// Experimenter queue property description.
// OFP_ASSERT(sizeof(ofp_queue_prop_experimenter) == 16);
struct ofp_queue_prop_experimenter : public ofp_queue_prop_header {
    uint32_t experimenter = 0;

    uint32_t pad = 0;

    uint8_t data[0];

    inline uint32_t get_experimenter() const;

    inline void set_experimenter(const uint32_t value);

};
inline uint32_t ofp_queue_prop_experimenter::get_experimenter() const {
    return ntohl(experimenter);
}

inline void ofp_queue_prop_experimenter::set_experimenter(const uint32_t value) {
    experimenter = htonl(value);
}

// Full description for a queue.
// OFP_ASSERT(sizeof(ofp_packet_queue) == 16);
struct ofp_packet_queue {
    uint32_t queue_id = 0;

    uint32_t port = 0;

    uint16_t len = 0;

    uint8_t pad[6] = {0,0,0,0,0,0};

    struct ofp_queue_prop_header properties[0];

    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint32_t get_queue_id() const;

    inline void set_queue_id(const uint32_t value);

    inline uint32_t get_port() const;

    inline void set_port(const uint32_t value);

    inline uint16_t get_len() const;

    inline void set_len(const uint16_t value);

};
inline uint32_t ofp_packet_queue::get_queue_id() const {
    return ntohl(queue_id);
}

inline void ofp_packet_queue::set_queue_id(const uint32_t value) {
    queue_id = htonl(value);
}

inline uint32_t ofp_packet_queue::get_port() const {
    return ntohl(port);
}

inline void ofp_packet_queue::set_port(const uint32_t value) {
    port = htonl(value);
}

inline uint16_t ofp_packet_queue::get_len() const {
    return ntohl(len);
}

inline void ofp_packet_queue::set_len(const uint16_t value) {
    len = htons(value);
}

// Query for port queue configuration.
// OFP_ASSERT(sizeof(ofp_queue_get_config_request) == 16);
struct ofp_queue_get_config_request : public ::ofp_message {
    uint32_t port = 0;

    uint32_t pad = 0;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint32_t get_port() const;

    inline void set_port(const uint32_t value);

};
inline uint32_t ofp_queue_get_config_request::get_port() const {
    return ntohl(port);
}

inline void ofp_queue_get_config_request::set_port(const uint32_t value) {
    port = htonl(value);
}

// Queue configuration for a given port.
// OFP_ASSERT(sizeof(ofp_queue_get_config_reply) == 16);
struct ofp_queue_get_config_reply : public ::ofp_message {
    uint32_t port = 0;

    uint32_t pad = 0;

    struct ofp_packet_queue queues[0];

    inline uint32_t get_port() const;

    inline void set_port(const uint32_t value);

};
inline uint32_t ofp_queue_get_config_reply::get_port() const {
    return ntohl(port);
}

inline void ofp_queue_get_config_reply::set_port(const uint32_t value) {
    port = htonl(value);
}

// OFPAT_SET_QUEUE action struct: send packets to given queue on port.
// OFP_ASSERT(sizeof(ofp_action_set_queue) == 8);
struct ofp_action_set_queue : public ::ofp_tlv_base {
    uint32_t queue_id = 0;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint32_t get_queue_id() const;

    inline void set_queue_id(const uint32_t value);

};
inline uint32_t ofp_action_set_queue::get_queue_id() const {
    return ntohl(queue_id);
}

inline void ofp_action_set_queue::set_queue_id(const uint32_t value) {
    queue_id = htonl(value);
}

//QUEUE State Request
//OFP_ASSERT(sizeof(ofp_queue_stats_request) == 8+16);
struct ofp_queue_stats_request : public ::ofp_stats_msg {
    uint32_t port_no = 0;

    uint32_t queue_id = 0;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint32_t get_port_no() const;

    inline void set_port_no(const uint32_t value);

    inline uint32_t get_queue_id() const;

    inline void set_queue_id(const uint32_t value);

};
inline uint32_t ofp_queue_stats_request::get_port_no() const {
    return ntohl(port_no);
}

inline void ofp_queue_stats_request::set_port_no(const uint32_t value) {
    port_no = htonl(value);
}

inline uint32_t ofp_queue_stats_request::get_queue_id() const {
    return ntohl(queue_id);
}

inline void ofp_queue_stats_request::set_queue_id(const uint32_t value) {
    queue_id = htonl(value);
}


// Configures the "role" of the sending controller.  The default role is:
//
//     - Equal (NX_ROLE_EQUAL), which allows the controller access to all
//       OpenFlow features. All controllers have equal responsibility.
//
//  The other possible roles are a related pair:
//
//     - Master (NX_ROLE_MASTER) is equivalent to Equal, except that there may
//       be at most one Master controller at a time: when a controller
//       configures itself as Master, any existing Master is demoted to the
//       Slave role.
//
//     - Slave (NX_ROLE_SLAVE) allows the controller read-only access to
//       OpenFlow features.  In particular attempts to modify the flow table
//       will be rejected with an OFPBRC_EPERM error.
//
//       Slave controllers do not receive OFPT_PACKET_IN or OFPT_FLOW_REMOVED
//       messages, but they do receive OFPT_PORT_STATUS messages.
enum ofp_controller_role {
    OFPCR_ROLE_NOCHANGE =    0,
    OFPCR_ROLE_EQUAL =    1,
    OFPCR_ROLE_MASTER =    2,
    OFPCR_ROLE_SLAVE =    3 //Read-only access.
};
//Read-only access.
//Role request and reply message.
//OFP_ASSERT(sizeof(ofp_role_request) == 24);
struct ofp_role_request : public ::ofp_message {
    uint32_t role = 0;

    uint32_t pad = 0;

    uint64_t generation_id = 0;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint32_t get_role() const;

    inline void set_role(const uint32_t value);

    inline uint64_t get_generation_id() const;

    inline void set_generation_id(const uint64_t value);
    friend std::ostream& operator<<(std::ostream &out, const ofp_role_request &msg);

};
inline uint32_t ofp_role_request::get_role() const {
    return ntohl(role);
}

inline void ofp_role_request::set_role(const uint32_t value) {
    role = htonl(value);
}

inline uint64_t ofp_role_request::get_generation_id() const {
    return ofp_message::ntohll(generation_id);
}

inline void ofp_role_request::set_generation_id(const uint64_t value) {
    generation_id = ofp_message::htonll(value);
}

enum ofp_port_config {
    OFPPC_PORT_DOWN =     1 << 0,
    OFPPC_NO_STP =     1 << 1, // not in 1.2+
    OFPPC_NO_RECV =     1 << 2,
    OFPPC_NO_RECV_STP =     1 << 3, // not in 1.2+
    OFPPC_NO_FLOOD =     1 << 4, // not in 1.2+
    OFPPC_NO_FWD =     1 << 5,
    OFPPC_NO_PACKET_IN =     1 << 6
};

///OTWG Extensions starts
#define OFP_EXPERIMENTER_OUI 0xFF000007
enum ofp_experimenter_stats_type {
    OFPEMPTE = 1 /* Extended port descriptions for 1.3 */
};

struct ofp_experimenter_port {
    uint32_t port_no = 0;
    uint16_t exp_port_length = 0;
    uint16_t pad = 0;
    uint8_t hw_addr[OFP_ETH_ALEN] = {0,0,0,0,0,0};
    uint16_t pad2 = 0;
    char    name[OFP_MAX_PORT_NAME_LEN];
    uint32_t config = 0; /// OFPPC_ flags
    uint32_t state = 0; /// OFPPS_ flags
    ::ofp_tlv_base properties[0]; /// optional port feature 1.5
};

struct ofp_optical_port_property : public ofp_tlv_base {
    uint8_t port_signal_type = 0;
    uint8_t reserved = 0;
    uint16_t pad = 0;
    ::ofp_tlv_base features[0]; /// optional for 1.3 - must be padded
};

enum optical_port_signal_type {
    OFPOTPT_OTSn = 1,
    OFPOTPT_OMSn = 2,
    OFPOTPT_OPSn = 3,
    OFPOTPT_OPSm = 4,
    OFPOTPT_OCh = 5,
    OFPOTPT_OTU1 = 11,
    OFPOTPT_OTU2 = 12,
    OFPOTPT_OTU3 = 13,
    OFPOTPT_OTU4 = 14
};

enum optical_port_features {
    OFPOTPF_OPT_INTFCE_CLASS = 1, /// Application code/id encoding
    OFPOTPF_LAYER_STACK		 = 2 /// Supported signal types
};

enum optical_interface_class {
    OFPOICT_PROPRIETARY = 1 << 7,
    OFPOICT_ITUT_G698_1 = 1,
    OFPOICT_ITUT_G698_2 = 2,
    OFPOICT_ITUT_G959_1 = 3,
    OFPOICT_ITUT_G695 = 4
};

struct optical_application_code : public ::ofp_tlv_base {
    uint8_t oic_type = 0; /// base_type is intfce_class or layer_stack
    char    app_code[15] = {0}; /// null terminated content
};

struct optical_port_layer_stack_entry {
    uint8_t layer_class = 0;
    uint8_t signal_type = 0;
    uint8_t adaptation = 0;
    uint8_t pad[5] = {0,0,0,0,0};
};


/* Total size must padded for 8 byte alignment */
struct optical_port_layer_stack : public ::ofp_tlv_base {
    optical_port_layer_stack_entry values[0];
};

enum optical_layer_class {
    OFPOTPL_PORT = 1,
    OFPOTPL_OCH = 2,
    OFPOPTL_ODU = 3,
    OFPOPTL_ODUCLT = 4
};

enum optical_och_signal_type {
    OFPOCHT_FIX_GRID       = 1,
    OFPOCHT_FLEX_GRID      = 2
};

enum optical_odu_signal_type {
    OFPODUT_ODU1           = 1,
    OFPODUT_ODU2           = 2,
    OFPODUT_ODU3           = 3,
    OFPODUT_ODU4           = 4,
    OFPODUT_ODU0           = 10,
    OFPODUT_ODU2E          = 11,
    OFPODUT_ODUfCBR        = 20,
    OFPODUT_ODUfGFPfHAO    = 21,
    OFPODUT_ODUfGFPf       = 22,
    /// default value
    OFPODUT_DEFAULT = OFPODUT_ODU2
};

enum optical_oduclt_signal_type {
    OFPODUCLT_STM16        = 1,
    OFPODUCLT_STM64        = 2,
    OFPODUCLT_STM256       = 3,
    OFPODUCLT_STM1         = 4,
    OFPODUCLT_STM4         = 5,
    OFPODUCLT_1GBE         = 6,
    OFPODUCLT_10GBE        = 7,
    OFPODUCLT_40GBE        = 8,
    OFPODUCLT_100GBE       = 9,
    OFPODUCLT_FC100        = 10,
    OFPODUCLT_FC200        = 11,
    OFPODUCLT_FC400        = 12,
    OFPODUCLT_FC800        = 13,
    OFPODUCLT_FC1200       = 14,
    OFPODUCLT_GPON         = 15,
    OFPODUCLT_XGPON        = 16,
    OFPODUCLT_IB_SDR       = 17,
    OFPODUCLT_IB_DDR       = 18,
    OFPODUCLT_IB_QDR       = 19,
    OFPODUCLT_SBCON_ESCON  = 20,
    OFPODUCLT_DVB_ASI      = 21,
    OFPODUCLT_SDI          = 22,
    OFPODUCLT_SDI1G5       = 23,
    OFPODUCLT_SDI3G        = 24,
    OFPODUCLT_ATM          = 25,
    OFPODUCLT_ETH          = 26,
    OFPODUCLT_MPLS         = 27,
    OFPODUCLT_IP           = 28
};

enum optical_adaptation_type {
    OFPADAPT_OTS_OMS       = 1,
    OFPADAPT_OMS_OCH       = 2,
    OFPADAPT_OPS_OCHr      = 3,
    OFPADAPT_OPSM_OTUk     = 4,
    OFPADAPT_OCH_OTUk      = 5,
    OFPADAPT_ODUk_ODUij    = 6,
    OFPADAPT_ODUk_ODUj21   = 7,
    OFPADAPT_ODUkh_ODUj21  = 8,
    OFPADAPT_ODU0_CBRx     = 9,
    OFPADAPT_ODUk_CBRx     = 10,
    OFPADAPT_ODUk_CBRxg    = 11,
    OFPADAPT_ODUk_RSn      = 12,
    OFPADAPT_ODUk_ATM      = 13,
    OFPADAPT_ODUk_ETH      = 14,
    OFPADAPT_ODUkh_ETH     = 15,
    OFPADAPT_ODUk_ETHPPOS  = 16
};

enum ofp_experimenters_field {
    OFPXMT_EXP_ODU_SIGTYPE     = 2,
    OFPXMT_EXP_ODU_SIGID       = 3,
    OFPXMT_EXP_OCH_SIGTYPE     = 4,
    OFPXMT_EXP_OCH_SIGID       = 5
};

struct oxm_odu_sigtype : public oxm_experimenter_fields {
    uint8_t sigtype = 0;
    inline uint16_t get_minimum_length() const { return (sizeof(*this) - sizeof(oxm_experimenter_fields)); }
}; //8+1 bytes

typedef oxm_odu_sigtype oxm_och_sigtype;
#define TS_PER_BYTE 8
struct oxm_odu_sigid : public oxm_experimenter_fields {
    // bit field defs if it was portable
    uint16_t tpn = 0;    // :12
                     // uint8_t  reserved   :8
    uint16_t tslen = 0;  // :12
    uint8_t  tsmap[0]; /* trib slot bitmap */

    inline uint16_t get_tpn() const                 { return ntohs((tpn  & 0xFFF0) >> 4); }
    inline uint16_t get_tslen() const               { return ntohs((tslen & 0x0FFF)); }
    inline void     set_tpn(const uint16_t param)   { tpn  &= 0x000F; tpn  |= ((htons(param) & 0xFFF) << 4); }
    inline void     set_tslen(const uint16_t param) { tslen &= 0xF000; tslen |= ((htons(param) & 0xFFF)); }
    inline uint16_t get_minimum_length() const { return (sizeof(*this) - sizeof(oxm_experimenter_fields)); }
}; //4+4 bytes

struct oxm_och_sigid : public oxm_experimenter_fields {
    uint8_t  gridtype = 0; /* trib port number */
    uint8_t  chl_spacing = 0;
    uint16_t n = 0; /* n */
    uint16_t m = 0; /* m */
    inline uint16_t get_n() const                 { return ntohs(n); }
    inline uint16_t get_m() const               { return ntohs(m); }
    inline void     set_n(const uint16_t param)   { n = htons(param); }
    inline void     set_m(const uint16_t param) { m = htons(param); }
    inline uint16_t get_minimum_length() const { return (sizeof(*this) - sizeof(oxm_experimenter_fields)); }
}; //8+6 bytes

enum ofp_grid_type {
    OFPGRIDT_RES    = 0,
    OFPGRIDT_DWDM   = 1,
    OFPGRIDT_CWDM   = 2,
    OFPGRIDT_FLEX   = 3     /* Note: Flex grid pending further discussion */
};


enum ofp_chl_spacing {
    OFPCS_RES       = 0,
    OFPCS_100GHZ    = 1,
    OFPCS_50GHZ     = 2,
    OFPCS_25GHZ     = 3,
    OFPCS_12P5GHZ   = 4,    /* 12.5 GHZ */
    OFPCS_6P25GHZ   = 5 /* 6.25 GHZ */
};

///OTWG Extensions ends

/**
 * Uitility functions for this namespace
 * @author sraihan 
 * 
 * @param instructions 
 * @param instructions_type 
 * @param cur 
 * @param reply 
 * 
 * @return bool 
 */
bool EncodeTableFeatureInstructions(const uint32_t instructions, const uint16_t instructions_type, ofp_tlv_base *cur, OpenflowV4::ofp_table_features_stats &reply);
/**
 * Uitility functions for this namespace
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
bool EncodeTableFeatureActions(const uint32_t actions, const uint16_t actions_type, ofp_tlv_base *cur, OpenflowV4::ofp_table_features_stats &reply);
/**
 * Uitility functions for this namespace
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
bool EncodeTableFeatureOXM(const uint32_t oxm_ids, const uint16_t oxm_ids_type, ofp_tlv_base *cur, OpenflowV4::ofp_table_features_stats &reply);

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
                      const int action_instruction_type);


/**
 *  Used with multiple message types hence templates
 * 
 * @author sraihan 
 * 
 * @param struct_with_ofp_match 
 * @param msg 
 * @param matchFrom 
 * 
 * @return bool 
 */
template<typename struct_with_ofp_match>
bool EncodeOXMFields(ofp_message &msg, const OfDpMatchT &matchFrom);

// List all instance/use of template functions
extern template bool
    EncodeOXMFields<OpenflowV4::ofp_flow_removed>(ofp_message &msg, const OfDpMatchT &matchFrom);

extern template bool
    EncodeOXMFields<OpenflowV4::ofp_packet_in>(ofp_message &msg, const OfDpMatchT &matchFrom);

extern template bool
    EncodeOXMFields<OpenflowV4::ofp_flow_stats>(ofp_message &msg, const OfDpMatchT &matchFrom);

extern template bool
EncodeOXMFields<OpenflowV4::ofp_flow_mod>(ofp_message& msg, const OfDpMatchT& matchFrom);


/**
 * @note convert oxm_fields_t element to OfDpMatchT
 * @see ofp_flow_mod
 * @author sraihan 
 * 
 * @param cur_ptr 
 * @param match 
 * @param error_code 
 * 
 * @return bool 
 */
bool ParseOXMField(const oxm_fields_t *&cur_ptr,
                   OfDpMatchT &match,
                   std::ostream&,
                   uint16_t &error_code);

/**
 * @note convert ofp_tlv_base element to OfDpActionT element
 * @see ofp_flow_mod
 * @author sraihan 
 * 
 * @param action_head 
 * @param actions 
 * @param num_actions 
 * @param error_code 
 * 
 * @return bool 
 */
bool ParseActionItem(const ofp_tlv_base *&action_head,
                     OfDpActionT *actions,
                     uint16_t &num_actions,
                     std::ostream&,
                     uint16_t &error_code);

/**
 * @see ofp_flow_mod
 * @author sraihan 
 * 
 * @param instruction_head 
 * @param actions 
 * @param num_actions 
 * @param max_actions 
 * @param error_type 
 * @param error_code 
 * 
 * @return bool 
 */
bool ParseInstructionItem(const ofp_tlv_base *&instruction_head,
                          OfDpActionT *actions,
                          uint16_t &num_actions,
                          const size_t max_actions,
                          std::ostream&,
                          uint16_t &error_type,
                          uint16_t &error_code);
/**
 *  
 * @author sraihan 
 * 
 * @param buckets 
 * @param num_buckets 
 * @param max_buckets 
 * @param error_type 
 * @param error_code 
 * 
 * @return bool 
 */
bool ParseBuckets(const ::ofp_bucket *&,
                  OfDpBucketT *buckets,
                  uint16_t &num_buckets,
                  const size_t max_buckets,
                  std::ostream&,
                  uint16_t &error_type,
                  uint16_t &error_code);
/**
 *  
 * @author sraihan 
 * 
 * @param msg 
 * @param cur_bucket 
 * @param buckets 
 * @param num_buckets 
 * 
 * @return bool 
 */
bool EncodeBuckets(ofp_message &msg,
                   ::ofp_bucket *cur_bucket,
                   const OfDpBucketT *buckets,
                   const uint16_t num_buckets,
                   std::ostream&);
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
                          uint16_t &code);
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
bool ParseSetActionExperimenter(const ofp_action_set_field *data,
                                OfDpActionT *actions,
                                uint16_t &num_actions,
                                std::ostream &out,
                                uint16_t &code);
} // namespace OpenflowV4
#pragma pack(pop)

#endif
