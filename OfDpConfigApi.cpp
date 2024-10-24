/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include <cstdio>
#include <thread>
#include <mutex>
#include <atomic>
#include <sstream>
#include <boost/range/adaptor/map.hpp>

#include "OfDpLogicalSw.h"
#include "OfDpController.h"
#include "OfConnManager.hpp"
#include "OfDpConfigApi.h"
#include "OfDataPlaneApi.h"
//#include "OfDpObjPD.h"

// Singleton instance as static class member
OfDpConfigApi OfDpConfigApi::_instance;
volatile bool OfDpConfigApi::mbInstanceDeleted(false); /// once in a life time set

/**
 * Singleton accessor
 * 
 * @author sraihan (7/25/2014)
 * 
 * @return OfDpConfigApi* 
 */
OfDpConfigApi* OfDpConfigApi::getInstance() {
    if (mbInstanceDeleted) {
        return nullptr;
    }
    return (&_instance);
}

/**
 * DTOR
 * 
 * @author sraihan (7/25/2014)
 */
OfDpConfigApi::OfDpConfigApi() {

}

/**
 * Set a dataplane Api
 * 
 * @author sraihan (7/25/2014)
 * 
 * @param dataPlanePtr 
 */
void OfDpConfigApi::setDataPlaneApiPtr(OfDataPlaneApi *dataPlanePtr) {
    mpDataPlane = dataPlanePtr;
}

/**
 * Get number of flows for a LS
 * 
 * @author sraihan (7/25/2014)
 * 
 * @param name 
 * 
 * @return uint32_t 
 */
uint32_t OfDpConfigApi::GetFlowCount(const std::string &name) const {
    return static_cast<const OfDpLogicalSw *>(mLogicalSwitches[name])->getTable().getFlowCount();
}

/**
 *  Update a logical switch / data path instance
 * 
 * @author sraihan (7/25/2014)
 * 
 * @param ls 
 * 
 * @return OfDpReturn 
 */
OfDpReturn OfDpConfigApi::Update(const OfDpLogicalSwT &ls) {
    DBG_TRACE_ENTER;
    OfDpReturn ret = OFDP_OK;
    std::lock_guard<std::recursive_mutex> xLock(mLogicalSwitches.mutex());
    const OfDpLogicalSw *logicalSwEntry = static_cast<const OfDpLogicalSw *>(GetLogicalSw(ls.name));

    switch (ls.command) {
    case OFDP_CONFIG_CREATE:
        {
            if (logicalSwEntry) {
                // we can not have same name
                ret = OFDP_ENTRY_EXISTS;
            } else {
                for(const auto& pSwitch : mLogicalSwitches | boost::adaptors::map_values ){
                    if (ls.dpId == pSwitch->dpId) {
                        // we can not have same dpId
                        ret = OFDP_ENTRY_EXISTS;
                        break;
                    }
                }
            }
            if (!logicalSwEntry && ret == OFDP_OK) {
                // Create new instance of logical switch, store in table
                OfDpLogicalSw *lswitch = new OfDpLogicalSw(mpDataPlane, ls);
                if (lswitch && !mLogicalSwitches.Attach(lswitch)) {
                    delete lswitch;
                    ret = OFDP_ENTRY_EXISTS;
                }
            }
            break;
        }
    case OFDP_CONFIG_REMOVE:
        {
            // Remove logical switch...
            if (logicalSwEntry &&
                mLogicalSwitches.Detach(ls.name)){
                // Remove port-to-ls map entries
                for(const auto& i : _portToLogicalSw){
                    if (static_cast<OfDpLogicalSw*>(i.second)== logicalSwEntry){
                        _portToLogicalSw.erase(i.first);
                    }
                }
            } else {
                ret = OFDP_LS_NOT_EXIST;
            }
            break;
        }
    case OFDP_CONFIG_MODIFY:
    case OFDP_CONFIG_UPDATE_STATE:
        {
            // We only support UpdateLogicalSwitch that does not need
            // attached connection update -
            // Use update attachment for that
            mLogicalSwitches.Notify(ls);
            break;
        }
    default:
        ret = OFDP_ERR_UNKNOWN_COMMAND;
        break;
    }

    DBG_TRACE_RET(ret);
}

/**
 * Update a Flow - from dataplane adaptation to cofa
 * 
 * @author sraihan (7/25/2014)
 * 
 * @param ls 
 * @param flow 
 * 
 * @return OfDpReturn 
 */
