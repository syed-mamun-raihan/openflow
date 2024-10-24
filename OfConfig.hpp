
#ifndef OFCONFIG_H_
#define OFCONFIG_H_
/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#ifndef __cplusplus
	#error "C is not supported, please use C++11 compatible compiler, like g++4.7.3"
#endif
	#include "ofDebug.h"
	
// all internal structs here 32bit aligned
// all defs/macros used in Cofa and ofMgr are defined here
// all structs for ipc are defined here too
// Openflow parsing related enums and structs are defined in OfDpDefine.hpp
#pragma pack(push,4)
/////////////////////////////////// common defs for accross process ////
/* Special buffer-id to indicate 'no buffer' */
#define OFP_NO_BUFFER 0xffffffff
#define DESC_STR_LEN   256
#define SERIAL_NUM_LEN 32
/* All ones is used to indicate all queues in a port (for stats retrieval). */
#define OFPQ_ALL      0xffffffff
/* Min rate > 1000 means not configured. */
#define OFPQ_MIN_RATE_UNCFG      0xffff

/* Max rate > 1000 means not configured. */
#define OFPQ_MAX_RATE_UNCFG      0xffff


/* The wildcards for ICMP type and code fields use the transport source
* and destination port fields, respectively. */
#define OFPFW_ICMP_TYPE OFPFW_TP_SRC
#define OFPFW_ICMP_CODE OFPFW_TP_DST

/* Values below this cutoff are 802.3 packets and the two bytes
* following MAC addresses are used as a frame length.  Otherwise, the
* two bytes are used as the Ethernet type.
*/
#define OFP_DL_TYPE_ETH2_CUTOFF   0x0600

/* Value of dl_type to indicate that the frame does not include an
* Ethernet type.
*/
#define OFP_DL_TYPE_NOT_ETH_TYPE  0x05ff


/* Define the lowest priority. */
#define OFP_LOWEST_PRIORITY 0x0000

#define OFP_MAX_TABLE_NAME_LEN 32
#define OFP_MAX_PORT_NAME_LEN  16

#define OFP_TCP_PORT  6633
#define OFP_SSL_PORT  6633

#define OFP_ETH_ALEN 6          /* Bytes in an Ethernet address. */
#define OFP_DEFAULT_MISS_SEND_LEN   128
/**
* End of Openflow published definitions above
* Start of  Specific definitions below
*/
#define _OF_JUMBO_PACKET_SIZE  1024*8
#define OFDP_COOKIE_NONE            0
#define OFDP_VLAN_NONE              0xffff
#define OFDP_OFPP_NONE              0xffffffff
#define OFDP_OFPQ_ALL               0xffffffff
#define OFDP_MAX_MSG_LEN            1600
#define OFDP_MAX_QUEUE_PER_PORT     8
#define OFDP_LLDP_ETYPE             0x88cc
#define OFDP_BDDP_ETYPE             0x8942
#define OFDP_IP_ETYPE               0x0800
#define OFDP_8021_ETYPE             0x8100
#define OFDP_MPLS_ETYPE             0x8847
#define OFDP_TABLE_ALL              0xff
#define OFDP_STATS_MORE             0x1
#define OFDP_IP_HOST_MASK           0xffffffff

#define OFDP_IS_LLDP( p )        ( (((p)[0] == 0x01) && ((p)[1] == 0x80) && ((p)[2] == 0xC2) && \
								((p)[3] == 0x00) && ((p)[4] == 0x00)) )

#define OFDP_IS_OFDP( p )        ( (((p)[0] == 0x01) && ((p)[1] == 0x23) && ((p)[2] == 0x00) && \
								   ((p)[3] == 0x00) && ((p)[4] == 0x00) && ((p)[5] == 0x01)) )

#define OFDP_IS_BDDP( e )           ( e == OFDP_BDDP_ETYPE )
#define OFDP_IS_LLOFDP( e )         ( e == OFDP_LLDP_ETYPE )


