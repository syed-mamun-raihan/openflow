#include <cstdio>
#include <iostream>
/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include "OfDpGroup.h"

// Output
std::ostream& operator<<( std::ostream& out, const OfDpGroup& instance )
{
    uint32_t i;

    out << "Group Id=" << instance.getId() << ", Group Type=" << toString( instance.getType() ) 
		<< " Action={";

    for ( i = 0; i < instance.getNumActions(); i++ )
      {
          out << instance.getAction(i);
          if ( i != (instance.getNumActions()-1) )
          {
              out << ", ";
          }
      }

      out << "}" << std::endl;

    return( out );
}

std::ostream& OfDpGroup::doActionOut( std::ostream& out, const OfDpActionT& action ) const
{
    switch( action.command )
    {
    case OFDP_OUTPUT:
        out << "out_port=" << action.value.output.port;
        break;
    case OFDP_SET_DL_DST:
        char buffer[20];
        sprintf( buffer, "0x%x:%x:%x:%x:%x:%x",
                action.value.dl_addr[0],
                action.value.dl_addr[1],
                action.value.dl_addr[2],
                action.value.dl_addr[3],
                action.value.dl_addr[4],
                action.value.dl_addr[5] );
        out << "set_dl_dst=" << buffer;
        break;
    case OFDP_PUSH_MPLS:
        out << "push_label=" << action.value.push_mpls.label;
        break;
    case OFDP_POP_MPLS:
        out << "pop_mpls";
        break;
    default:
        break;
    }
	return( out );
}

const char* toString( const OfDpGroupType groupType )
{
    switch( groupType )
    {
    case OFDP_GROUP_ALL:
        return( "All" );
        break;
    case OFDP_GROUP_SELECT:
        return( "Select" );
        break;
    case OFDP_GROUP_INDIRECT:
        return( "Indirect" );
        break;
    case OFDP_GROUP_FF:
        return( "Fast failover" );
        break;
    default:
        break;
    }

    return( "UNKNOWN" );
}