OfDpReturn OfDpConfigApi::UpdateFlow(const OfDpLogicalSwT *ls, const OfDpFlowT &flow) {
    std::lock_guard<std::recursive_mutex> xLock(mControllers.mutex());

    OfDpReturn ret = OFDP_OK;
    OfDpLogicalSw *logicalSwEntry;

    // this will possibly need to be populated with data
    OfDpResponseT response = OfDpResponseT();

    auto iter = mLogicalSwitches.find(ls->name);
    if (iter != mLogicalSwitches.end()) {
        logicalSwEntry = static_cast<OfDpLogicalSw *>(iter->second.get());
        // pass the flow and response to the update flow function for the
        // logical switch.
        ret = logicalSwEntry->updateFlow(flow, response);
    }

    return ret;
}

/**
 * Update a controller
 * 
 * @author sraihan (7/25/2014)
 * 
 * @param controller 
 * 
 * @return OfDpReturn 
 */
OfDpReturn OfDpConfigApi::Update(const OfDpControllerT &controller) {
    DBG_TRACE_ENTER;
    std::lock_guard<std::recursive_mutex> xLock(mControllers.mutex());
    OfDpReturn ret = OFDP_OK;

    switch (controller.command) {
    case OFDP_CONFIG_CREATE:
        {
            if (GetController(controller.name)) {
                // we can not have same name
                ret = OFDP_ENTRY_EXISTS;
            } else {
                for(const auto& pCtl : mControllers  | boost::adaptors::map_values ){
                    if (controller.ipAddress == pCtl->ipAddress &&
                        controller.port == pCtl->port) {
                        // we can not have same dpId
                        ret = OFDP_ENTRY_EXISTS;
                        break;
                    }
                }
            }
            if (ret == OFDP_OK) {
                // Create new instance of Controller
                OfDpController *pController = new OfDpController(controller);
                // Check if we can Attach
                if (pController && !mControllers.Attach(pController)) {
                    ret = OFDP_ENTRY_EXISTS;
                    delete pController;
                }
            }
            break;
        }
    case OFDP_CONFIG_REMOVE:
        // Remove logical switch...
        if (mControllers.Detach(controller.name)) {
            for(auto& pSwitch : mLogicalSwitches | boost::adaptors::map_values ){
                static_cast<OfDpLogicalSw&>(*pSwitch).DeleteController(controller.name);
            }
        } else {
            ret = OFDP_CONTROLLER_NOT_EXIST;
        }
        break;
    case OFDP_CONFIG_MODIFY:
    case OFDP_CONFIG_UPDATE_STATE:
        {
            mControllers.Notify(controller);
            for(auto& pSwitch : mLogicalSwitches | boost::adaptors::map_values){
                static_cast<OfDpLogicalSw&>(*pSwitch).UpdateController(controller);
            }
            break;
        }
    default:
        ret = OFDP_ERR_UNKNOWN_COMMAND;
        break;
    }

    DBG_TRACE_RET(ret);

}

/**
 * Update controller attachments
 * 
 * @author sraihan (7/25/2014)
 * 
 * @param attachment 
 * 
 * @return OfDpReturn 
 */
OfDpReturn OfDpConfigApi::Update(const OfDpControllerAttachmentT &attachment) {
    DBG_TRACE_ENTER;
    std::lock_guard<std::recursive_mutex> xLock(mLogicalSwitches.mutex());
    OfDpReturn ret = OFDP_OK;
    const OfDpLogicalSw *ls = static_cast<const OfDpLogicalSw *>(GetLogicalSw(attachment.lsName));

    switch (attachment.command) {
    case OFDP_CONFIG_CREATE:
        {
            if (ls) {
                const OfDpControllerT *pController = GetController(attachment.name);

                /// Find controller entry
                if (pController) {
                    ret = const_cast<OfDpLogicalSw&>(*ls).AddController(*pController,
                                                                        const_cast<OfDpControllerAttachmentT&>(attachment));
                } else {
                    ret = OFDP_ENTRY_NOT_EXIST;
                }
            } else {
                ret = OFDP_ENTRY_NOT_EXIST;
            }
            break;
        }
    case OFDP_CONFIG_REMOVE:
        {
            if (!ls) {
                ret = OFDP_ENTRY_NOT_EXIST;
            } else {
                ret = const_cast<OfDpLogicalSw&>(*ls).DeleteController(attachment.name);
            }
            break;
        }
    case OFDP_CONFIG_MODIFY:
    case OFDP_CONFIG_UPDATE_STATE:
        {
            for(auto& pSwitch : mLogicalSwitches | boost::adaptors::map_values ){
                static_cast<OfDpLogicalSw&>(*pSwitch).UpdateAttachment(const_cast<OfDpControllerAttachmentT&>(attachment));
            }
            break;
        }
    default:
        ret = OFDP_ERR_UNKNOWN_COMMAND;
        break;
    }

    DBG_TRACE_RET(ret);
}

