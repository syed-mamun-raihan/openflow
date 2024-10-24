/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */#include "OfDpController.h"

// For logging
#include <cstdio>

// Output
std::ostream& operator<<( std::ostream& out, OfDpController& instance )
{
    out << "Controller Name=" << instance.name
            << ", ipAddress=" << instance.ipAddress << ", port=" <<
            instance.port << std::endl;

    return( out );
}
