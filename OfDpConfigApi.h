#ifndef __OFDPCONFIGAPI_H_INCL__
#define __OFDPCONFIGAPI_H_INCL__
/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include <iostream>
#include <boost/unordered_map.hpp>
#include <mutex>
#include <list>
#include <atomic>

#include "OfConfig.hpp"
#include "GenericObservableConfig.hpp"
#include "OfDpDefine.hpp"
#include "OfDataPlaneApi.h"

/**
 * @class OfDpConfigApi OfDpConfigApi.h
 * @brief It is a singleton class that manages all aspect of COFA
 *
 * @author   sraihan
 */
class OfDpConfigApi final {
public:
    /**
     * Method to fetch/create singleton instance.
     * 
     * @author sraihan ()
     * 
     * @return OfDpConfigApi* 
     */
    static OfDpConfigApi* getInstance();

    /**
     * Add/Remove/Modify a logical switch
     * 
     * @author sraihan ()
     * 
     * @return OfDpReturn 
     */
    OfDpReturn Update(const OfDpLogicalSwT&);

    /**
     * @brief updates a flow on a logical switch; can create,modify, 
     *        & delete
     * 
     * @author sraihan ()
     * 
     * @param ls 
     * @param flow 
     * 
     * @return OfDpReturn 
     */
    OfDpReturn UpdateFlow(const OfDpLogicalSwT *ls, const OfDpFlowT &flow);

    /**
     * a const logical switch instance, null means none
     * 
     * @author sraihan ()
     * 
     * @param name 
     * 
     * @return const OfDpLogicalSwT* 
     */
    const OfDpLogicalSwT* GetLogicalSw(const std::string &name) const { return mLogicalSwitches[name]; }

    /**
     * all switches
     * 
     * @author sraihan ()
     * 
     * @return const GenericConfigObserver<OfDpLogicalSwT>& 
     */
    inline const GenericConfigObserver<OfDpLogicalSwT>& GetAllSwitches() const { return mLogicalSwitches;}

    /**
     * Add/Remove/Modify a controller
     * 
     * @author sraihan ()
     * 
     * @return OfDpReturn 
     */
    OfDpReturn Update(const OfDpControllerT&);

    /**
     * All Controllers 
     *  
     * @author sraihan ()
     * 
     * @return const GenericConfigObserver<OfDpControllerT>& 
     */
    inline const GenericConfigObserver<OfDpControllerT>& GetAllControllers() const { return mControllers;}

    /**
     * a Controller instance, null means none
     * 
     * @author sraihan ()
     * 
     * @param name 
     * 
     * @return const OfDpControllerT* 
     */
    const OfDpControllerT* GetController(const std::string &name) const { return mControllers[name]; }

    /**
     * Add/Remove/Modify a controller attachment
     * 
     * @author sraihan ()
     * 
     * @param attachment 
     * 
     * @return OfDpReturn 
     */
    OfDpReturn Update(const OfDpControllerAttachmentT &attachment);

    /**
     * Get All controller attachments
     * 
     * @author sraihan ()
     * 
     * @param lsName 
     * 
     * @return OfDpReturn 
     */
    OfDpReturn Get(const std::string &lsName, std::list<OfDpControllerAttachmentT>&) const;

    /**
     * Get All switches attached to a controller
     * 
     * @author sraihan ()
     * 
     * @param ctlName 
     * @param lsList 
     * 
     * @return OfDpReturn 
     */
    OfDpReturn Get(const std::string &ctlName, std::list<OfDpLogicalSwT> &lsList) const;

    /**
     * Get Attachment data
     * 
     * @author sraihan ()
     * 
     * @param lsName 
     * @param controllerName 
     * 
     * @return const OfDpControllerAttachmentT* 
     */
    const OfDpControllerAttachmentT* GetAttachmentInfo(const std::string &lsName, const std::string &controllerName) const;

    /**
     * Update ports attachments
     * 
     * @author sraihan ()
     * 
     * @param port_attachment 
     * 
     * @return OfDpReturn 
     */
    OfDpReturn Update(const  OfDpPortAttachmentT &port_attachment);

    /**
     * Get All port attachments
     * 
     * @author sraihan ()
     * 
     * @param lsName 
     * @param pList 
     * 
     * @return OfDpReturn 
     */
    OfDpReturn Get(const std::string &lsName, std::list<OfDpPortAttachmentT> &pList) const;