#define OFDP_PKT_UNKNOWN    0
#define OFDP_PKT_LLDP       0x1
#define OFDP_PKT_BDDP       0x2
#define OFDP_PKT_OFDP       0x4
#define OFDP_PKT_ALL        0xff

#define OFDP_VPORT_GLSR_MAX_VPORTS      (2000 * 2)
#define OFDP_VPORT_GLSR_MAX_CHAIN_DEPTH 2
#define OFDP_VPORT_GLSR_MIXED_CHAINING  1


#define OFDP_MAX_VLANID                         4096

#define OFDP_MAX_GROUP_ID         0xffffff00
#define OFDP_ALL_GROUP_ID         0xfffffffc
#define OFDP_ANY_GROUP_ID         0xffffffff
/* Value used in "idle_timeout" and "hard_timeout" to indicate that the entry
* is permanent. */
#define OFP_FLOW_PERMANENT 0

/* By default, choose a priority in the middle. */
#define OFP_DEFAULT_PRIORITY 0x8000

#define OFDP_MAC_LEN                6
#define OFDP_MAX_ACTIONS            16
#define OFDP_MAX_QUEUE_PROP         16
#define OFDP_MAX_BUCKETS            16
#define OFDP_IP_LEN                 16
#define OFDP_PORT_LEN               8
#define OFDP_NAME_LEN               32
#define OFDP_FILE_NAME_LEN          128
#define OFDP_MAX_LS                 12   // MAX Logical switches
#define OFDP_MAX_RESOURCES          100  // for config table purposes ?
#define OFDP_DESC_STR_LEN           256
#define OFDP_SERIAL_NUM_LEN         32
#define OFDP_OFP_MAX_TABLE_NAME_LEN 32
#define OFDP_MAX_PORT_NAME_LEN      16
#define OFDP_MAX_ATTACH             2    // No of Controllres per LS
#define OFDP_MAX_CONTROLLER         24   // No of Controllres per LS
#define OFDP_DPID_LEN               8

#define OFDP_MFR_DESC                   ""
#define OFDP_GLSR_MAX_ENTRIES           2000000
#define OFDP_GLSR_HYBRID_MAX_ENTRIES    2000
#define OFDP_SWITCH_MAX_ENTRIES         1000000

#define maxFlowNum 				   1010000

typedef std::array<uint8_t,6> MacBytes;
typedef std::array<uint32_t,2> IpWithMaskBytes;


typedef uint32_t OfDpPktType;
typedef uint8_t dl_mac_type[OFDP_MAC_LEN];


