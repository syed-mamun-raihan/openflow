/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <boost/range/adaptor/map.hpp>
#include "ofDebug.h"
#include "OfpHeader.h"
#include "AbstractConn.hpp"
#include "OfConnManager.hpp"
#include "OfpHeader.h"
#include "OfConnStateManager.hpp"
#include "OfConnStateObjects.hpp"
#include "OFV4Parser.hpp"
#include "OfDpDefinePD.h"
#include "OfDpLogicalSw.h"
#include "SslClient.hpp"
#include "GenericConn.hpp"

OfConnManager::OfConnManager(OfDpLogicalSw& msngr):mLogicalSw(msngr)
{
}

OfConnManager::~OfConnManager()
{
}

/// Run all attach connections state machine
void OfConnManager::operator ()(void)
{
    const timespec sleep_wait = {0,1000000000};
    while ( !mLogicalSw.mbIsDeleted ) {
        if(mLogicalSw.GetConfigObject().adminState) {
            const time_t now = time(0);
            std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
            for ( auto& i :  GetAttachments() | boost::adaptors::map_values ) {
                GetVconn(*i).Run(now);
            }
        }
        nanosleep(&sleep_wait,0);
    }
    mbThreadDied = true;
}

uint32_t OfConnManager::PacketIn(const OfDpPacketInT & info)
{
    unsigned cConnSend = 0;
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for ( auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).IsConnected() &&
             GetVconn(*i).mpParser->SendPacketIn(info) ) ++cConnSend;
    }
    return cConnSend;
}

uint32_t OfConnManager::PortStatus(const OfDpPortStatusT & info)
{
    unsigned cConnSend = 0;
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for ( auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).IsConnected() &&
             GetVconn(*i).mpParser->SendPortStatus(info) ) ++cConnSend;
    }
    return cConnSend;
}

uint32_t OfConnManager::FlowRemoved(const OfDpFlowRemovedT & info)
{
    unsigned cConnSend = 0;
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for ( auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).IsConnected() &&
             GetVconn(*i).mpParser->SendFlowRemoved(info) ) ++cConnSend;
    }
    return cConnSend;
}

bool OfConnManager::SendBarrierReply( const int fd, const uint32_t xid )
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for ( auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).IsConnected() &&
             (GetVconn(*i).GetSocketFd() == fd) &&
             GetVconn(*i).mpParser->SendBarrierReply(xid) ) return true;
    }
    return(false);
}

/// Get packets sent and recieved
bool OfConnManager::GetPacketStat(const std::string& host, const std::string& port, uint32_t& rcvdPkts, uint32_t& sendPkts) const
{
    std::lock_guard<std::recursive_mutex> lock(const_cast<OfConnManager*>(this)->mpVconn.mutex());
    for (auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if (GetVconn(*i).Match(0,host.data(), port.data())) {
            rcvdPkts =  GetVconn(*i).GetPacketsReceived();
            sendPkts =  GetVconn(*i).GetPacketsSent();
            return true;
        }
    }
    return false;
}


/// Get OfDpSwitchFeaturesT
bool OfConnManager::GetFeatureReply(const OfDpSwitchFeaturesT& info, const OfDpResponseT& response)
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for (auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).IsConnected() &&
             (GetVconn(*i).GetSocketFd() == response.socket_fd) &&
             GetVconn(*i).mpParser->GetFeatureReply(info, response) ) return true;
    }
    return(false);
}

/// Get OfDpSwitchConfigT reply
bool OfConnManager::GetConfigReply(const OfDpSwitchConfigT& info, const OfDpResponseT& response)
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for (auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).IsConnected() &&
             (GetVconn(*i).GetSocketFd() == response.socket_fd) &&
             GetVconn(*i).mpParser->GetConfigReply(info, response) ) return true;
    }
    return(false);
}

/// Set OfDpSwitchConfigT reply
bool OfConnManager::SetConfigReply(const OfDpResponseT& response)
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for (auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).IsConnected() &&
             (GetVconn(*i).GetSocketFd() == response.socket_fd) &&
             GetVconn(*i).mpParser->SetConfigReply(response) ) return true;
    }
    return(false);
}

/// PacketOut reply
bool OfConnManager::PacketOutReply(const OfDpResponseT& response)
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for (auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).IsConnected() &&
             (GetVconn(*i).GetSocketFd() == response.socket_fd) &&
             GetVconn(*i).mpParser->PacketOutReply(response) ) return true;
    }
    return(false);
}

/// Reply OfDpFlowT
bool OfConnManager::ResourceModReply(const OfDpFlowT & info, const OfDpResponseT& response)
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for (auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).IsConnected() &&
             (GetVconn(*i).GetSocketFd() == response.socket_fd) &&
             GetVconn(*i).mpParser->ResourceModReply(info, response) ) return true;
    }
    return(false);
}

/// Reply OfDpGroupT
bool OfConnManager::ResourceModReply(const OfDpGroupT & info, const OfDpResponseT& response)
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for (auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).IsConnected() &&
             (GetVconn(*i).GetSocketFd() == response.socket_fd) &&
             GetVconn(*i).mpParser->ResourceModReply(info, response) ) return true;
    }
    return(false);
}