/**
 * Get controller attachments
 * 
 * @author sraihan (7/25/2014)
 * 
 * @param lsName 
 * @param controllerName 
 * 
 * @return const OfDpControllerAttachmentT* 
 */
const OfDpControllerAttachmentT* OfDpConfigApi:: GetAttachmentInfo(const std::string &lsName, const std::string &controllerName) const {
    DBG_TRACE_ENTER;
    const OfDpLogicalSw *ls = static_cast<const OfDpLogicalSw *>(GetLogicalSw(lsName));
    const OfDpControllerAttachmentT *retVal = ls->GetAttachmentInfo(controllerName);
    DBG_TRACE_EXIT;
    return (retVal);
}

/**
 * Pending work
 * 
 * @author sraihan (7/25/2014)
 * 
 * @return OfDpReturn 
 */
OfDpReturn OfDpConfigApi::CheckFlowTimeout() {
    DBG_TRACE_ENTER;
#if 0
    OfDpReturn ret = OFDP_OK;
    std::map< uint8_t, OfDpLogicalSw * >::iterator i;

    i = _logicalSws.begin();

    while (i != _logicalSws.end()){
        if ((ret = i->second->checkFlowTimeout())!= OFDP_OK){}

        i++;
    }
#endif
    // Always return success.
    DBG_TRACE_RET(OFDP_OK);
}

/**
 * Update ports
 * 
 * @author sraihan (7/25/2014)
 * 
 * @param port 
 * 
 * @return OfDpReturn 
 */
OfDpReturn OfDpConfigApi::Update(const  OfDpPortInfoT &port) {
    DBG_TRACE_ENTER;
    std::lock_guard<std::recursive_mutex> xLock(GetPorts().mutex());
    OfDpReturn ret = OFDP_OK;

    switch (port.command) {
    case OFDP_CONFIG_CREATE:
        {
            if (GetPorts()[port.key()]) {
                DBG_TRACE_RET(OFDP_ENTRY_EXISTS);
            }
            auto* pPort = new GenericObservableConfig<OfDpPortInfoT>(port);
            if (pPort && !GetPorts().Attach(pPort)) {
                ret = OFDP_ERR_INPUT;
                delete pPort;
            }
            break;
        }
    case OFDP_CONFIG_REMOVE:
        {
            /// name must match
            const OfDpPortInfoT* info = GetPorts()[port.key()];
            /// port id must match and Notify observer must work
            if(!(info &&
                 info->port_no == port.port_no &&
                 GetPorts().Detach(info->port_no))){
                ret = OFDP_ERR_INPUT;
            }
            break;
        }
    case OFDP_CONFIG_MODIFY:
    case OFDP_CONFIG_UPDATE_STATE:
        {
            /// name must match
            const OfDpPortInfoT* info = GetPorts()[port.key()];
            /// port id must match and Notify observer must work
            if(!(info &&
                 info->port_no == port.port_no &&
                 GetPorts().Notify(port))){
                ret = OFDP_ERR_INPUT;
            }

            // Send port status update to controller
            {
                OfDpLogicalSwT *ls = _portToLogicalSw[port.port_no];
                if (ls){
                    OfDpPortStatusT status;
                    status.reason = OFDP_OFPPR_MODIFY;
                    SendPortStatus(port, *ls, status);
                }
            }
            break;
        }
    default:
        ret = OFDP_ERR_UNKNOWN_COMMAND;
        break;
    }
    DBG_TRACE_RET(ret);
}

/**
 * Update port attachments
 * 
 * @author sraihan (7/25/2014)
 * 
 * @param port_attachment 
 * 
 * @return OfDpReturn 
 */
