/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include <typeinfo>
#include <string>
#include <cstring>
#include <cassert>
#include <climits>

#include "TcpClient.hpp"
#include "SslClient.hpp"
#include "OfpHeader.h"
#include "AbstractConn.hpp"
#include "OfConnManager.hpp"
#include "OfpHeader.h"
#include "OfConnStateManager.hpp"
#include "OfConnStateObjects.hpp"
#include "OFV4Parser.hpp"
#include "OfDpDefinePD.h"
#include "OfDpLogicalSw.h"

// For logging
#include <cstdio>

time_t StateObjectInterface::Timeout(__attribute__((unused)) const time_t now) const
{
    const AbstractConn& lVconn = mStateContext.mVconn;
    return std::max(lVconn.GetProbeInterval(), lVconn.GetBackOff());
}

bool iVconnStartState::Commit( const time_t now)
{
    AbstractConn& lVconn = mStateContext.mVconn;
    if(mStateContext.get_mStateEnteredTime() < now){
    	lVconn.Close(); // if possible close now
    }
    return true;
}

bool iVconnStartState::Run(const time_t now)
{
    return mStateContext.TransitionTo(OfConnStateManager::S_CONNECTING, now);

}

bool iVconnConnectingState::Commit(const time_t now)
{
    AbstractConn& lVconn = mStateContext.mVconn;
    bool retVal = lVconn.IsOpen();
    if ( !retVal ) {
        retVal = lVconn.Reconnect(now);
    }
    if ( retVal ) {
        retVal = mStateContext.TransitionTo(OfConnStateManager::S_CONNECTED, now);
    }
    return retVal;
}

bool iVconnConnectingState::Run(const time_t now) {
	return false;
}

bool iVconnConnectedState::Commit(const time_t now)
{
    AbstractConn& lVconn = mStateContext.mVconn;
    if (lVconn.send_hello(0)) {
        if (!lVconn.Receive(now) && lVconn.TimedOut(now) ) {
            lVconn.BackOff(now);
        }
        return true;
    } else {
        return false;
    }
}

bool iVconnConnectedState::Run(const time_t now)
{
    AbstractConn& lVconn = mStateContext.mVconn;
    if ( !lVconn.Receive(now) && lVconn.TimedOut(now) ) {
        lVconn.BackOff(now); // it is to close stale connection
    }
    return true;
}

bool iVconnActiveState::Commit(const time_t now)
{
    mStateContext.mVconn.ResetBackoff(now); // reset BackOff so that it stays in Active State
    DBG( DBG_DETAIL|DBG_PARSER_STATE, (" iVconnActiveState::Commit done!\n" ) );
    return true;
}

bool iVconnActiveState::Run(const time_t now)
{
    AbstractConn& lVconn = mStateContext.mVconn;
    bool retVal = false;
    // try to send any packets (max 5) saved before
    // regardless if we should read or not
    // regardless if we back off or not
    lVconn.SendQueue();
    if ( lVconn.Receive(now) ) {
        retVal = true;
        lVconn.ResetBackoff(now);
    } else if ( lVconn.TimedOut(now) ) {
        lVconn.BackOff(now);
    }
    return retVal;
}

bool iVconnIdleState::Commit(const time_t now)
{
    AbstractConn& lVconn = mStateContext.mVconn;
    lVconn.ResetBackoff(now); // reset BackOff & send echo probe
    const bool retVal = lVconn.mpParser->send_echo();
    const char* func = " iVconnIdleState::Commit ";
    DBG( DBG_DETAIL|DBG_PARSER_STATE, (" %s send_echo %s\n", func, retVal?"success":"failed" ) );
    return true; // we always accept to idle state even if send_echo fails
}

bool iVconnIdleState::Run(const time_t now)
{
    AbstractConn& lVconn = mStateContext.mVconn;
    bool retVal = false;
    if ( lVconn.Receive(now) ) {
        mStateContext.TransitionTo(OfConnStateManager::S_ACTIVE, now);
        retVal = true;
    } else if ( lVconn.TimedOut(now) ) {
        retVal = lVconn.mpParser->send_echo();
        lVconn.BackOff(now);
    }
    return retVal;
}

bool iVconnDeadState::Commit(__attribute__((unused)) const time_t now)
{
    return true; // we always accept it
}

bool iVconnDeadState::Run(const time_t now){
	return false;
}

