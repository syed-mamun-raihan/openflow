#ifndef OfDpGroup_h_
#define OfDpGroup_h_
/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include <iostream>
#include "OfConfig.hpp"
#include "OfDpDefine.hpp"
#include "OfDpRef.h"
#include "GenericObservableConfig.hpp"

class OfDpGroup :
    public OfDpRef, public GenericObservableConfig<OfDpGroupT> 
{
public:
    OfDpGroup( const OfDpGroupT &config ): GenericObservableConfig<OfDpGroupT>(config) { }
    ~OfDpGroup() {}

    uint32_t getNumBuckets() const { return( GetConfigObject().num_buckets ); }
    const OfDpBucketT& getBuckets( const uint8_t idx ) const { return( GetConfigObject().buckets[ idx ] ); }
    OfDpBucketT& getBuckets( const uint8_t idx ) { return( GetConfigObject().buckets[ idx ] ); }

    uint32_t getNumActions(const uint8_t bucket_num = 0) const { return( GetConfigObject().buckets[bucket_num].num_actions ); }
    OfDpActionT& getAction( const uint8_t idx, const uint8_t bucket_num = 0 ) { return( GetConfigObject().buckets[bucket_num].actions[ idx ] ); }
    const OfDpActionT& getAction( const uint8_t idx, const uint8_t bucket_num = 0 ) const { return( GetConfigObject().buckets[bucket_num].actions[ idx ] ); }

    OfDpGroupType getType() const { return( GetConfigObject().type ); }
    uint32_t getId() const { return( GetConfigObject().group_id ); }

    // Output
    friend std::ostream& operator<<( std::ostream& out, const OfDpGroup& instance );
    std::ostream& doActionOut( std::ostream& out, const OfDpActionT& action ) const;

};
const char* toString( const OfDpGroupType groupType );

#endif // OfDpGroup_h_