OfDpReturn OfDpConfigApi::Update(const  OfDpPortAttachmentT &port_attachment) {
    DBG_TRACE_ENTER;
    std::lock_guard<std::recursive_mutex> xLock(mLogicalSwitches.mutex());
    const OfDpLogicalSwT *lsBase = GetLogicalSw(port_attachment.lsName);
    const OfDpLogicalSw *ls = static_cast<const OfDpLogicalSw *>(lsBase);
    if (!ls) {
        std::cout << " To attach port, ls not found := " << port_attachment.lsName << std::endl;
        DBG_TRACE_RET(OFDP_ENTRY_NOT_EXIST);
    }
    OfDpReturn ret = OFDP_OK;
    OfDpPortStatusT info;
    const OfDpPortInfoT *port = GetPorts()[port_attachment.key()];
    switch (port_attachment.command) {
    case OFDP_CONFIG_CREATE:
        {
            info.reason = OFDP_OFPPR_ADD;
            if (!ls->GetPorts()[port_attachment.key()] && port) {
                auto* pPort = new GenericObservableConfig<OfDpPortAttachmentT>(port_attachment);
                if (pPort && !const_cast<OfDpLogicalSw *>(ls)->GetPorts().Attach(pPort)) {
                    delete pPort;
                    ret = OFDP_ERR_INPUT;
                } else if (pPort) {
                    _portToLogicalSw[port_attachment.port_no] = const_cast<OfDpLogicalSwT *>(lsBase);
                    SendPortStatus(*port, *const_cast<OfDpLogicalSwT *>(lsBase), info);
                }
            } else {
                ret = OFDP_BAD_PORT_ID;
            }
            break;
        }
    case OFDP_CONFIG_REMOVE:
        {
            info.reason = OFDP_OFPPR_DELETE;
            /// name must match
            if(const_cast<OfDpLogicalSw*>(ls)->GetPorts().Detach(port_attachment.key())){
                _portToLogicalSw.erase(port_attachment.port_no);
                SendPortStatus(*port, *const_cast<OfDpLogicalSw*>(ls), info);
            } else {
                ret = OFDP_BAD_PORT_ID;
            }
            break;
        }
    case OFDP_CONFIG_MODIFY:
    case OFDP_CONFIG_UPDATE_STATE:
        {
            info.reason = OFDP_OFPPR_MODIFY;
            /// port id must match and Notify observer must work
            if(!(port &&
                 port->port_no == port_attachment.port_no &&
                 const_cast<OfDpLogicalSw*>(ls)->GetPorts().Notify(port_attachment))){
                ret = OFDP_ERR_INPUT;
            } else {
                SendPortStatus(*port, *const_cast<OfDpLogicalSw*>(ls), info);
            }
            break;
        }
    default:
        ret = OFDP_ERR_UNKNOWN_COMMAND;
        break;
    }
    DBG_TRACE_RET(ret);
}

/**
 * Notify all controller for Port Status change
 * 
 * @author sraihan (7/25/2014)
 * 
 * @param port 
 * @param ls 
 * @param info 
 * 
 * @return uint32_t 
 */
uint32_t OfDpConfigApi::SendPortStatus(const OfDpPortInfoT &port, OfDpLogicalSwT &ls, OfDpPortStatusT &info) {
    /// TODO: Translate here but define internal representation first
    info.desc.advertised = port.advertised;
    info.desc.config = port.config;
    info.desc.port_no = port.port_no;
    info.desc.curr = port.curr;
    info.desc.curr_speed = port.curr_speed;
    memcpy(info.desc.hw_addr, port.hw_addr, sizeof(info.desc.hw_addr));
    info.desc.mask = port.mask;
    info.desc.max_speed = port.max_speed;
    info.desc.name = port.name;
    info.desc.peer = port.peer;
    info.desc.state = port.state;
    info.desc.supported = port.supported;
    return static_cast<OfDpLogicalSw&>(ls).GetOfConnManager().PortStatus(info);
}

/**
 * Get all controllers attached to a LS
 * 
 * @author sraihan (7/25/2014)
 * 
 * @param lsName 
 * @param attachments 
 * 
 * @return OfDpReturn 
 */
OfDpReturn OfDpConfigApi::Get(const std::string &lsName,
                              std::list<OfDpControllerAttachmentT> &attachments) const {
    DBG_TRACE_ENTER;
    const OfDpLogicalSw *ls = static_cast<const OfDpLogicalSw *>(GetLogicalSw(lsName));

    if (ls) {
        for(const auto& pCtl : ls->GetAttachments()| boost::adaptors::map_values  ){
            attachments.emplace_back(*pCtl);
        }
        DBG_TRACE_RET(OFDP_OK);
    }
    DBG_TRACE_RET(OFDP_ENTRY_NOT_EXIST);
}

/**
 * Forward packets to a controller
 * 
 * @author sraihan (7/25/2014)
 * 
 * @param portIdx 
 * @param phyPortIdx 
 * @param pkt 
 * @param pktLen 
 * 
 * @return OfDpReturn 
 */
OfDpReturn OfDpConfigApi::ReceivePkt(uint32_t portIdx,
                                     uint32_t phyPortIdx,
                                     uint8_t  *pkt,
                                     uint32_t pktLen) {
    DBG_TRACE_ENTER;
    OfDpReturn ret = OFDP_OK;

    auto&& portToLogicalSwIt = _portToLogicalSw.find( portIdx );

    if (portToLogicalSwIt != _portToLogicalSw.end()) {
        ret = static_cast<OfDpLogicalSw *>(portToLogicalSwIt->second)->receivePkt(portIdx, phyPortIdx, pkt, pktLen);
    } else {
        ret = OFDP_ENTRY_NOT_EXIST;
    }

    DBG_TRACE_RET(ret);
}

