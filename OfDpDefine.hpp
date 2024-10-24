
#ifndef OfDpDefine_h_
    #define OfDpDefine_h_
/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
    #include "ofDebug.h"
    #include "OfConfig.hpp"
    #include <ostream>
// Openflow parsing related enums and structs are defined in OfDpDefine.hpp

    #pragma pack(push, 4)
// Spec 1.0 Specific
    #define OFP_VERSION_V4   0x04 /* 1.3 Spec */
    #define MAX_TSLEN_BYTES  12
enum OfDpFlowCommand {
    OFDP_FLOW_NONE=-1,
    OFDP_FLOW_ADD=0,
    OFDP_FLOW_MODIFY,
    OFDP_FLOW_MODIFY_STRICT,
    OFDP_FLOW_DELETE,
    OFDP_FLOW_DELETE_STRICT
};

enum OfDpGroupType {
    OFDP_GROUP_NONE=-1,
    OFDP_GROUP_ALL=0,
    OFDP_GROUP_SELECT,
    OFDP_GROUP_INDIRECT,
    OFDP_GROUP_FF,
    OFDP_GROUP_MAX_TYPE
};

enum OfDpGroupTypeBitMap {
    OFDP_GROUP_ALL_BIT      = 1 << 0,
    OFDP_GROUP_SELECT_BIT   = 1 << 1,
    OFDP_GROUP_INDIRECT_BIT = 1 << 2,
    OFDP_GROUP_FF_BIT       = 1 << 3
};


enum OfDpGroupCapabilities {
    OFDP_SELECT_WEIGHT      = 1 << 0,
    OFDP_SELECT_LIVENESS    = 1 << 1,
    OFDP_CHAINING           = 1 << 2,
    OFDP_CHAINING_CHECKS    = 1 << 3
};


enum OfDpMaxGroupsPerType {
    OFDP_MAX_ALL_GROUPS         = 0,
    OFDP_MAX_SELECT_GROUPS      = 0,
    OFDP_MAX_INDIRECT_GROUPS    = 512,
    OFDP_MAX_FF_GROUPS          = 512
};

// Switch config

struct OfDpSwitchConfigT {
    uint16_t flags;
    uint16_t miss_send_len;
};

// Port state

enum OfDpPortNo {
    OFDP_OFPP_MAX = 0xffffff00,
    OFDP_OFPP_IN_PORT = 0xfffffff8,
    OFDP_OFPP_TABLE = 0xfffffff9,
    OFDP_OFPP_NORMAL = 0xfffffffa,
    OFDP_OFPP_FLOOD = 0xfffffffb,
    OFDP_OFPP_ALL = 0xfffffffc,
    OFDP_OFPP_CONTROLLER = 0xfffffffd,
    OFDP_OFPP_LOCAL = 0xfffffffe,
    OFDP_OFPP_ANY = 0xffffffff
};

enum OFDpPortFeatures {
    OFDP_OFPPF_10MB_HD    = 1 << 0,  /* 10 Mb half-duplex rate support. */
    OFDP_OFPPF_10MB_FD    = 1 << 1,  /* 10 Mb full-duplex rate support. */
    OFDP_OFPPF_100MB_HD   = 1 << 2,  /* 100 Mb half-duplex rate support. */
    OFDP_OFPPF_100MB_FD   = 1 << 3,  /* 100 Mb full-duplex rate support. */
    OFDP_OFPPF_1GB_HD     = 1 << 4,  /* 1 Gb half-duplex rate support. */
    OFDP_OFPPF_1GB_FD     = 1 << 5,  /* 1 Gb full-duplex rate support. */
    OFDP_OFPPF_10GB_FD    = 1 << 6,  /* 10 Gb full-duplex rate support. */
    OFDP_OFPPF_40GB_FD =    1 << 7,
    OFDP_OFPPF_100GB_FD =    1 << 8,
    OFDP_OFPPF_1TB_FD =    1 << 9,
    OFDP_OFPPF_OTHER =    1 << 10,
    OFDP_OFPPF_COPPER =    1 << 11,
    OFDP_OFPPF_FIBER =    1 << 12,
    OFDP_OFPPF_AUTONEG =    1 << 13,
    OFDP_OFPPF_PAUSE =    1 << 14,
    OFDP_OFPPF_PAUSE_ASYM =    1 << 15
};

