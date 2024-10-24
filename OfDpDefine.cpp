/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include "OfConfig.hpp"
#include "OfDpDefine.hpp"
#include "ofDebug.h"

using namespace std;

#define CMDSTR(x) #x":"

ostream& operator <<(ostream& s, const OfDpMatchT& match)
{
    s<< " OfDpMatchT: " << hex << " InPort: " << match.in_port << " VlanId: "
    << match.dl_vlan << " EthType: " << match.dl_type << " Pcp: " << (uint32_t)match.dl_vlan_pcp
    << " Label1: " << match.mpls_label1 ;

    s << hex << ", sigtype: " << (unsigned)match.signal_type
      << ", tpn: " << match.tpn << " tslen: " << match.tslen << " timeslots: ";
      
    for (const auto& slot : match.timeslots) {
        s <<hex << (unsigned)slot << " ";
    }
    s << dec << "; ";
    return s;
}
ostream& operator<<(ostream& s, const OfDpFlowT& oflow)
{
    s << "OfDpFlowT: LsName:" << hex << oflow.ls_idx->name 
    << ", FlowId " << oflow.flow_id << " ,TableId " << (uint32_t)oflow.table_id
    << " , Cookie " << oflow.cookie << " , Cookie Mask " << (uint32_t)oflow.cookie_mask
    << " , out_port " << oflow.out_port << " , out_group " << (uint32_t)oflow.out_group
    << " , Command " << (uint32_t)oflow.command << " , Priority " << (uint32_t)oflow.priority 
    << oflow.match << dec
    << ", Num Actions " << oflow.num_actions << " ";
    for ( int i =0; i < oflow.num_actions; i++ ) {
        s << " " << oflow.actions[i] ;
    }
    s << dec << "\n" << ends;

    return s;
}

ostream& operator<<(ostream& s, const OfDpGroupT& ogroup)
{
    s << "OfDpGroupT struct: " << endl
    << "GroupId " << ogroup.group_id << " ,Command " << (uint32_t)ogroup.command << " ,LsName " << ogroup.ls_idx->name
    << " ,Type " << (uint32_t)ogroup.type << " ,Num Buckets " << ogroup.num_buckets << endl;

    for ( int i=0; i < ogroup.num_buckets; i++ ) {
        s << "\t Bucket [" << i << "]" << ogroup.buckets[i];
    }
    return s;
}

std::ostream& operator<<(std::ostream& s, const OfDpBucketT& obkt)
{
    s << ":weight:" << obkt.weight << " ,watch_port:" << obkt.watch_port
    << " ,watch_group:" << obkt.watch_group << " ,Num Actions " << obkt.num_actions << endl;
    for ( int j=0; j < obkt.num_actions; j++ )
        s << "\t\t [ "<< j << "]:" << obkt.actions[j] << endl;
    return s;
}


ostream& operator<<(ostream& s, const OfDpActionT& oAction)
{
    s << " OfDpActionT: ";
    switch ( oAction.command ) {
    case OFDP_OUTPUT:           /* Output to switch port. */
        s << CMDSTR(OUTPUT_PORT) << " "<< oAction.value.output.port;
        break;
    case OFDP_SET_VLAN_VID:     /* Set the 802.1q VLAN id. */
        s << CMDSTR(SET_VLAN_VID) << " " << oAction.value.vlan.vid;
        break;
    case OFDP_SET_VLAN_PCP:     /* Set the 802.1q priority. */
        s << CMDSTR(SET_VLAN_PCP) << " " << (uint32_t)oAction.value.vlan.pcp;
        break;
    case OFDP_SET_DL_SRC:       /* Ethernet source address. */
        {
            s << CMDSTR(SET_DL_SRC) << " ";
            for(const auto& i : oAction.value.dl_addr) 
            {
				s << hex << i << " ";
			}
            s << dec;
        }
        break;

    case OFDP_SET_DL_DST:       /* Ethernet destination address. */
        {
            s << CMDSTR(SET_DL_DST) << " ";
            for(const auto& i : oAction.value.dl_addr) 
            {
                s << hex << i << " ";
            }
            s << dec;
        }
        break;

    case OFDP_SET_MPLS_LABEL:  /* Set the MPLS label. */
        s << CMDSTR(SET_MPLS_LABEL) << " " << oAction.value.push_mpls.label;
        break;
    case OFDP_POP_MPLS:        /* Pop MLPS label.          */
        s << CMDSTR(POP_MPLS) << " " ;
        break;
    case OFDP_PUSH_MPLS:       /* Push MPLS label.         */
        s << CMDSTR(PUSH_MPLS) << " " << oAction.value.push_mpls.label;
        break;
    case OFDP_POP_VLAN:       /* Pop Vlan tag  */
        s << CMDSTR(POP_VLAN) << " " ;
        break;
    case OFDP_PUSH_VLAN_VID:   /* Push Vlan tag  */
        s << CMDSTR(PUSH_VLAN_VID) << " " << oAction.value.vlan.vid;
        break;
    case OFDP_GROUP:           /* Output to Group */
        s << CMDSTR(OFDP_GROUP) << " " << oAction.value.group_id;
        break;
    case OFDP_SET_NW_SRC:       /* IP source address. */
        s << CMDSTR(OFDP_SET_NW_SRC) << " " << hex << oAction.value.nw_addr;
        break;
    case OFDP_SET_NW_DST:       /* IP destination address. */
        s << CMDSTR(OFDP_SET_NW_DST) << " " << hex << oAction.value.nw_addr;
        break;
    case OFDP_SET_TP_SRC:       /* TCP/UDP source port. */
        s << CMDSTR(OFDP_SET_TP_SRC) << " " << dec << oAction.value.tp_port;
        break;
    case OFDP_SET_TP_DST:      /* TCP/UDP destination port. */
        s << CMDSTR(OFDP_SET_TP_DST) << " " << hex << oAction.value.tp_port;
        break;
    case OFDP_SET_SIG_ID:         /* OTN Sigid */
        {
            s << CMDSTR(OFDP_SET_SIG_ID) << " tpn " << hex << oAction.value.sigid.tpn
              << " tslen " << oAction.value.sigid.tslen << " tsmap " << hex;
            for (const auto& val : oAction.value.sigid.timeslots) {
                s << hex << (unsigned) val <<" ";
            }
            s << dec;
            break;
        }
    case OFDP_SET_SIG_TYPE:         /* OTN Sigtype */
        s << CMDSTR(OFDP_SET_SIG_TYPE) << " " << hex << (unsigned)oAction.value.signal_type;
        break;
    case OFDP_SET_NW_TOS:       /* IP Tos */
    case OFDP_SET_MPLS_EXP:    /* Set the MPLS EXP bits. */
    case OFDP_DEC_MPLS_TTL:    /* Decrement MPLS TTL */
    case OFDP_SET_MPLS_TTL:    /* Set MPLS TTL */
    case OFDP_ENQUEUE:         /* Enqueue packet */
    default:
        s << CMDSTR(UNKNOWN_CMD) << dec;
        break;
    }
    s << "; ";
    return s;
}

std::ostream& operator<<(std::ostream& s, const OfDpGroupDescStatsT& ogroup)
{
    s << "OfDpGroupDescStatsT struct: " << endl
    << "GroupId " << ogroup.group_id << " ,Type " << (uint32_t)ogroup.type << " ,Num Buckets " << ogroup.num_buckets << endl;

    for ( int i=0; i < ogroup.num_buckets; i++ ) {
        s << "\t Bucket [" << i << "]" << ogroup.buckets[i];
    }
    return s;
}



