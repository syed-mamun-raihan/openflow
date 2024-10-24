/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include <cstdio>
#include <iomanip>
#include <sys/time.h>

#include "OfpHeader.h"
#include "AbstractConn.hpp"
#include "OfConnManager.hpp"
#include "OfConnStateManager.hpp"
#include "OfConnStateObjects.hpp"
#include "OFV4Parser.hpp"
#include "OfDpDefinePD.h"
#include "OfDpLogicalSw.h"


struct timeval perf_starttime;
struct DsDbRecordStatus flowstats;
int gDispMplsFlowCount = 10000;
int gDispVlanFlowCount = 5000;

// flag for performance debug
uint32_t gDsDbPerfDebugFlag = 0;
/**
 * CTOR
 * @see OfDpControllerAttachmentT
 * @author sraihan ()
 * 
 * @param switchP 
 * @param obj 
 * @param now 
 */
AbstractConn::AbstractConn(OfConnManager & switchP, const OfDpControllerAttachmentT& obj, const time_t now) :
GenericObservableConfig<OfDpControllerAttachmentT>(obj)
,mCreationTime(now)
,mLastBackOffTime(now)
,mLastQuestionedTime(now)
,mLastActivityTime(now)
,mConnMgr(switchP)
{
    if (GetProbeInterval() == 0) GetProbeInterval() = 15;
    if (GetTimeoutInterval() == 0) GetTimeoutInterval() = 30;
    mpStateMgr = new OfConnStateManager(*this);
    //OFDP_VERSION_V4 is higest and deafult
    mpParser = new OFV4Parser(*this);
}

/**
 * DTOR
 * 
 * @author sraihan ()
 */
AbstractConn::~AbstractConn()
{
    if ( mpParser )delete mpParser;
    if ( mpStateMgr )delete mpStateMgr;
}

/**
 *  checks current state and transitions to next
 *  state if necessary. Reads a packet. calls iOpenflow->process_of_msg()
 *  Used by state machine.
 * 
 * @author sraihan ()
 * 
 * @param now 
 * 
 * @return bool 
 */
bool AbstractConn::Run(const time_t now) {
    return mpStateMgr->Run(now);
}

/**
 * Used by State machine
 * 
 * @author sraihan ()
 * 
 * @param now 
 * 
 * @return bool 
 */
bool AbstractConn::Reconnect(const time_t now) {
    // Attempt to S_CONNECTING
    bool retVal = false;
    unsigned n_attempts = 0;
    const unsigned max_attempts = 3;
    do {
        retVal = Open();
    }while ( !retVal && ++n_attempts < max_attempts );

    if ( retVal ) ResetBackoff(now);

    return retVal;
}

/**
 *  Used by state machine.
 * 
 * @author sraihan ()
 * 
 * @param pPktBuffer 
 * @param length 
 * @param type_name 
 * @param out 
 */
void AbstractConn::DumpPacket(const uint8_t* pPktBuffer, const uint16_t length, const char* type_name, std::ostream& out) {
    const char oldfill = out.fill();
    const int oldwidth = out.width();
    out << "\n" << type_name <<": Packet Length: " << std::dec << length << std::endl;
    for ( uint32_t i = 1; i <= length; i++ ) {
        out << "0x"
        << std::setfill('0')
        << std::setw(2)
        << std::hex
        << (uint32_t)(*pPktBuffer++) << " ";
        if ( (i % 8) == 0 ) out << std::endl;
    }
    out << std::setfill(oldfill)
    << std::setw(oldwidth)
    << std::dec
    << std::endl;
}

/**
 *  Used by state machine. Questions connectivity.
 * 
 * @author sraihan ()
 * 
 * @param now 
 * 
 * @return bool 
 */
bool AbstractConn::BackOff(const time_t now) {
    const bool isOpened = IsOpen();
    if ( isOpened && (now - mLastBackOffTime > GetProbeInterval()) ) {
        mpStateMgr->TransitionTo(OfConnStateManager::S_IDLE, now);
    } else if ( !isOpened ) {
        mpStateMgr->TransitionTo(OfConnStateManager::S_VOID, now);
    }
    // See if BackOffTime needs update
    if ( now - mLastBackOffTime > GetProbeInterval() ) {
        DBG( DBG_DETAIL|DBG_PARSER_STATE, (" iVconn::BackOff: wait for %li sec before disconnect, state %s\n", GetProbeInterval(), GetCurrentStateName().c_str() ) );
        mLastBackOffTime = now ;
        if ( (mpStateMgr->CurrentId() == OfConnStateManager::S_IDLE ||
              mpStateMgr->CurrentId() == OfConnStateManager::S_CONNECTED) && !mbHelloRecieved ) {
            DBG( DBG_DETAIL|DBG_PARSER_STATE, (" iVconn::BackOff: sending hello as hello has not received \n") );
            this->send_hello(now);
        }

    }

    // Question Connectivity to reset connection
    if ( now - mLastQuestionedTime > GetTimeoutInterval() ) {
        // we try to reconnect every minute
        DBG( DBG_DETAIL|DBG_PARSER_STATE, (" iVconn::BackOff: Trying reconnect @ %li\n", now ) );
        mLastQuestionedTime = now;
        // State machine will close the socket if it is still valid
        mpStateMgr->TransitionTo(OfConnStateManager::S_VOID, now);
        // State machine will try to move to Connecting State
        mpStateMgr->Run(now);
    }

    return true;
}

/**
 *  Used by state machine.
 * 
 * @author sraihan ()
 * 
 * @param now 
 * 
 * @return bool 
 */
