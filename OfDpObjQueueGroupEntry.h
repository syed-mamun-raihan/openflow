#ifndef OfDpObjQueueGroupEntry_h_
#define OfDpObjQueueGroupEntry_h_
/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include "OfDpObjEntry.h"

class OfDpObjQueueGroupEntry :
    public OfDpObjEntry
{
public:
    // Only can set prior to activation.
    virtual OfDpReturn setPgid( uint32_t pgidIn )=0;

    // Get latest view of queue group from Saos.
    virtual OfDpReturn update( OfDpQueueConfigT &queueInfo )=0;

    // Get queue stats
    virtual OfDpReturn getStats( uint32_t queueIdx,
                                 OfDpQueueStatsT &stats )=0;

protected:
    OfDpObjQueueGroupEntry( OfDpObjPlatform platform, OfDpObjType objType ) :
        OfDpObjEntry( platform, objType ) {}
};

#endif