    /**
     * Add/Remove/Modify a Port
     * 
     * @author sraihan ()
     * 
     * @param port 
     * 
     * @return OfDpReturn 
     */
    OfDpReturn Update(const  OfDpPortInfoT &port);

    /**
     * return a single port
     * 
     * @author sraihan ()
     * 
     * @param key 
     * 
     * @return const OfDpPortInfoT* 
     */
    inline const OfDpPortInfoT* GetPort(const OfDpPortInfoT::key_type &key) const { return mPorts[key];}

    ///
    /// return ports use - range based for loop and adaptors to iterate
    ///
    inline const GenericConfigObserver<OfDpPortInfoT>& GetPorts() const { return mPorts;}

    ///
    /// return ports use - range based for loop and adaptors to iterate
    ///
    inline GenericConfigObserver<OfDpPortInfoT>& GetPorts() { return mPorts;}

    /**
     * Not defined yet
     * 
     * @author sraihan ()
     * 
     * @return OfDpReturn 
     */
    OfDpReturn CheckFlowTimeout();

    /**
     * @brief return number of flows
     * 
     * @author sraihan ()
     * 
     * @param name 
     * 
     * @return uint32_t 
     */
    uint32_t GetFlowCount(const std::string &name) const;

    /**
     * Receive packet
     * 
     * @author sraihan ()
     * 
     * @param portIdx 
     * @param phyPortIdx 
     * @param pkt 
     * @param pktLen 
     * 
     * @return OfDpReturn 
     */
    OfDpReturn ReceivePkt(uint32_t portIdx,
                          uint32_t phyPortIdx,
                          uint8_t  *pkt,
                          uint32_t pktLen);
    /**
     * @brief sets mpDataPlane so configurations can be stored to 
     *        hardware
     * 
     * @author sraihan ()
     * 
     * @param dataPlanePtr 
     */
    void setDataPlaneApiPtr(OfDataPlaneApi *dataPlanePtr);
    /**
     *  
     * @author sraihan ()
     * 
     * @return OfDataPlaneApi* 
     */
    OfDataPlaneApi* getDataPlaneApiPtr() const { return mpDataPlane;}

    /**
     * Dump all LS
     * 
     * @author sraihan (7/25/2014)
     * @param s: std::ostream
     */
    void DumpLS(std::ostream &s) const;

private:
    /**
     * Constructor - protected so users cannot call it.
     */
    OfDpConfigApi();

    /** 
     * Destructor 
     * Once in a lifetime set/no resetting 
     * 
     * @author sraihan ()
     */
    ~OfDpConfigApi() { OfDpConfigApi::mbInstanceDeleted = true; }

    /**
     * Non const version of GetPorts
     * 
     * @author sraihan ()
     * 
     * @param port 
     * @param ls 
     * @param info 
     * 
     * @return uint32_t 
     */
    uint32_t SendPortStatus(const OfDpPortInfoT &port, OfDpLogicalSwT &ls, OfDpPortStatusT &info);

    ///
    /// registered observers for config change
    ///
    GenericConfigObserver<OfDpLogicalSwT> mLogicalSwitches; /// LogicalSwitches collections
    GenericConfigObserver<OfDpControllerT> mControllers; /// controllers collections
    GenericConfigObserver<OfDpPortInfoT> mPorts; /// Ports collection
    
    boost::unordered_map<uint32_t, OfDpLogicalSwT *> _portToLogicalSw; /// Port idx to logical switch
    OfDataPlaneApi *mpDataPlane; /// Dataplane Api Interface

    static OfDpConfigApi _instance; ///the single instance of the object
    static volatile bool mbInstanceDeleted; /// It is set to false - reset to true bt DTOR

    ///
    /// Copy constructor
    /// Declared but not defined to prevent auto-generated
    /// copy constructor.  Refer to "Effective C++" by Meyers
    ///
    OfDpConfigApi(const OfDpConfigApi &src) = delete;

    ///
    /// Assignment operator
    /// Declared but not defined to prevent auto-generated
    /// assignment operator.  Refer to "Effective C++" by Meyers
    ///
    OfDpConfigApi& operator=(const OfDpConfigApi &src) = delete;

};


#endif