enum OfDpPortConfig {
    OFDP_OFPPC_PORT_DOWN = 1 << 0,    // All
    OFDP_OFPPC_NO_STP = 1 << 1,       // 1.0
    OFDP_OFPPC_NO_RECV = 1 << 2,      // All
    OFDP_OFPPC_NO_RECV_STP = 1 << 3,  // 1.0
    OFDP_OFPPC_NO_FLOOD = 1 << 4,     // 1.0
    OFDP_OFPPC_NO_FWD = 1 << 5,       // All
    OFDP_OFPPC_NO_PACKET_IN = 1 << 6  // All
};

enum OfDpPortState {
    OFDP_OFPPS_LINK_DOWN = 1 << 0,    // All
    OFDP_OFPPS_BLOCKED = 1 << 1,      // 1.3
    OFDP_OFPPS_LIVE = 1 << 2,         // 1.3
    OFDP_OFPPS_STP_LISTEN = 0 << 8,   // 1.0
    OFDP_OFPPS_STP_LEARN = 1 << 8,    // 1.0
    OFDP_OFPPS_STP_FORWARD = 2 << 8,  // 1.0
    OFDP_OFPPS_STP_BLOCK = 3 << 8,    // 1.0
    OFDP_OFPPS_STP_MASK = 3 << 8      // 1.0
};

enum OfDpConfigFlags {
    OFDP_OFPC_FRAG_NORMAL=0,
    OFDP_OFPC_FRAG_DROP=1,
    OFDP_OFPC_FRAG_REASM=2,
    OFDP_FRAG_MASK=3
};

// Switch features

enum OfDpCapabilities {
    OFDP_FLOW_STATS     = 1 << 0,  // All
    OFDP_TABLE_STATS    = 1 << 1,  // All
    OFDP_PORT_STATS     = 1 << 2,  // All
    OFDP_STP            = 1 << 3,  // 1.0
    OFDP_RESERVED       = 1 << 4,  // All
    OFDP_IP_REASM       = 1 << 5,  // All
    OFDP_QUEUE_STATS    = 1 << 6,  // All
    OFDP_ARP_MATCH_IP   = 1 << 7,  // 1.0
    OFDP_VPORT_TABLE    = 1 << 8,  // 1.0 + MPLS Ext
    OFDP_PORT_BLOCKED   = 1 << 8   // 1.3
};

struct OfDpSwitchFeaturesT {
    uint64_t datapath_id;
    uint32_t n_buffers;
    uint8_t n_tables;
    uint32_t capabilities;

    // These two fields are only relevant if LS is running as v1.0.
    uint32_t actions;
    OfDpSwitchFeaturesT():datapath_id(0),n_buffers(0),n_tables(0),capabilities(0)
    ,actions(0)
    {
    }
};

struct OfDpPortListT {
    uint16_t flags;
    OfDpPortInfoT ports[1];
    OfDpPortListT():flags(0)
    {
    }
};

enum OfDpInstructionCommand {
    OFDP_NONE_INSTRUCTION = 0,
    OFDP_GOTO_TABLE_INSTRUCTION = 1,
    OFDP_WRITE_METADATA_INSTRUCTION = 2,
    OFDP_WRITE_INSTRUCTION = 3,
    OFDP_APPLY_INSTRUCTION = 4,
    OFDP_CLEAR_INSTRUCTION = 5,
    OFDP_METER_INSTRUCTION = 6
};
// Action definition

enum OfDpActionCommand {
    OFDP_NONE_COMMAND = -1,
    OFDP_OUTPUT=0,           /* Output to switch port. */
    OFDP_SET_VLAN_VID=1,     /* Set the 802.1q VLAN id. */
    OFDP_SET_VLAN_PCP=2,     /* Set the 802.1q priority. */
//    OFDP_STRIP_VLAN=3,       /* Strip the 802.1q header. 1.0 only*/
    OFDP_SET_DL_SRC=4,       /* Ethernet source address. */
    OFDP_SET_DL_DST=5,       /* Ethernet destination address. */
    OFDP_SET_NW_SRC=6,       /* IP source address. */
    OFDP_SET_NW_DST=7,       /* IP destination address. */
    OFDP_SET_NW_TOS=8,       /* IP Tos */
    OFDP_SET_TP_SRC=9,       /* TCP/UDP source port. */
    OFDP_SET_TP_DST=10,      /* TCP/UDP destination port. */
    OFDP_ENQUEUE=11,         /* Enqueue packet */
    OFDP_SET_MPLS_LABEL=12,  /* Set the MPLS label. */
    OFDP_SET_MPLS_EXP=13,    /* Set the MPLS EXP bits. */
    OFDP_POP_MPLS=14,        /* Pop MLPS label.          */
    OFDP_PUSH_MPLS=15,       /* Push MPLS label.         */
    OFDP_POP_VLAN=16,        /* Pop Vlan tag  */
    OFDP_PUSH_VLAN_VID=17,   /* Push Vlan tag  */
    OFDP_DEC_MPLS_TTL=18,    /* Decrement MPLS TTL */
    OFDP_SET_MPLS_TTL=19,    /* Set MPLS TTL */
    OFDP_GROUP = 20,         /* Output to Group */
    OFDP_METER = 21,         /* Apply meter (Instruction) */
    OFDP_SET_SIG_TYPE = 22,  /* ODU SIG TYPE*/
    OFDP_SET_SIG_ID = 23,    /* TPN and TimeSlots */
    OFDP_MAX_COMMAND         /* !!! Must be last with the value one grater that the last valid cmd !!! */
};