/// Reply OfDpPortInfoT
bool OfConnManager::ResourceModReply(const OfDpPortInfoT & info, const OfDpResponseT& response)
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for (auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).IsConnected() &&
             (GetVconn(*i).GetSocketFd() == response.socket_fd) &&
             GetVconn(*i).mpParser->ResourceModReply(info, response) ) return true;
    }
    return(false);
}

/// Get description stats
bool OfConnManager::GetStatReply( const OfDpDescStatsT &stats, const OfDpResponseT& response)
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for (auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).IsConnected() &&
             (GetVconn(*i).GetSocketFd() == response.socket_fd) &&
             GetVconn(*i).mpParser->GetStatReply(stats, response) ) return true;
    }
    return(false);
}

/// Get flow stats
bool OfConnManager::GetStatReply( const OfDpFlowStatsT &stats, const OfDpResponseT& response)
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for (auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).IsConnected() &&
             (GetVconn(*i).GetSocketFd() == response.socket_fd) &&
             GetVconn(*i).mpParser->GetStatReply(stats, response) ) return true;
    }
    return(false);
}

/// Get aggregate flow stats
bool OfConnManager::GetStatReply( const OfDpAggregateStatsT &stats, const OfDpResponseT& response)
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for (auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).IsConnected() &&
             (GetVconn(*i).GetSocketFd() == response.socket_fd) &&
             GetVconn(*i).mpParser->GetStatReply(stats, response) ) return true;
    }
    return(false);
}

/// Get table stats
bool OfConnManager::GetStatReply( const OfDpTableStatsT &stats, const OfDpResponseT& response)
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for (auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).IsConnected() &&
             (GetVconn(*i).GetSocketFd() == response.socket_fd) &&
             GetVconn(*i).mpParser->GetStatReply(stats, response) ) return true;
    }
    return(false);
}

/// Get port stats
bool OfConnManager::GetStatReply( const OfDpPortStatsT &stats, const OfDpResponseT& response)
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for (auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).IsConnected() &&
             (GetVconn(*i).GetSocketFd() == response.socket_fd) &&
             GetVconn(*i).mpParser->GetStatReply(stats, response) ) return true;
    }
    return(false);
}

/// Get queue stats
bool OfConnManager::GetStatReply(const OfDpQueueStatsT &stats, const OfDpResponseT& response)
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for (auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).IsConnected() &&
             (GetVconn(*i).GetSocketFd() == response.socket_fd) &&
             GetVconn(*i).mpParser->GetStatReply(stats, response) ) return true;
    }
    return(false);
}

/// Queue config
bool OfConnManager::GetConfigReply( const OfDpQueueConfigT& info, const OfDpResponseT& response)
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for (auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).IsConnected() &&
             (GetVconn(*i).GetSocketFd() == response.socket_fd) &&
             GetVconn(*i).mpParser->GetConfigReply(info, response) ) return true;
    }
    return(false);
}

/// Get stats
bool OfConnManager::GetStatReply(const OfDpGroupDescStatsT &stats, const OfDpResponseT& response)
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for (auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).IsConnected() &&
             (GetVconn(*i).GetSocketFd() == response.socket_fd) &&
             GetVconn(*i).mpParser->GetStatReply(stats, response) )return true;
    }
    return(false);
}

/// Get stats
bool OfConnManager::GetStatReply(const OfDpGroupFeatureStatsT &stats, const OfDpResponseT& response)
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for (auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).IsConnected() &&
             (GetVconn(*i).GetSocketFd() == response.socket_fd) &&
             GetVconn(*i).mpParser->GetStatReply(stats, response) ) return true;
    }
    return(false);
}

/// Get stats
bool OfConnManager::GetStatReply(const OfDpGroupStatsT &stats,  const OfDpResponseT& response)
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for (auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).IsConnected() &&
             (GetVconn(*i).GetSocketFd() == response.socket_fd) &&
             GetVconn(*i).mpParser->GetStatReply(stats, response) ) return true;
    }
    return(false);
}

/// Get port stats 1.3+
bool OfConnManager::GetStatReply( const OfDpPortListT &stats, const OfDpResponseT& response)
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for (auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).IsConnected() &&
             (GetVconn(*i).GetSocketFd() == response.socket_fd) &&
             GetVconn(*i).mpParser->GetStatReply(stats, response) ) return true;
    }
    return(false);
}

/// Get table feature stats 1.3+
bool OfConnManager::GetStatReply( const OfDpTableFeatureStatsT &stats, const OfDpResponseT& response)
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for (auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).IsConnected() &&
             (GetVconn(*i).GetSocketFd() == response.socket_fd) &&
             GetVconn(*i).mpParser->GetStatReply(stats, response) ) return true;
    }
    return(false);
}