/**
 * Get All attached ports for a LS
 * 
 * @author sraihan (7/25/2014)
 * 
 * @param lsName 
 * @param ports 
 * 
 * @return OfDpReturn 
 */
OfDpReturn OfDpConfigApi::Get(const std::string &lsName,
                              std::list<OfDpPortAttachmentT> &ports) const {
    const OfDpLogicalSw *ls = static_cast<const OfDpLogicalSw *>(GetLogicalSw(lsName));

    if (ls) {
        std::lock_guard<std::recursive_mutex> xLock(ls->GetPorts().mutex());
        for(const auto& pPort : ls->GetPorts()| boost::adaptors::map_values  ){
            ports.emplace_back(*pPort);
        }
        return OFDP_OK;
    }

    return OFDP_ENTRY_NOT_EXIST;
}

/**
 * Get All switches connected to a controller
 * 
 * @author sraihan (7/25/2014)
 * 
 * @param ctlName 
 * @param switches 
 * 
 * @return OfDpReturn 
 */
OfDpReturn OfDpConfigApi::Get(const std::string &ctlName, std::list<OfDpLogicalSwT> &switches) const {
    std::lock_guard<std::recursive_mutex> xLock(mLogicalSwitches.mutex());
    const OfDpControllerT *ctl = GetController(ctlName);
    if (ctl) {
        for(const auto& pSwitch : mLogicalSwitches | boost::adaptors::map_values  ){
            if (static_cast<const OfDpLogicalSw&>(*pSwitch).GetAttachmentInfo(ctlName)) switches.emplace_back(*pSwitch);
        }
        DBG_TRACE_RET(OFDP_OK);
    }
    DBG_TRACE_RET(OFDP_ENTRY_NOT_EXIST);
}

/**
 * Dump all LS
 * 
 * @author sraihan (7/25/2014)
 * 
 * @param s: std::ostream
 */
void OfDpConfigApi::DumpLS(std::ostream&) const {
    std::lock_guard<std::recursive_mutex> xLock(mLogicalSwitches.mutex());
    char buffer[8096];
    std::ostringstream out(buffer);
    for(const auto& pSwitch : mLogicalSwitches | boost::adaptors::map_values  ){
        out.seekp(0, std::ios_base::beg);
        out << static_cast<const OfDpLogicalSw&>(pSwitch->GetConfigObject())
            << std::ends;
        std::cout << out.str();
    }
}

#ifdef STANDALONE
#include "RestMsgHandler.h"
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>

static void ChangeStackSize() { // ifneeded
    rlimit rl;
    if (getrlimit(RLIMIT_STACK, &rl) == 0) {
        if (rl.rlim_cur < (16 * 1024 * 1024)) {
            rl.rlim_cur = (16 * 1024 * 1024);
            setrlimit(RLIMIT_STACK, &rl);
        }
    }
}

/// Defintions for cofa executable
/// Implement logging mechanism here
std::string CofaLog(const char *format, ...) {
    va_list aptr;
    char buffer[4096]; buffer[0] = 0;
    va_start(aptr, format);
    vsnprintf(buffer, 4096, format, aptr);
    va_end(aptr);
    /// todo call syslog here
    return buffer;
}
bool mongoose_running = false;
void handle_signal(int sig) {
    if (mongoose_running) {
        cout << "Exiting..." << endl;
        mongoose_running = false;
        exit(0);
    }
}

int main(int argc, char **args) {
    gDbgMask = DBG_FLOW_ENTRY | DBG_ERR_OF | DBG_FLOW_TABLE | DBG_DP;
    // instantiates necessary objects for cofa
    ChangeStackSize();
    srand(time(NULL));

    signal(SIGINT, handle_signal);
    OfDataPlaneApi api;
    OfDpConfigApi::getInstance()->setDataPlaneApiPtr(&api);

    restMsgHandler restMsgHandler;

    Server server(8181);
    server.registerController(&restMsgHandler);
    server.setOption("enable_directory_listing", "false");


    // Start mongoose
    server.start();
    if (!mongoose_running) mongoose_running = true;
    cout << "Mongoose started, routes:" << endl;
    restMsgHandler.dumpRoutes();
    while (1) sleep(10);
    // done cofa initialization
    return 0;
}
#endif