static inline const char* toString( OfDpActionCommand action )
{
    switch( action )
    {
    case OFDP_OUTPUT:
        return( "Output" );
        break;
    case OFDP_SET_VLAN_VID:
        return( "Set VLAN VID" );
        break;
    case OFDP_SET_VLAN_PCP:
        return( "Set VLAN PCP" );
        break;
    case OFDP_SET_DL_SRC:
        return( "Set Dl Src" );
        break;
    case OFDP_SET_DL_DST:
        return( "Set Dl Dst" );
        break;
    case OFDP_SET_NW_SRC:
        return( "Set Nw Src" );
        break;
    case OFDP_SET_NW_DST:
        return( "Set Nw Dst" );
        break;
    case OFDP_SET_NW_TOS:
        return( "Set Nw Tos" );
        break;
    case OFDP_SET_TP_SRC:
        return( "Set Tp Src" );
        break;
    case OFDP_SET_TP_DST:
        return( "Set Tp Dst" );
        break;
    case OFDP_ENQUEUE:
        return( "Enqueue" );
        break;
    case OFDP_SET_MPLS_LABEL:
        return( "Set Mpls Label" );
        break;
    case OFDP_SET_MPLS_EXP:
        return( "Set Mpls Exp" );
        break;
    case OFDP_SET_SIG_TYPE:
        return( "Set ODU Sig Type" );
        break;
    case OFDP_SET_SIG_ID:
        return( "Set ODU Sig ID" );
        break;
    case OFDP_POP_MPLS:
        return( "Pop Mpls" );
        break;
    case OFDP_PUSH_MPLS:
        return( "Push Mpls" );
        break;
    default:
        break;
    }

    return( "UNKNOWN");
}

struct OfPdOutput {
    uint32_t port;
    uint16_t max_len;
    OfPdOutput(uint32_t p=0, uint32_t l=0) : port(p), max_len(l) {}
    OfPdOutput(const OfPdOutput& obj) = default;
};
std::ostream& operator<<(std::ostream& s, const OfPdOutput& obj);

struct OfPdVlan {
    uint16_t vid;
    uint16_t ethertype; // ethertype for vlan - 1.2
    uint8_t  pcp;
    OfPdVlan(uint16_t v=0, uint16_t e=0, uint8_t p=0) : vid(v), ethertype(e), pcp(p) {}
    OfPdVlan(const OfPdVlan& obj) = default;
};
std::ostream& operator<<(std::ostream& s, const OfPdVlan& obj);

struct OfPdMpls{
    uint32_t label;
    uint8_t  exp;
    uint8_t  ttl;
    uint16_t ethertype; // ethertype for mpls - 1.2
    OfPdMpls (uint32_t l=0, uint8_t e=0, uint8_t t=0, uint16_t=0)
        : label(l), exp(e), ttl(t), ethertype(e) {}
    OfPdMpls(const OfPdMpls& obj) = default;
};
std::ostream& operator<<(std::ostream& s, const OfPdMpls& obj);

struct OfPdQueue{
    uint32_t port;
    uint32_t queue;
    OfPdQueue (uint32_t p=0, uint32_t q=0) : port(p), queue(q) {}
    OfPdQueue(const OfPdQueue& obj) = default;
};
std::ostream& operator<<(std::ostream& s, const OfPdQueue& obj);

struct SigId{
    uint16_t    tpn;
    uint16_t    tslen;
    uint8_t     timeslots[MAX_TSLEN_BYTES];
    SigId():tpn(0),tslen(0),timeslots{0,0,0,0,0,0,0,0,
                                      0,0,0,0}
    {
    }
};