/// Add a SSL Attachments
bool OfConnManager::AddSSLController(const OfDpControllerAttachmentT& attachment,
                                     const OfDpControllerT& curController ,
                                     const time_t now)
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for (const auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).Match(0,curController.ipAddress, curController.port)) {
        	return false; // duplicate
        }
    }
    GenericConn<SslClient <TcpClient> >* new_ssl_conn = new GenericConn<SslClient <TcpClient> >(*this,  attachment, now);
    if (new_ssl_conn) {
		new_ssl_conn->GetHost() = curController.ipAddress;
		new_ssl_conn->GetPort() = curController.port;
		new_ssl_conn->mConnObj.msClientCert = curController.certfile;
		new_ssl_conn->mConnObj.msClientKey = curController.privfile;
		new_ssl_conn->mConnObj.msCaCert = curController.cafile;
		if ( mpVconn.Attach(new_ssl_conn)) {
			new_ssl_conn->Run(now);
			std::cout << " SSL Connection State Machine Started\n";
			return true;
		}
		delete new_ssl_conn;
    }
    return false;
}

/// Add TCP attachments
bool OfConnManager::AddController(const OfDpControllerAttachmentT& attachment,
                                  const OfDpControllerT& curController ,
                                  const time_t now)
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for (const auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).Match(0,curController.ipAddress, curController.port)) {
        	return false; // duplicate
        }
    }
    GenericConn<TcpClient>* new_tcp_conn = new GenericConn<TcpClient>(*this, attachment, now);
    if (new_tcp_conn) {
		new_tcp_conn->GetHost() = curController.ipAddress;
		new_tcp_conn->GetPort() = curController.port;
		if ( mpVconn.Attach(new_tcp_conn)) {
			new_tcp_conn->Run(now); // Run the state machine for the first time
			std::cout << " TCP Connection State Machine Started\n";
			return true;
		}
		delete new_tcp_conn;
    }
    return false;
}

bool OfConnManager::DelController(const std::string& ctlName)
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    return mpVconn.Detach(ctlName);
}

/**
 * Call this function when Role Request is made by one
 * controller // It does not validate any pre/post-requisites
 * TODO: AUX_MASTER and AUX_SLAVE need to be handled if we add
 * AUX channel supports
 *
 */
void OfConnManager::ResetMastersToSlave()
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for (auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( GetVconn(*i).GetCurrentRole() == OFDP_CHANNEL_ROLE_MASTER ) {
            GetVconn(*i).GetCurrentRole() = OFDP_CHANNEL_ROLE_SLAVE;
        }
    }
}

/**
 * Updates probeInterval, timeoutInterval
 *
 * @param config
 * @param fd
 *
 * @return bool
 */
bool OfConnManager::UpdateAttachment(OfDpControllerAttachmentT& config)
{
    std::lock_guard<std::recursive_mutex> lock(mpVconn.mutex());
    for (auto& i :  GetAttachments() | boost::adaptors::map_values) {
        if ( config.key() == i->key()) {
            config.isTlsSet = i->isTlsSet;
            mpVconn.Notify(config);
            return true;
        }
    }
    return false;
}

std::ostream& operator << (std::ostream& out, const OfConnManager& obj)
{
    size_t count = 0;
    std::lock_guard<std::recursive_mutex> lock(const_cast<OfConnManager&>(obj).mpVconn.mutex());
    for ( const auto& i : obj. GetAttachments() | boost::adaptors::map_values) {
        out << " Conn["<< count << "] := "
        << " host : " << obj.GetVconn(*i).GetHost()
        << " port : " << obj.GetVconn(*i).GetPort()
        << " Role: " << to_string(obj.GetVconn(*i).GetCurrentRole())
        << " (" << obj.GetVconn(*i).GetCurrentRole() << ")"
        << " State: " << obj.GetVconn(*i).GetCurrentStateName()
        << " Version: " << (unsigned)obj.GetVconn(*i).mpParser->get_wire_version()
        << " socket_fd : " << obj.GetVconn(*i).GetSocketFd()
        << " isTlsSet : " << i->isTlsSet
        << " probeInterval : " << obj.GetVconn(*i).GetProbeInterval()
        << " timeoutInterval : " << obj.GetVconn(*i).GetTimeoutInterval()
        << "\n";
        ++count;
    }
    return out;
}

/// It can return NULL pointer so use it with only valid iterator
AbstractConn& OfConnManager::GetVconn(OfDpControllerAttachmentT& attachment ){
    if (attachment.isTlsSet) {
        return static_cast<AbstractConn&>((GenericConn< SslClient< TcpClient > >&) (attachment));
    }
    return static_cast<AbstractConn&>((GenericConn< TcpClient >&) (attachment));
}

/// It can return NULL pointer so use it with only valid iterator
const AbstractConn& OfConnManager::GetVconn(const OfDpControllerAttachmentT& attachment ) const{
    if (attachment.isTlsSet) {
        return static_cast<const AbstractConn&>((const GenericConn< SslClient< TcpClient > >&) (attachment));
    }
    return static_cast<const AbstractConn&>((const GenericConn< TcpClient >&) (attachment));
}