enum OfDpReturn {
    OFDP_OK=0, // always firsrt one
    OFDP_INVALID_NAME,
    OFDP_LS_NOT_EXIST,
    OFDP_CONTROLLER_NOT_EXIST,
    OFDP_PARAM_LEN_ERROR,
    OFDP_DPID_INVALID,
    OFDP_ERR_TABLE_ID,
    OFDP_ENTRY_EXISTS,
    OFDP_ENTRY_NOT_EXIST,
    OFDP_ERR_INPUT,
    OFDP_PENDING,
    OFDP_ERR_NOT_ACTIVATABLE,
    OFDP_ERR_NOT_ACTIVATED,
    OFDP_ERR_ACTIVATED,
    OFDP_ERR_UNKNOWN_COMMAND,
    OFDP_ERR_NO_FREE_ENTRY,
    OFDP_ERR_ATTACHED,
    OFDP_UNSUPPORTED_VERSION,
    OFDP_STATS_NOT_AVAILABLE,
    OFDP_ERR_TABLE_FULL,
    OFDP_ERR_FLOW_OVERLAP,
    OFDP_ERR_NOT_ACTIVATED_PENDING,
    OFDP_INSTRUCTION_BAD_TABLE_ID,
    OFDP_INSTRUCTION_UNSUP_METADATA,
    OFDP_INSTRUCTION_UNSUP_METADATA_MASK,
    OFDP_ACTION_BAD_TYPE,
    OFDP_ACTION_BAD_OUT_PORT,
    OFDP_ACTION_BAD_ARGUMENT,
    OFDP_ACTION_BAD_SET_ARGUMENT,
    OFDP_ACTION_TOO_MANY,
    OFDP_ACTION_BAD_QUEUE,
    OFDP_ACTION_BAD_OUT_GROUP,
    OFDP_ACTION_MATCH_INCONSISTENT,
    OFDP_ACTION_UNSUPPORTED_ORDER,
    OFDP_ACTION_BAD_TAG,
    OFDP_MATCH_BAD_ETHERTYPE,
    OFDP_MATCH_BAD_TYPE,
    OFDP_MATCH_BAD_TAG,
    OFDP_MATCH_BAD_DL_ADDR_MASK,
    OFDP_MATCH_BAD_NW_ADDR_MASK,
    OFDP_MATCH_BAD_WILDCARDS,
    OFDP_MATCH_BAD_FIELD,
    OFDP_MATCH_BAD_VALUE,
    OFDP_MATCH_BAD_MASK,
    OFDP_MATCH_BAD_PREREQ,
    OFDP_MATCH_DUP_FIELD,
    OFDP_GROUP_INVALID_GROUP,
    OFDP_GROUP_WEIGHT_UNSUPPORTED,
    OFDP_GROUP_CHAINING_UNSUPPORTED,
    OFDP_GROUP_WATCH_UNSUPPORTED,
    OFDP_GROUP_LOOP,
    OFDP_GROUP_CHAINED_GROUP,
    OFDP_GROUP_BAD_WATCH,
    OFDP_GROUP_BAD_ID,
    OFDP_GROUP_BAD_TYPE,
    OFDP_GROUP_EXISTS,
    OFDP_GROUP_OUT_OF_GROUPS,
    OFDP_GROUP_OUT_OF_BUCKETS,
    OFDP_GROUP_TOOMANY,
    OFDP_GROUP_BUCKETS_TOOMANY,
    OFDP_GROUP_UNKNOWN,
    OFDP_BUFFER_UNKNOWN,
    OFDP_BAD_FLAG,
    OFDP_BAD_PORT_ID,
    OFDP_BAD_QUEUE_ID,
    OFDP_BAD_OUT_GROUP,
    OFDP_BAD_BUCKET,
    OFDP_BAD_TYPE,
    OFDP_BAD_VALUE,
    OFDP_BAD_WILDCARD,
    OFDP_BAD_MASK,
    OFDP_BAD_CONFIG,
    OFDP_BAD_ADVERTISE,
    OFDP_BAD_TIMEOUT,
    OFDP_ERROR_UNKNOWN,
    OFDP_LINX_RETURN_MAX // last one so always add a new one before it
} ;

enum OfDpVersion {
    OFDP_VERSION_UNKNOWN=0x00,
    OFDP_VERSION_DEFAULT=0x04,
    OFDP_VERSION_V4=0x04, // This is 1.3 Spec -supported
};

// For future Table Type in OF Spec
 enum OfDpProfile {
    OFDP_SINGLE_TABLE_TYPE=0x00
};

// Config items================================================================
enum OfDpConfigCommand {
    OFDP_CONFIG_CREATE=0, // create or add or attach
    OFDP_CONFIG_REMOVE, // remove, delete or detach
    OFDP_CONFIG_MODIFY,
    OFDP_CONFIG_UPDATE_STATE, // state change
    OFDP_CONFIG_NONE // Must be the last command
};

 enum OfDpControllerChannelType {
    OFDP_CHANNEL_ROLE_EQUAL,
    OFDP_CHANNEL_ROLE_MASTER,
    OFDP_CHANNEL_ROLE_SLAVE,
    OFDP_CHANNEL_AUX_EQUAL,
    OFDP_CHANNEL_AUX_MASTER,
    OFDP_CHANNEL_AUX_SLAVE,
    OFDP_CHANNEL_ROLE_MAX
} ;

