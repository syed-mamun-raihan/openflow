#ifndef _STATECONTEXTINTERFACE_H
#define _STATECONTEXTINTERFACE_H

/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_iterator.hpp>
#include <memory>

class AbstractConn;
class StateObjectInterface;
///
///Implements State Pattern.
///Each State can Transit to only one of its Available State Objects based on Target State Id.
///See StateObjectInterface derived classes.
///When a valid state is found it calls commit - that handles level changes
///In each state may have additional functions like Run, Timeout as appropriate
///State machines are internal to client and do not exchanges this info with controller
///
class OfConnStateManager {
public:
    enum State {
        S_VOID = 0,
        S_CONNECTING = 1,
        S_CONNECTED = 2,
        S_ACTIVE = 3,
        S_IDLE = 4,
        S_DEADMEAT = 5,
        NO_OF_STATES
    };
    typedef boost::adjacency_list< boost::vecS, boost::vecS > StateGraph;
    typedef boost::graph_traits<StateGraph>::vertex_descriptor Vertex;
    typedef typename boost::graph_traits<StateGraph>::adjacency_iterator adjacency_iterator;
    typedef std::pair<int, int> Edge;
    ///
    /// StateManager for AbstractConnection
    ///
    explicit OfConnStateManager(AbstractConn& pVconn);

    ~OfConnStateManager();
    
    ///
    /// Uses state graph and simple StateObjectInterface to commit to the
    /// desired state which must be connected to current state
    /// Commit() function may attempt one or more transitions immediately
    /// So this one has to be re-entrant
    ///
    bool TransitionTo(const uint32_t transition_id, const time_t now);

    time_t get_mStateEnteredTime() const { return mStateEnteredTime;}

    ///
    /// All states should have a Run function that would say what it would
    /// do while system is in that state
    ///
    bool Run(const time_t now);

    bool Timeout(const time_t now) const; /// Is there any timeout in current state

    uint32_t CurrentId() const; /// returns current state id

    const StateObjectInterface* GetState(const uint32_t state_id) const;

    StateObjectInterface* GetState(const uint32_t state_id);

    const std::string& GetStateName(const uint32_t state_id) const;
    
    ///
    /// This class is a slave to iVconn - so store ref. only.
    /// Save the class that is being managed by this state machine manager
    ///
    AbstractConn& mVconn;

protected:
    StateObjectInterface *mpCurrentState = nullptr; /// current state pointer
    std::unique_ptr<StateObjectInterface> mpArrayStateObjects[NO_OF_STATES]; /// All the state objects
    time_t mStateEnteredTime = 0; /// State enetered time 
    const static Edge state_edges[]; /// define edges in the directed graph
    const static StateGraph mStateGraph; /// directed graph for the state machine
private:
    // shallow/deep copy not allowed
    OfConnStateManager(const OfConnStateManager& );
    OfConnStateManager& operator = (const OfConnStateManager& );
    // Equality tests are also not allowed
    bool operator == (const OfConnStateManager& );
    bool operator != (const OfConnStateManager& );
};

#endif
