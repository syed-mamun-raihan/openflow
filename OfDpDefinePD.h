
#ifndef OfDpDefinePD_h_
#define OfDpDefinePD_h_
/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include <stdint.h>
#include "OfConfig.hpp"
#include "OfDpDefine.hpp"

struct DsDbRecordStatus
{
public:
    int mCounts[3]; // good count, bad count, error code of last bad count

    DsDbRecordStatus(int initVal=0) 
    {
        resetAllCnt(initVal);
    }

    void resetAllCnt(int initVal)
    {
        resetCnt(initVal);
        resetRejectCnt(initVal);
    }

    void resetCnt(int initVal)
    {
        mCounts[0]=initVal;
    }

    void resetRejectCnt(int initVal)
    {
        mCounts[1]=initVal;
        mCounts[2]=initVal;
    }

    void incrCnt() 
    {
        mCounts[0]++; 
    };
    void decrCnt() 
    {
        if (mCounts[0] > 0)
            mCounts[0]--; 
    };

    void incrRejectCnt(int rejectReason=0) // TBD: need an unknown reason to replace this 
    {
        mCounts[1]++;
        mCounts[2] = rejectReason;
    };

    int getCnt() 
    {
        return mCounts[0]; 
    };

    int getRejectCnt(int &rejectReason) 
    {
        rejectReason = mCounts[2];
        return mCounts[1]; 
    };

private:
        ;
};


// Following externs are defined in iVConn.cpp, used for performance debug
extern struct DsDbRecordStatus flowstats;
extern uint32_t gDsDbPerfDebugFlag;
extern struct timeval perf_starttime;
extern int gDispMplsFlowCount;
extern int gDispVlanFlowCount;

#endif