static inline const char *to_string(const OfDpReturn ret )
{
    switch ( ret ) {
    case OFDP_OK:
        return( "Ok" );
        break;
    case OFDP_ERROR_UNKNOWN:
        return( "Error Unknown" );
        break;
    case OFDP_LS_NOT_EXIST:
        return( "Logical Switch Does Not Exist");
        break;
    case OFDP_CONTROLLER_NOT_EXIST:
        return( "Controller Does Not Exist");
    case OFDP_PARAM_LEN_ERROR:
        return( "Param Length Error");
    case OFDP_DPID_INVALID:
        return( "Invalid Dpid");
    case OFDP_ERR_TABLE_ID:
        return( "Error Table Id" );
        break;
    case OFDP_ENTRY_EXISTS:
        return( "Entry Exists" );
        break;
    case OFDP_ENTRY_NOT_EXIST:
        return( "Entry Does Not Exist" );
        break;
    case OFDP_ERR_INPUT:
        return( "Error Input" );
        break;
    case OFDP_PENDING:
        return( "Pending" );
        break;
    case OFDP_ERR_NOT_ACTIVATABLE:
        return( "Not Activatable" );
        break;
    case OFDP_ERR_NOT_ACTIVATED:
        return( "Not Activated" );
        break;
    case OFDP_ERR_ACTIVATED:
        return( "Already Activated" );
        break;
    case OFDP_ERR_UNKNOWN_COMMAND:
        return( "Unknown Command" );
        break;
    case OFDP_ERR_NO_FREE_ENTRY:
        return( "No Free Entries" );
        break;
    case OFDP_ERR_ATTACHED:
        return( "Attached" );
        break;
    case OFDP_UNSUPPORTED_VERSION:
        return( "Unsupported OF version" );
        break;
    case OFDP_STATS_NOT_AVAILABLE:
        return( "Stats not available" );
        break;
    case OFDP_ERR_TABLE_FULL:
        return( "Table full");
        break;
    case OFDP_ERR_FLOW_OVERLAP:
        return( "Flow overlap" );
        break;
    case OFDP_ERR_NOT_ACTIVATED_PENDING:
        return( "Not Activated or Pending" );
        break;
    case OFDP_ACTION_BAD_TYPE:
        return( "Action Bad Type" );
        break;
    case OFDP_ACTION_BAD_OUT_PORT:
        return( "Action Bad Out Port" );
        break;
    case OFDP_MATCH_BAD_ETHERTYPE:
        return( "Match Bad Ethertype" );
        break;
    case OFDP_BUFFER_UNKNOWN:
        return( "Buffer Unknown" );
        break;
    case OFDP_LINX_RETURN_MAX:
        return( "Linx Return Max" );
        break;
    case OFDP_GROUP_INVALID_GROUP:
        return( "Invalid Group Id" );
        break;
    case OFDP_GROUP_WEIGHT_UNSUPPORTED:
        return(" Group weight unsupported" );
        break;
    case OFDP_GROUP_CHAINING_UNSUPPORTED:
        return(" Group chaining unsupported" );
        break;
    case OFDP_GROUP_WATCH_UNSUPPORTED:
        return(" Group watch unsupported" );
        break;
    case OFDP_GROUP_LOOP:
        return(" Group loop unsupported" );
        break;
    case OFDP_GROUP_CHAINED_GROUP:
        return(" Group chained" );
        break;
    case OFDP_GROUP_BAD_TYPE:
        return(" Group Bad type" );
        break;
    case OFDP_GROUP_BAD_WATCH:
        return(" Group Bad watch" );
        break;
    case OFDP_GROUP_EXISTS:
        return("Group exists" );
        break;
    case OFDP_GROUP_BUCKETS_TOOMANY:
        return("Group Buckets too many" );
        break;
    default:
        break;
    }

    return( "UNKNOWN" );

}