union OfDpActionDataType {
    uint64_t      data[2];
    OfPdOutput    output;
    OfPdVlan      vlan;
    uint8_t       dl_addr[ OFDP_MAC_LEN ];
    uint32_t      nw_addr;
    uint16_t      tp_port;
    uint32_t      group_id;
    uint32_t      meter_id;
    OfPdMpls      push_mpls;
    OfPdQueue     queue;
    uint8_t       signal_type;
    SigId         sigid;
    OfDpActionDataType():data{0,0}{}
    OfDpActionDataType(const OfDpActionDataType& obj):data{obj.data[0],
                                                            obj.data[0]}{}
    
};
static_assert(sizeof(OfDpActionDataType)==16,"OfDpActionDataType size mismatch");

struct OfDpActionT {
    OfDpActionCommand command;
    OfDpActionDataType value;
    OfDpActionT():command(OFDP_NONE_COMMAND)
    {
    }
    OfDpActionT(const OfDpActionT& obj)=default;
};

std::ostream& operator<<(std::ostream& s, const OfDpActionT& oflow);


// Match definition
enum OfDpFlowWildcards {
    OFDP_IN_PORT  = 1 << 0,  /* Switch input port. */
    OFDP_DL_VLAN  = 1 << 1,  /* VLAN. */
    OFDP_DL_SRC   = 1 << 2,  /* Ethernet source address. */
    OFDP_DL_DST   = 1 << 3,  /* Ethernet destination address. */
    OFDP_DL_TYPE  = 1 << 4,  /* Ethernet frame type. */
    OFDP_NW_PROTO = 1 << 5,  /* IP protocol. */
    OFDP_TP_SRC   = 1 << 6,  /* TCP/UDP source port. */
    OFDP_TP_DST   = 1 << 7,  /* TCP/UDP destination port. */
    OFDP_NW_SRC = 1 << 8,
    OFDP_NW_DST =  1 << 9,
    OFDP_DL_VLAN_PCP = 1 << 10,  /* VLAN priority. */
    OFDP_NW_TOS = 1 << 11,  /* IP ToS (DSCP field, 6 bits). */
    OFDP_MPLS_L1       = 1 << 12, /* MPLS Label 1. */
    OFDP_MAX_MATCH     = 13, /* last 1 */
    OFDP_ALL = ((1 << OFDP_MAX_MATCH) - 1)
};
/**
 * Remove wild cards 
 * init with wildcarded or default values 
 */
struct OfDpMatchT {
    uint32_t in_port;          /* Input switch port. */
    std::array<uint8_t,OFDP_MAC_LEN> dl_src; /* Ethernet source address. */
    std::array<uint8_t,OFDP_MAC_LEN> dl_dst; /* Ethernet destination address. */
    uint16_t dl_vlan;          /* Input VLAN. */
    uint16_t dl_type;          /* Ethernet frame type. */
    uint8_t dl_vlan_pcp;       /* Input VLAN priority. */
    uint8_t nw_tos;            /* IP ToS (actually DSCP field, 6 bits). */
    uint8_t nw_proto;          /* IP protocol. */
    uint32_t nw_src;           /* IP source address. */
    uint32_t nw_dst;           /* IP destination address. */
    uint16_t tp_src;           /* TCP/UDP source port. */
    uint16_t tp_dst;           /* TCP/UDP destination port. */
    uint32_t mpls_label1;      /* Top of label stack */
    uint8_t  signal_type;      /* signal type for particular layer class */
    uint16_t tpn;              /* optical trib port# or start time slot*/
    uint16_t tslen;
    std::array<uint8_t,12>  timeslots;         /* time slot map*/         

    OfDpMatchT():in_port(0),dl_vlan(0),dl_type(0),dl_vlan_pcp(0)
    ,nw_tos(0),nw_proto(0),nw_src(0),nw_dst(0)
    ,tp_src(0),tp_dst(0),mpls_label1(0)
    ,signal_type(0),tpn(0),tslen(0)
    {
		dl_src.fill(0);
		dl_dst.fill(0);
        timeslots.fill(0);
    }

    OfDpMatchT(const OfDpMatchT& match)=default;
    OfDpMatchT(OfDpMatchT&&) = default;
    OfDpMatchT& operator = (const OfDpMatchT&) = default;
    OfDpMatchT& operator = (OfDpMatchT&&) = default;

