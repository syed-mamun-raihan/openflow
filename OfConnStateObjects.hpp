#ifndef _STATEOBJECTINTERFACE_H
#define _STATEOBJECTINTERFACE_H

#include <iostream>
#include <string>
#include <ctime>
#include <cstdint>
#include <cstring>
#include "OfConnStateManager.hpp"
/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */

class StateObjectInterface {
public:
    explicit StateObjectInterface(OfConnStateManager& rContext, const uint32_t id, const std::string& id_str):
    mStateContext(rContext),
    mTransitionId(id) ,
    mStrName(id_str)
    {
    }

    virtual ~StateObjectInterface(){}

    // Finalize transition to this state
    virtual bool Commit(const time_t now) = 0 ;

    // Currently all states has fixed timeout period
    virtual time_t Timeout(const time_t now) const ;

    virtual bool Run(const time_t now) = 0 ;

    uint32_t get_mTransitionId() const { return mTransitionId;}

    const std::string& get_mStrName() const { return mStrName;}

    bool operator == (const StateObjectInterface& obj){ return mTransitionId == obj.mTransitionId;}

    bool operator != (const StateObjectInterface& obj){ return mTransitionId != obj.mTransitionId;}

protected:
    OfConnStateManager& mStateContext;

    const uint32_t mTransitionId;

    const std::string mStrName;

private: // disable copy CTOR and assignment operator
    StateObjectInterface(const StateObjectInterface&) = delete;
    StateObjectInterface& operator = (const StateObjectInterface&) = delete;
    StateObjectInterface( StateObjectInterface&& ) = delete;
    StateObjectInterface& operator = ( StateObjectInterface&& ) = delete;
};

//1. AddController will add a controller with specfic socket descriptor and OF version expected to accept
//  iVconn enters S_VOID state
class iVconnStartState final : public StateObjectInterface {
public:
    explicit iVconnStartState(OfConnStateManager& rStateContext):
    StateObjectInterface(rStateContext, OfConnStateManager::S_VOID,"S_VOID") {}

    virtual bool Commit(const time_t now) final override;

    virtual bool Run(const time_t now) final override;

};

//2. Upon indication of ReceiveOf message iVconn checks for if mSock is Opened or not
//   if Opened iVconn enters S_CONNECTED state.
class iVconnConnectingState final : public StateObjectInterface {
public:
    explicit iVconnConnectingState(OfConnStateManager& rStateContext):
    StateObjectInterface(rStateContext, OfConnStateManager::S_CONNECTING,"S_CONNECTING") {}

    // on success it always move to connected state
    // else stayed on start state
    virtual bool Commit(const time_t now) final override;
    virtual bool Run(const time_t now) final override;
};

class iVconnConnectedState final : public StateObjectInterface {
public:
    explicit iVconnConnectedState(OfConnStateManager& rStateContext):
    StateObjectInterface(rStateContext, OfConnStateManager::S_CONNECTED,"S_CONNECTED") {}

    virtual bool Commit(const time_t now) final override;

    virtual bool Run(const time_t now) final override;

};

//3. If OFP_VERSION matches or accepted iVconn enters S_ACTIVE state from
//   Connected, Connecting, Start or Idle
class iVconnActiveState final : public StateObjectInterface {
public:
    explicit iVconnActiveState(OfConnStateManager& rStateContext):
    StateObjectInterface(rStateContext, OfConnStateManager::S_ACTIVE,"S_ACTIVE") {}

    virtual bool Commit(const time_t now) final override;

    virtual bool Run(const time_t now) final override;

};

// Backoff timer starts because of missed echo reply / timeout
// Upon expiry of Timeout iVconn check for TimedOut condition.
// If TimedOut iVconn BacksOff and enters FailedSecuredMode(S_IDLE)
// If TimedOut in Idle State it send echo probe
// If rcvd recho_reply in Idle state it changes state to Active
// If it changed state to Idle State we send echo probe too
class iVconnIdleState final: public StateObjectInterface {
public:
    explicit iVconnIdleState(OfConnStateManager& rStateContext):
    StateObjectInterface(rStateContext, OfConnStateManager::S_IDLE,"S_IDLE") {}

    virtual bool Commit(const time_t now) final override;

    virtual bool Run(const time_t now) final override;
};

//iVconn is being removed
class iVconnDeadState final : public StateObjectInterface {
public:
    explicit iVconnDeadState(OfConnStateManager& rStateContext):
    StateObjectInterface(rStateContext, OfConnStateManager::S_DEADMEAT,"S_DEADMEAT") {}

    virtual bool Commit(const time_t now) final override;
    // do not make a dead meat to run
    virtual bool Run(const time_t now) final override;
};
#endif