bool AbstractConn::TimedOut(const time_t now) const
{
    const time_t max_1 = std::max(mpStateMgr->get_mStateEnteredTime(),
                                  mLastActivityTime);
    const time_t max_2 = std::max(max_1, mLastBackOffTime);
    return(now >= ( max_2 + mpStateMgr->Timeout(now)));
}

/**
 * Utility function - return value based on state machine
 * 
 * @author sraihan ()
 * 
 * @return bool 
 */
bool AbstractConn::IsConnected() const
{
    const OfConnStateManager::State meState = static_cast<const OfConnStateManager::State>(mpStateMgr->CurrentId());
    return(meState == OfConnStateManager::S_ACTIVE || meState == OfConnStateManager::S_IDLE);
}

/**
 * For version negotiation
 * 
 * @author sraihan ()
 * 
 * @param msg 
 * @param out 
 * 
 * @return bool 
 */
bool AbstractConn::SelectNewParser(const ofp_message & msg, std::ostream & out)
{
    if ( !this->mbAutoNegotiation ) {
        return false; // we are not allowed to autonegotiate so we can not select new parser
    }
    if ( msg.version == mpParser->get_wire_version() ) return false;
    if ( msg.version == OFDP_VERSION_V4 ) {
        delete mpParser;
        mpParser = new OFV4Parser(*this);
        out << " changing to OF1.3 - ";
        return true;
    } 
    return false;
}

//\param    ofp_message
//\desc     msg.type == OpenflowV3::OFPT_HELLO
//\return   If a message is not admitted and no error is send then we return false
//          If a message is admitted then we may send a response or not it should return true
bool AbstractConn::process_of_msg(const ofp_message & msg, const time_t now, std::ostream& out) {
    if ( !mpParser || msg.get_length() > GetMaxReadBufferLen() ) {
        out << "Error: Parser is not defined or corrupt messages, ERR\n";
        return false;
    }
    // Dump the whole packet as C Array
    if ( msg.get_length() <= GetMaxReadBufferLen() && (gDbgMask == (DBG_DETAIL|DBG_PARSER_PARSE)) ) {
        const uint8_t* pPktBuffer = reinterpret_cast<const uint8_t*>(&msg);
        DumpPacket(pPktBuffer, msg.get_length(), "iVconn::receive_message", out);
    } else {
        out <<  "\niVconn::receive_message " << msg;
    }
    bool retVal = false;

    if ( mpParser->get_wire_version() == msg.version && // version match
         msg.type !=  mpParser->get_hello_type() &&   // not a hello
         msg.get_length() >= msg.get_minimum_length() && // enough bytes to check header
         msg.get_length() <= GetMaxReadBufferLen() &&   // so far good length encoding
         mbHelloSend &&
         mbHelloRecieved ) { // forward to this openflow handler
        retVal = mpParser->process_of_msg(msg,now,out); // a false return indicates serios error and socket would be closed
    } else if ( mpParser->get_hello_type()  == msg.type &&   // got hello
                mpParser->get_wire_version() == msg.version &&
                !mbHelloRecieved ) {// with correct version match and no hello yet
        mbHelloRecieved = true;
        retVal = send_hello(msg.get_xid());
        if ( retVal ) // success in sending hello - move to active
            mpStateMgr->TransitionTo(OfConnStateManager::S_ACTIVE, now); // we should not close on state transitions
    } else if ( mpParser->get_hello_type() == msg.type &&
                mpParser->get_wire_version() != msg.version ) {// See if we have a parser for a requested hello
        retVal = SelectNewParser(msg, out); // autonegotiate new version
        if ( retVal ) { // and send hello
            out << " selected a new parser - sending a hello now ";
            this->mConnMgr.mLogicalSw.VersionNegotiated(msg.version);
            retVal = send_hello(msg.get_xid());
            if ( retVal ) // success in sending hello - move to active
                mpStateMgr->TransitionTo(OfConnStateManager::S_ACTIVE, now); // we should not close on state transitions
        } else { // report failure
            out << " can not switch version and closing socket as expecting: " << (unsigned )(mpParser->get_wire_version());
            mpParser->report_bad_message(msg, mpParser->get_hello_failed_type(), mpParser->get_hello_failed_code(), out);
        }
    } else { // closes socket
        if ( msg.version != mpParser->get_wire_version() ) {
            out << " unknown version - sending a error ";
            mpParser->report_bad_message(msg, 1, 0, out); // bad version
        } else {
            out << " unknown packet - sending a error ";
            mpParser->report_bad_message(msg, 1, 1, out); // bad type
        }
    }

    return retVal;// if false closes socket
}

/**
 * msg.type == OpenflowV3::OFPT_HELLO
 * 
 * @author sraihan ()
 * 
 * @param xid 
 * 
 * @return bool 
 */
bool AbstractConn::send_hello(uint32_t xid) {
    if ( !mbHelloSend ) {
        ofp_message hello_pkt;
        hello_pkt.set_length(sizeof(hello_pkt));
        hello_pkt.type = mpParser->get_hello_type();
        hello_pkt.version = mpParser->get_wire_version();
        hello_pkt.set_xid(xid);
        mbHelloSend = Send(hello_pkt);
        mpParser->ResetSendEchoFlag();
    }
    return mbHelloSend;
}

/**
 * Utility function
 * 
 * @author sraihan ()
 * 
 * @return const std::string& 
 */
const std::string& AbstractConn::GetCurrentStateName() const
{
    return mpStateMgr->GetStateName(mpStateMgr->CurrentId());
}

/**
 * Logical Switch version - not the negotiated version
 * 
 * @author sraihan ()
 * 
 * @return int 
 */
int AbstractConn::GetVersion() const { return mConnMgr.mLogicalSw.getVersion();}