    inline bool operator == (const OfDpMatchT & match)
    {
        const bool ret_val = (in_port == match.in_port &&
                             dl_vlan == match.dl_vlan &&
                             dl_type == match.dl_type &&
                             dl_vlan_pcp == match.dl_vlan_pcp &&
                             nw_tos == match.nw_tos &&
                             nw_proto == match.nw_proto &&
                             nw_src == match.nw_src &&
                             nw_dst == match.nw_dst &&
                             tp_src == match.tp_src &&
                             tp_dst == match.tp_dst &&
                             mpls_label1 == match.mpls_label1 &&
                             dl_src == match.dl_src &&
                             dl_dst == match.dl_dst &&
                             signal_type == match.signal_type &&
                             tpn == match.tpn &&
                             tslen == match.tslen &&
                             timeslots == match.timeslots);
        return ret_val;
    }
};
std::ostream& operator << (std::ostream& s, const OfDpMatchT& match);

/*
 *  OF 1.0 Spec supports - OFDP_OFPFF_SEND_FLOW_REM, OFDP_OFPFF_CHECK_OVERLAP, OFDP_OFPFF_EMERG
 *  OF 1.1, 1.2 Specs supports - OFDP_OFPFF_SEND_FLOW_REM, OFDP_OFPFF_CHECK_OVERLAP, OFDP_RESET_COUNTS
 *  OF 1.3 Spec supports - all except OFDP_OFPFF_EMERG
 *
 *  Values OFDP_OFPFF_EMERG & OFDP_RESET_COUNTS uses the same bit
 */

enum OfDpFlowModFlags {
    OFDP_OFPFF_SEND_FLOW_REM = 1 << 0,
    OFDP_OFPFF_CHECK_OVERLAP = 1 << 1,
    OFDP_OFPFF_EMERG = 1 << 2,
    OFDP_RESET_COUNTS = 1 << 2,
    OFDP_NO_PKT_COUNTS = 1 << 3,
    OFDP_NO_BYT_COUNTS = 1 << 4
};

// Flow update
struct OfDpFlowT {
    OfDpMatchT match;      /* Fields to match */
    /* Flow actions. */
    OfDpFlowCommand command;
    OfDpLogicalSwT* ls_idx;
    OfDpFlowModFlags flags;
	std::string in_port_name;
	std::string out_port_name;
    uint32_t flow_id;
    uint16_t table_id;
    uint16_t stats_flags;
    uint64_t cookie;
    uint64_t cookie_mask; // 1.2+ with FlowMod request but may not need to save
    uint16_t idle_timeout;
    uint16_t hard_timeout;
    uint16_t priority;
    uint32_t out_port;
    uint32_t out_group; // 1.2+
    uint64_t packet_count;
    uint64_t byte_count;
    uint32_t duration_sec;
    uint32_t duration_nsec;
    uint16_t num_actions;
    std::array<OfDpActionT,OFDP_MAX_ACTIONS> actions;

    OfDpFlowT():command(OFDP_FLOW_NONE),ls_idx(0),flags(OFDP_OFPFF_SEND_FLOW_REM)
    ,flow_id(0),table_id(0),stats_flags(0), cookie(0),cookie_mask(0),idle_timeout(0)
    ,hard_timeout(0),priority(0),out_port(OFDP_OFPP_NONE),out_group(OFDP_OFPP_NONE),
    packet_count(0),byte_count(0),duration_sec(0),duration_nsec(0),num_actions(0)
    {
    }

    OfDpFlowT(const OfDpFlowT& match)=default;
    OfDpFlowT(OfDpFlowT&&) = default;
    OfDpFlowT& operator = (const OfDpFlowT&) = default;
    OfDpFlowT& operator = (OfDpFlowT&&) = default;

};

std::ostream& operator<<(std::ostream& s, const OfDpFlowT& oflow);

// Port mod
struct OfDpPortMod {
    uint32_t port_no;
    uint8_t hw_addr[ OFDP_MAC_LEN ];
    uint32_t config;
    uint32_t mask;
    uint32_t advertise;
    OfDpPortMod():port_no(0),hw_addr{0,0,0,0,0,0},config(0),mask(0),advertise(0)
    {
    }
    OfDpPortMod(const OfDpPortMod& match)=default;
    OfDpPortMod(OfDpPortMod&&) = default;
    OfDpPortMod& operator = (const OfDpPortMod&) = default;
    OfDpPortMod& operator = (OfDpPortMod&&) = default;
};

// Packet out
struct OfDpPacketOutT {
    uint32_t buffer_id;
    uint32_t in_port;
    uint16_t num_actions;
    std::array<OfDpActionT,OFDP_MAX_ACTIONS> actions;
    uint32_t message_len;
    char message[ OFDP_MAX_MSG_LEN ];
    OfDpPacketOutT():buffer_id(0),in_port(0),num_actions(0),message_len(0)
    {
        message[0] = 0;
    }
    OfDpPacketOutT(const OfDpPacketOutT& match)=default;
    OfDpPacketOutT(OfDpPacketOutT&&) = default;
    OfDpPacketOutT& operator = (const OfDpPacketOutT&) = default;
    OfDpPacketOutT& operator = (OfDpPacketOutT&&) = default;
};