// the *EnumToStr must match CLI values
// Do not change the string - it need to be sync cliMenuXml
static inline const char* to_string(const OfDpVersion version)
{
    switch ( version ) {
    case OFDP_VERSION_V4:
        return "1.3";
    default:
        return "Unknown";
    }
}

static inline bool  from_string(const char *ver, OfDpVersion& version)
{
    if (strcmp(ver, "1.3") == 0)
    {
    	version = OFDP_VERSION_V4;
    	return true;
    }
    else
    {
        version = OFDP_VERSION_UNKNOWN;
    }
    return false;
}


static inline const char* to_string(const OfDpProfile profileType)
{
    switch(profileType) {
        case OFDP_SINGLE_TABLE_TYPE:
        return "SingleTableType";
        break;
        default:
        break;
    }
    return " Unsupported ";
}

static inline bool from_string(const char* profile, OfDpProfile& profileType)
{
    if(strcmp(profile, "SingleTableType")==0) {
    	profileType = OFDP_SINGLE_TABLE_TYPE;
        return true;
    }
    return false;
}

static inline bool from_string(const char *role, OfDpControllerChannelType& channelType)
{
   if (strcmp(role, "MASTER") == 0)
   {
   	channelType = OFDP_CHANNEL_ROLE_MASTER;
   	return true;
   }
   else if (strcmp(role, "SLAVE") == 0)
   {
   	channelType = OFDP_CHANNEL_ROLE_SLAVE;
   	return true;
   }
   else if (strcmp(role, "EQUAL") == 0)
   {
   	channelType = OFDP_CHANNEL_ROLE_EQUAL;
   	return true;
   }
   else if (strcmp(role, "AUXMASTER") == 0)
   {
   	channelType = OFDP_CHANNEL_AUX_MASTER;
   	return true;
   }
   else if (strcmp(role, "AUXSLAVE") == 0)
   {
   	channelType = OFDP_CHANNEL_AUX_SLAVE;
   	return true;
   }
   else if (strcmp(role, "AUXEQUAL") == 0)
   {
   	channelType = OFDP_CHANNEL_AUX_EQUAL;
   }
   else
   {
   	channelType = OFDP_CHANNEL_ROLE_MAX;

   }
   return false;
}

static inline const char* to_string(OfDpControllerChannelType role)
{
   switch (role)
   {
       case OFDP_CHANNEL_ROLE_MASTER:
           return "MASTER";
       case OFDP_CHANNEL_ROLE_SLAVE:
           return "SLAVE";
       case OFDP_CHANNEL_ROLE_EQUAL:
           return "EQUAL";
       case OFDP_CHANNEL_AUX_MASTER:
           return "AUXMASTER";
       case OFDP_CHANNEL_AUX_SLAVE:
           return "AUXSLAVE";
       case OFDP_CHANNEL_AUX_EQUAL:
           return "AUXEQUAL";
       default:
           return "EQUAL";
   }
}


struct OfDpLogicalSwT
{
    typedef std::string key_type;
    OfDpConfigCommand command;
    bool adminState;
    key_type name;
    OfDpVersion version;
    OfDpVersion negotiatedVersion;
    std::array<uint8_t,OFDP_DPID_LEN> dpId;
    OfDpProfile profileType; // not used
    uint16_t flags;
    uint16_t miss_send_len;
    OfDpLogicalSwT():adminState(true)
                    ,version(OFDP_VERSION_V4)
                    ,negotiatedVersion(OFDP_VERSION_V4)
                    ,profileType(OFDP_SINGLE_TABLE_TYPE)
                    ,flags(0)
                    ,miss_send_len(0){dpId.fill(0);}

    OfDpLogicalSwT(const OfDpLogicalSwT& obj)= default;
    OfDpLogicalSwT(OfDpLogicalSwT&&) = default;
    OfDpLogicalSwT& operator = (const OfDpLogicalSwT& obj) = default;
    OfDpLogicalSwT& operator = (OfDpLogicalSwT&&) = default;
    const key_type& key() const { return name; }
};

