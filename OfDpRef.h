#ifndef OfDpRef_h_
#define OfDpRef_h_

/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */

#include <stdint.h>

class OfDpRef
{
public:
    OfDpRef():_refCount(0) {}
    ~OfDpRef(){}

    uint32_t getRefCount() const { return( _refCount ); }
    uint32_t incRefCount() { return( ++_refCount ); }
    uint32_t decRefCount() { return( --_refCount ); }

private:
    uint32_t _refCount;
};

#endif
