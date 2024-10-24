#ifndef OfDpController_h_
#define OfDpController_h_
/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "OfConfig.hpp"
#include "GenericObservableConfig.hpp"
/**
 * Observer pattern implementation
 */
class OfDpController : public GenericObservableConfig<OfDpControllerT> {
    // TODO: define any other notification functions
    // for observing subjects.  This is an interface,
    // so all functions must be pure virtual ones.
public:
    virtual ~OfDpController(){}

    friend std::ostream& operator << (std::ostream& out, OfDpController& );
    // Exact match comparison for Controller.
    bool operator == (const OfDpControllerT& rhs) const
    {
        return( this->ipAddress == rhs.ipAddress &&
                this->port == rhs.port );
    }
    friend class OfDpConfigApi;
protected:
    OfDpController(const OfDpControllerT& obj):GenericObservableConfig<OfDpControllerT>(obj){}

};

#endif