// Stats messages
struct OfDpDescStatsT {
    char mfr_desc[ OFDP_DESC_STR_LEN ];
    char hw_desc[ OFDP_DESC_STR_LEN ];
    char sw_desc[ OFDP_DESC_STR_LEN ];
    char serial_num[ OFDP_SERIAL_NUM_LEN ];
    char dp_desc[ OFDP_DESC_STR_LEN ];
    OfDpDescStatsT(){mfr_desc[0]=0, hw_desc[0]=0,sw_desc[0]=0,serial_num[0]=0,dp_desc[0]=0;}
    
} ;

// Cookies are not included
struct OfDpFlowStatsReqT {
    OfDpMatchT match;
    OfDpLogicalSwT* ls_idx;
    uint8_t table_id;
    uint64_t cookie;
    uint64_t cookie_mask; // 1.2+ with FlowMod request but may not need to save
    uint32_t out_port;
    uint32_t out_group; // 1.2+
    OfDpFlowStatsReqT():table_id(0),cookie(0),cookie_mask(0),out_port(-1),out_group(-1){}
    OfDpFlowStatsReqT(const OfDpFlowStatsReqT& match)=default;
    OfDpFlowStatsReqT(OfDpFlowStatsReqT&&) = default;
    OfDpFlowStatsReqT& operator = (const OfDpFlowStatsReqT&) = default;
    OfDpFlowStatsReqT& operator = (OfDpFlowStatsReqT&&) = default;
};

typedef OfDpFlowT OfDpFlowStatsT;

struct OfDpAggregateStatsReqT {
    OfDpMatchT match;
    OfDpLogicalSwT* ls_idx;
    uint8_t table_id;
    uint64_t cookie;
    uint64_t cookie_mask; // 1.2+ with FlowMod request but may not need to save
    uint32_t out_port;
    uint32_t out_group; // 1.2+
    OfDpAggregateStatsReqT():table_id(0),cookie(0),cookie_mask(-1),out_port(0),out_group(0){}
} ;

struct OfDpAggregateStatsT {
    uint64_t packet_count;
    uint64_t byte_count;
    uint32_t flow_count;
    OfDpAggregateStatsT():packet_count(0),byte_count(0),flow_count(0){}
};

struct OfDpTableStatsT {
    uint16_t flags;
    OfDpLogicalSwT* ls_idx;
    uint8_t table_id;
    char name[ OFDP_OFP_MAX_TABLE_NAME_LEN ];
    uint32_t wildcards;
    uint32_t max_entries;
    uint32_t active_count;
    uint64_t lookup_count;
    uint64_t matched_count;
    OfDpTableStatsT():flags(0),table_id(0),wildcards(0),max_entries(0)
    ,active_count(0),lookup_count(0),matched_count(0)
    {}
};

struct OfDpPortStatsReqT {
    uint32_t port_no;
    OfDpPortStatsReqT(const uint32_t port = 0):port_no(port)
    {}
};

struct OfDpPortStatsT {
    uint16_t flags;
    uint32_t port_no;
    uint64_t rx_packets;
    uint64_t tx_packets;
    uint64_t rx_bytes;
    uint64_t tx_bytes;
    uint64_t rx_dropped;
    uint64_t tx_dropped;
    uint64_t rx_errors;
    uint64_t tx_errors;
    uint64_t rx_frame_err;
    uint64_t rx_over_err;
    uint64_t rx_crc_err;
    uint64_t collisions;
    uint32_t duration_sec;
    uint32_t duration_nsec;
    OfDpPortStatsT():flags(0),port_no(0),rx_packets(0),tx_packets(0),rx_bytes(0)
    ,tx_bytes(0),rx_dropped(0),tx_dropped(0),rx_errors(0)
    ,tx_errors(0),rx_frame_err(0),rx_over_err(0),rx_crc_err(0)
    ,collisions(0),duration_sec(0),duration_nsec(0)
    {}
};

struct OfDpQueueStatsT {
    uint16_t flags;
    uint32_t port_no;
    uint32_t queue_id;
    uint64_t tx_bytes;
    uint64_t tx_packets;
    uint64_t tx_errors;
    OfDpQueueStatsT():flags(0),port_no(0),queue_id(0)
    ,tx_bytes(0),tx_packets(0),tx_errors(0)
    {}
} ;