struct OfDpControllerT
{
    typedef std::string key_type;
    OfDpConfigCommand command;
    bool adminState; 
    key_type name;
    std::string ipAddress; /// String representation
    std::string port; /// String representation
    std::string cafile;
    std::string certfile;
    std::string privfile;
    OfDpControllerT():adminState(true){};
    OfDpControllerT(const OfDpControllerT&) = default;
    OfDpControllerT(OfDpControllerT&&) = default;
    OfDpControllerT& operator = (const OfDpControllerT&) = default;
    OfDpControllerT& operator = (OfDpControllerT&&) = default;
    const key_type& key() const { return name; }
};

struct OfDpControllerAttachmentT
{
    typedef std::string key_type;
    OfDpConfigCommand command; /// ADD == attach, REMOVE == detach
    bool adminState;
    key_type   name; /// attachment needs full controller info
    std::string lsName; /// attached logical switch
    OfDpControllerChannelType role; /// equal = 0, Master = 1, Slave = 2
    time_t probeInterval; /// echo interval
    time_t timeoutInterval; /// Socket closing interval in inactive
    uint32_t maxRetries; /// Not Used
    bool  isTlsSet;/// Set if TLS params are given for a Controller
    bool  mbAutoNegotiation; /// enable or disable auto negotiation
    OfDpControllerAttachmentT():adminState(true)
                                ,role(OFDP_CHANNEL_ROLE_EQUAL)
                                ,probeInterval(0)
                                ,timeoutInterval(0)
                                ,maxRetries(0)
                                ,isTlsSet(false)
                                ,mbAutoNegotiation(false){}
    OfDpControllerAttachmentT(const OfDpControllerAttachmentT&) = default;
    OfDpControllerAttachmentT(OfDpControllerAttachmentT&&) = default;
    OfDpControllerAttachmentT& operator = (const OfDpControllerAttachmentT&) = default;
    OfDpControllerAttachmentT& operator = (OfDpControllerAttachmentT&&) = default;
    const key_type& key() const { return name; }
};

struct OfDpPortInfoT {
    typedef uint32_t key_type;
    OfDpConfigCommand command;
    bool adminState;
    uint32_t port_no;
    uint8_t hw_addr[ OFDP_MAC_LEN ];
    std::string name;
    uint32_t config;
    uint32_t state;
    uint32_t curr;
    uint32_t advertised;
    uint32_t supported;
    uint32_t peer;
    uint32_t curr_speed;
    uint32_t max_speed;
    uint32_t mask; // for v1 ofp_port_mod.mask field
    OfDpPortInfoT():adminState(true)
                    ,port_no(0)
                    ,hw_addr{0}
                    ,config(0)
                    ,state(0)
                    ,curr(0)
                    ,advertised(0)
                    ,peer(0)
                    ,curr_speed(0)
                    ,max_speed(0)
                    ,mask(0){}
    
    OfDpPortInfoT(const OfDpPortInfoT&) = default;
    OfDpPortInfoT(OfDpPortInfoT&&) = default;
    OfDpPortInfoT& operator = (const OfDpPortInfoT&) = default;
    OfDpPortInfoT& operator = (OfDpPortInfoT&&) = default;
    const key_type& key() const { return port_no; }
};

struct OfDpPortAttachmentT {
    typedef uint32_t key_type;
    OfDpConfigCommand command;
    bool adminState;
    std::string name; /// port name
    std::string lsName;
    uint32_t port_no;
    OfDpPortAttachmentT():adminState(true),port_no(0){}
    OfDpPortAttachmentT(const OfDpPortAttachmentT&) = default;
    OfDpPortAttachmentT(OfDpPortAttachmentT&&) = default;
    OfDpPortAttachmentT& operator = (const OfDpPortAttachmentT&) = default;
    OfDpPortAttachmentT& operator = (OfDpPortAttachmentT&&) = default;
    const key_type& key() const { return port_no; }
};



#pragma pack(pop)

#endif /* OFCONFIG_H_ */
