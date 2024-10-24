#ifndef OfDpIndex_h_
#define OfDpIndex_h_
/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include <stdint.h>

class OfDpIndex
{
public:
    OfDpIndex( uint32_t maxIndex ):
    _maxIndex( maxIndex ), _numAllocated(0), _freeIdx(0)
    {
        _bitmap = new uint8_t[ _maxIndex / 8 + (_maxIndex % 8 ? 1 : 0) ];
        memset( _bitmap, 0, _maxIndex / 8 + (_maxIndex % 8 ? 1 : 0) );
    }

    ~OfDpIndex()
    {
        delete[] _bitmap;
    }


    bool reqIndex( uint32_t &indexOut )
{
    uint32_t i;
    uint8_t mask;

    if ( _numAllocated >= _maxIndex )
    {
        return( false );
    }

    indexOut = (8 * _freeIdx);

    for ( i=0, mask=0x1; i < 8; i++, mask<<=1 )
    {
        if ( !(_bitmap[ _freeIdx ] & mask) )
        {
            indexOut += i;
            _bitmap[ _freeIdx ] |= mask;
            _numAllocated++;
            break;
        }
    }

    while ( _bitmap[ _freeIdx ] == 0xff )
    {
        _freeIdx++;
    }

    return( true );
}
    
    void relIndex( uint32_t indexIn )
{
    uint32_t curIdx;
    uint8_t mask;

    curIdx = indexIn / 8;
    mask = 1 << (indexIn % 8);

    _bitmap[ curIdx ] &= ~mask;
    _numAllocated--;

    if ( curIdx < _freeIdx )
    {
        _freeIdx = curIdx;
    }
}

private:
    const uint32_t _maxIndex;
    uint32_t _numAllocated;
    uint8_t *_bitmap;
    uint32_t _freeIdx;
};

#endif