struct OfDpBucketCounterT {
    uint64_t packet_count;
    uint64_t byte_count;
    OfDpBucketCounterT():packet_count(0),byte_count(0)
    {}
} ;

struct OfDpGroupStatsT {
    uint16_t flags;
    OfDpLogicalSwT*  ls_idx;
    uint32_t group_id;
    uint32_t ref_count;
    uint64_t packet_count;
    uint64_t byte_count;
    uint32_t duration_sec;
    uint32_t duration_nsec;
    uint16_t num_bucket_stats;
    std::array<OfDpBucketCounterT,OFDP_MAX_BUCKETS> bucket_stats;
    OfDpGroupStatsT():flags(0),group_id(0),ref_count(0),packet_count(0),duration_sec(0),duration_nsec(0),num_bucket_stats(0)
    {}
};

struct OfDpBucketT {
    uint16_t weight;
    uint32_t watch_port;
    uint32_t watch_group;
    uint16_t num_actions;
    std::array<OfDpActionT,OFDP_MAX_ACTIONS> actions;
    OfDpBucketT():weight(0),watch_port(0),watch_group(0),num_actions(0)
    {}
};

std::ostream& operator<<(std::ostream& s, const OfDpBucketT& obkt);


// Group update

struct OfDpGroupT {
    typedef uint32_t key_type;
    key_type group_id;
    OfDpConfigCommand command;
    bool adminState;
    OfDpGroupType type;
    OfDpLogicalSwT* ls_idx;
    // Add buckets later, just actions set for now...
    uint16_t num_buckets;
    std::array<OfDpBucketT,OFDP_MAX_BUCKETS> buckets;
    OfDpGroupT():group_id(0),command(OFDP_CONFIG_NONE),adminState(true),type(OFDP_GROUP_ALL),ls_idx(0),num_buckets(0)
    {}
    OfDpGroupT(const OfDpGroupT& obj)= default;
    OfDpGroupT(OfDpGroupT&&) = default;
    OfDpGroupT& operator = (const OfDpGroupT& obj) = default;
    OfDpGroupT& operator = (OfDpGroupT&&) = default;
    const key_type& key() const { return group_id; }

};

std::ostream& operator<<(std::ostream& s, const OfDpGroupT& ogroup);

// Get Group
struct OfDpGroupDescStatsT {
    uint16_t flags;
    uint8_t type;
    uint32_t group_id;
    uint16_t num_buckets;
    std::array<OfDpBucketT,OFDP_MAX_BUCKETS> buckets;
    OfDpGroupDescStatsT():flags(0),type(0),group_id(0),num_buckets(0)
    {}
};
std::ostream& operator<<(std::ostream& s, const OfDpGroupDescStatsT& ogroup);


struct OfDpGroupFeature {
    uint32_t types;
    uint32_t capabilities;
    std::array<uint32_t,OFDP_GROUP_MAX_TYPE> max_groups;
    std::array<uint32_t,OFDP_GROUP_MAX_TYPE>  actions;
    OfDpGroupFeature():types(0),capabilities(0)
    {
        max_groups.fill(0);
        actions.fill(0);
    }
};

struct OfDpGroupFeatureStatsT {
    uint16_t flags;
    OfDpGroupFeature feature;
    OfDpGroupFeatureStatsT():flags(0)
    {}
};

enum OfDpTableConfig {
    OFDP_TABLE_MISS_CONTROLLER =    0,
    OFDP_TABLE_MISS_CONTINUE =    1 << 0,
    OFDP_TABLE_MISS_DROP =    1 << 1,
    OFDP_TABLE_MISS_MASK =    3
};

struct OfDpTableFeatureT {
    uint8_t table_id;
    char    name[OFDP_OFP_MAX_TABLE_NAME_LEN];
    uint64_t metadata_match;
    uint64_t metadata_write;
    uint32_t config;
    uint32_t max_entries;
    uint32_t instructions; // bitmap
    uint32_t instructions_miss; // bitmap
    uint64_t next_tables; // each byte is a table id - max next table from current table is 8 - or it can be a bitmap
    uint64_t next_tables_miss; // each byte is a table id - max next table from current table is 8 - or it can be a bitmap
    uint64_t write_actions; // bitmap
    uint64_t write_actions_miss; // bitmap
    uint64_t apply_actions;
    uint64_t apply_actions_miss;
    uint64_t match_fields;
    uint64_t wildcard_fields;
    uint64_t write_set_fields;
    uint64_t write_set_fields_miss;
    uint64_t apply_set_fields;
    uint64_t apply_set_fields_miss;
    uint32_t actions;
    OfDpTableFeatureT():table_id(0),metadata_match(0),metadata_write(0),config(0)
    ,max_entries(0),instructions(0),instructions_miss(0)
    ,next_tables(0),next_tables_miss(0),write_actions(0),write_actions_miss(0)
    ,apply_actions(0),apply_actions_miss(0),match_fields(0),wildcard_fields(0)
    ,write_set_fields(0),write_set_fields_miss(0),actions(0)
    {
        name[0] = 0;
    }
};

