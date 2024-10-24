/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include <iostream>
#include <string>
#include <limits>
#include <ctime>
#include <cstring>
#include <cassert>
#include <cstdint>
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

const OfConnStateManager::Edge OfConnStateManager::state_edges[] = {
	// for start state
	OfConnStateManager::Edge((int)OfConnStateManager::S_VOID, (int)OfConnStateManager::S_VOID),
	OfConnStateManager::Edge((int)OfConnStateManager::S_VOID, (int)OfConnStateManager::S_CONNECTING),

    // for connecting state
	OfConnStateManager::Edge((int)OfConnStateManager::S_CONNECTING, (int)OfConnStateManager::S_CONNECTING),
	OfConnStateManager::Edge((int)OfConnStateManager::S_CONNECTING, (int)OfConnStateManager::S_CONNECTED),
	OfConnStateManager::Edge((int)OfConnStateManager::S_CONNECTING, (int)OfConnStateManager::S_ACTIVE),
	OfConnStateManager::Edge((int)OfConnStateManager::S_CONNECTING, (int)OfConnStateManager::S_VOID),
	OfConnStateManager::Edge((int)OfConnStateManager::S_CONNECTING, (int)OfConnStateManager::S_DEADMEAT),

    // for connected state
	OfConnStateManager::Edge((int)OfConnStateManager::S_CONNECTED, (int)OfConnStateManager::S_CONNECTED),
	OfConnStateManager::Edge((int)OfConnStateManager::S_CONNECTED, (int)OfConnStateManager::S_ACTIVE),
	OfConnStateManager::Edge((int)OfConnStateManager::S_CONNECTED, (int)OfConnStateManager::S_VOID),
	OfConnStateManager::Edge((int)OfConnStateManager::S_CONNECTED, (int)OfConnStateManager::S_DEADMEAT),

    // for active state
	OfConnStateManager::Edge((int)OfConnStateManager::S_ACTIVE, (int)OfConnStateManager::S_ACTIVE),
	OfConnStateManager::Edge((int)OfConnStateManager::S_ACTIVE, (int)OfConnStateManager::S_IDLE),
	OfConnStateManager::Edge((int)OfConnStateManager::S_ACTIVE, (int)OfConnStateManager::S_VOID),
	OfConnStateManager::Edge((int)OfConnStateManager::S_ACTIVE, (int)OfConnStateManager::S_DEADMEAT),

	// for idle state
	OfConnStateManager::Edge((int)OfConnStateManager::S_IDLE, (int)OfConnStateManager::S_IDLE),
	OfConnStateManager::Edge((int)OfConnStateManager::S_IDLE, (int)OfConnStateManager::S_ACTIVE),
	OfConnStateManager::Edge((int)OfConnStateManager::S_IDLE, (int)OfConnStateManager::S_VOID),
	OfConnStateManager::Edge((int)OfConnStateManager::S_IDLE, (int)OfConnStateManager::S_DEADMEAT)
  };

/// From Boost Graph Library
const OfConnStateManager::StateGraph OfConnStateManager::mStateGraph(OfConnStateManager::state_edges,
		OfConnStateManager::state_edges+sizeof(OfConnStateManager::state_edges)/sizeof(OfConnStateManager::Edge),
		 OfConnStateManager::NO_OF_STATES);
///
/// @class OfConnStateManager
/// @param AbstractConnection object's reference
/// It inititalizes StateObjectsInterface array
///
OfConnStateManager::OfConnStateManager(AbstractConn& pVconn):
mVconn(pVconn)
{
    // inserts all new states between S_VOID and S_DEADMEAT if needed.
    mpArrayStateObjects[S_VOID] = std::unique_ptr<StateObjectInterface>(new iVconnStartState(*this));

    mpArrayStateObjects[S_CONNECTING] = std::unique_ptr<StateObjectInterface>(new iVconnConnectingState(*this));

    mpArrayStateObjects[S_CONNECTED] = std::unique_ptr<StateObjectInterface>(new iVconnConnectedState(*this));

    mpArrayStateObjects[S_ACTIVE] = std::unique_ptr<StateObjectInterface>(new iVconnActiveState(*this));

    mpArrayStateObjects[S_IDLE] = std::unique_ptr<StateObjectInterface>(new iVconnIdleState(*this));

    mpArrayStateObjects[S_DEADMEAT] = std::unique_ptr<StateObjectInterface>(new iVconnDeadState(*this));

    mpCurrentState = mpArrayStateObjects[S_VOID].get();
}

OfConnStateManager::~OfConnStateManager()
{
}

///
/// @brief if there is an edge to target state from currect state then move to
/// @returns true for successful transtition to the desired state
///
bool OfConnStateManager::TransitionTo(const uint32_t transition_id, const time_t now)
{
    if (!(static_cast<const State>(transition_id) <= S_DEADMEAT)) {
        std::cout << "Error: transition_id is not defined, should have asserted, ERR\n";
        return false;
    }

    StateObjectInterface *mpOldState = mpCurrentState;

	StateObjectInterface *nextIntendedState = nullptr;
	adjacency_iterator ai;
	adjacency_iterator ai_end;
	for(boost::tie(ai, ai_end) = adjacent_vertices(mpCurrentState->get_mTransitionId(), OfConnStateManager::mStateGraph);
		ai != ai_end; ++ai )
	{
		if(*ai == transition_id) {
			nextIntendedState = mpArrayStateObjects[transition_id].get();
			break;
		}
	} 

    bool retVal = (nextIntendedState == mpCurrentState); // check if already there

    if ( nextIntendedState && nextIntendedState != mpCurrentState ) {

        mpCurrentState = nextIntendedState;

        if ( mpCurrentState->Commit(now) ) {

            // It can chain to another state transition
            mStateEnteredTime = now;
            retVal = true;
            std::string outbuf(GetStateName(transition_id));
            outbuf += "\n";
            DBG( DBG_DETAIL|DBG_PARSER_STATE, ( "%s", outbuf.c_str() ) );

        } else {
            mpCurrentState = mpOldState;
        }
    }

    return retVal;
}

bool OfConnStateManager::Run(const time_t now)
{
    return mpCurrentState? mpCurrentState->Run(now):false;
}

bool OfConnStateManager::Timeout(const time_t now) const
{
    return mpCurrentState? mpCurrentState->Timeout(now):false;
}

uint32_t OfConnStateManager::CurrentId() const
{
    return mpCurrentState? mpCurrentState->get_mTransitionId():0;
}

const StateObjectInterface* OfConnStateManager::GetState(const uint32_t state_id) const
{
    return(state_id <= S_DEADMEAT) ?
    mpArrayStateObjects[state_id].get() : nullptr;
}

StateObjectInterface* OfConnStateManager::GetState(const uint32_t state_id)
{
    return(state_id <= S_DEADMEAT) ?
    mpArrayStateObjects[state_id].get() : nullptr;
}

const std::string& OfConnStateManager::GetStateName(const uint32_t state_id) const
{
    const static std::string invalid_name("INVALID");
    return(state_id <= S_DEADMEAT) ?
    mpArrayStateObjects[state_id]->get_mStrName() : invalid_name;
}