struct OfDpTableFeatureStatsT {
    uint16_t flags;
    OfDpTableFeatureT feature;
    OfDpTableFeatureStatsT():flags(0)
    {}
};
// Async messages

// Port update

enum OfDpPortReasonT {
    OFDP_OFPPR_ADD,
    OFDP_OFPPR_DELETE,
    OFDP_OFPPR_MODIFY
} ;

struct OfDpPortStatusT {
    OfDpPortReasonT reason;
    OfDpPortInfoT desc;
    OfDpPortStatusT():reason(OFDP_OFPPR_ADD)
    {}
} ;

// Packet in

enum OfDpPacketInReasonT {
    OFDP_OFPR_NO_MATCH,
    OFDP_OFPR_ACTION,
    OFDP_OFPR_INVALID_TTL  // 1.2+
};

struct OfDpPacketInT {
    uint32_t buffer_id;
    uint16_t total_len;
    uint32_t in_port;
    OfDpPacketInReasonT reason;
    uint8_t  table_id; // 1.2+
    char message[ OFDP_MAX_MSG_LEN ];
    OfDpPacketInT():buffer_id(0),total_len(0),in_port(0),reason(OFDP_OFPR_NO_MATCH),table_id(0)
    {}
};

// Flow removed
enum OfDpFlowRemovedReasonT {
    OFDP_OFPRR_IDLE_TIMEOUT,
    OFDP_OFPRR_HARD_TIMEOUT,
    OFDP_OFPRR_DELETE,
    OFDP_OFPRR_GROUP_DELETE
};

struct OfDpFlowRemovedT {
    OfDpMatchT match;
    uint64_t cookie;
    uint16_t priority;
    uint8_t reason;
    uint8_t table_id;
    uint32_t duration_sec;
    uint32_t duration_nsec;
    uint16_t idle_timeout;
    uint64_t packet_count;
    uint64_t byte_count;

    OfDpFlowRemovedT():cookie(0),priority(0),reason(0),table_id(0),duration_sec(0),duration_nsec(0)
    ,idle_timeout(0),packet_count(0),byte_count(0)
    {}
};

// Queue definition

enum OfDpQueuePropertyTypeT {
    OFDP_OFPQT_NONE=0,
    OFDP_OFPQT_MIN_RATE
};

struct OfDpQueuePropertyDataT {
    uint16_t rate;
    OfDpQueuePropertyDataT():rate(0)
    {}
};

struct OfDpQueuePropertyT {
    OfDpQueuePropertyTypeT property;
    OfDpQueuePropertyDataT data;
    OfDpQueuePropertyT():property(OFDP_OFPQT_NONE)
    {}
} ;

 struct OfDpQueueT {
    uint32_t queue_id;
    uint16_t num_properties;
    OfDpQueuePropertyT properties[ OFDP_MAX_QUEUE_PROP ];
    OfDpQueueT():queue_id(0),num_properties(0)
    {}
} ;

 struct OfDpQueueConfigT {
    uint32_t port;
    uint32_t num_queues;
    OfDpQueueT queues[ OFDP_MAX_QUEUE_PER_PORT ];
    OfDpQueueConfigT():port(0),num_queues(0)
    {}
} ;

 struct OfDpResponseT {
    OfDpReturn return_code;
    const int socket_fd;
    uint8_t msg[64];

    OfDpResponseT(const int fd=0, const uint8_t* buf=0, unsigned short len=0): return_code(OFDP_OK),socket_fd(fd)
    {
        if ( len <= 64 && buf ) memcpy(msg, buf, len);
        if ( len < 64 ) memset(msg+len,0, 64-len);
    }
    OfDpResponseT(const OfDpResponseT& ref):return_code(ref.return_code),socket_fd(ref.socket_fd)
    {
        memcpy(msg, ref.msg, 64);
    }
} ;

	void  etherstr(std::ostream& , const std::array<uint8_t,OFDP_MAC_LEN>& );
    const char* toString(const OfDpVersion version);
    #define GET_VERSION(msg) *static_cast<const uint8_t*>(msg)

    #pragma pack(pop)
#endif
