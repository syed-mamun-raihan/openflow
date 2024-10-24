#ifndef __OFDPOBSERVEDCONFIG_H_INCL__
#define __OFDPOBSERVEDCONFIG_H_INCL__
/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include <mutex>
#include <map>
#include <memory>
#include "OfDpDefine.hpp"


/**
 * GenericObservableConfig defines the interface
 * that all objects must implement to be used in GenericConfigObserver. 
 * All config Object must model key, =, < operator, Copy CTOR 
 * @class GenericObservableConfig
 * @brief It is base for objects that is exposed to ConfigAPI
 * @author   sraihan
 */
template<typename configObject>
class GenericObservableConfig : public configObject {
public:
    typedef typename configObject::key_type key_type;
    typedef configObject value_type;
    /**
     * @param configuration object
     * @brief CTOR
     */
    GenericObservableConfig(const configObject &param);

    /**
     * @param configuration object
     * @brief DTOR
     */
    virtual ~GenericObservableConfig() { }

    /**
     * @param None
     * @return const reference to configuration object
     * @brief getter
     */
    const configObject& GetConfigObject() const {
        return *static_cast<const configObject *>(this);
    }

    /**
     * @param none
     * @return non-const reference to configuration object
     * @brief setter
     */
    configObject& GetConfigObject() {
        return *static_cast<configObject *>(this);
    }

    /**
     * @param configuration object
     * @brief Updates the object
     */
    void Update(const configObject &config);

    /**
     * @param generic configuration object
     * @brief Comparator
     */
    bool operator==(const GenericObservableConfig<configObject> &obj) const;

    /**
     * @param configuration object
     * @brief Comparator
     */
    bool operator==(const configObject &obj) const;
};

/**
 * GenericConfigObserver defines the class that holds objects 
 * that will be observing its changes done by the owner. It 
 * maintains an internal storage. 
 * supports iterator concetps - can be used with range based for loops 
 * introduced in C++11 
 *  
 * @class GenericConfigObserver
 * @brief Configuration collector/storage
 * @author   sraihan
 */
template<typename configObject>
class GenericConfigObserver final {
public:
    typedef typename configObject::key_type key_type; /// Key Type must be modeled in configObjectType
    typedef typename std::unique_ptr<GenericObservableConfig<configObject> > value_type;
    typedef typename std::map<key_type, value_type> map_type;
    typedef typename map_type::iterator iterator;
    typedef typename map_type::const_iterator const_iterator;
    /**
     * @param none
     * @brief Constructor
     */
    GenericConfigObserver();

    /**
     * @param none
     * @brief Destructor
     */
    ~GenericConfigObserver();

    /**
     * @param Generic Observable Config derived objects
     * @brief add to storage
     */
    bool Attach(GenericObservableConfig<configObject> *);

    /**
     * @param Generic Observable Config derived objects
     * @brief Remove from storage
     */
    bool Detach(const key_type &name);

    /**
     * @param Generic Observable Config derived objects
     * @brief updates to objects
     */
    bool Notify(const configObject&);

    /**
     * caller Must check size and index overflow/underflow
     * @param
     * @return number of observers
     */
    size_t size() const;

    /**
     *\brief implements iterator concepts
     */
    typename GenericConfigObserver<configObject>::iterator begin();


    /**
     *\brief implements iterator concepts
     */
    typename GenericConfigObserver<configObject>::iterator end();


    /**
     *\brief implements iterator concepts
     */
    typename GenericConfigObserver<configObject>::iterator find(const key_type &key);


    /**
     *\brief implements iterator concepts
     */
    typename GenericConfigObserver<configObject>::const_iterator begin() const;


    /**
     *\brief implements iterator concepts
     */
    typename GenericConfigObserver<configObject>::const_iterator  end() const;


    /**
     *\brief implements iterator concepts
     */
    typename GenericConfigObserver<configObject>::const_iterator  find(const key_type &key) const; 


    std::recursive_mutex& mutex() const;

    /**
     * @param name == key
     * @brief getter
     */
    const configObject * operator [](const key_type&) const;

    /**
     * @param name == key
     * @brief getter
     */
    configObject * operator [](const key_type&);

    /**
     * @param iter 
     * @brief getter
     */
    configObject * operator [](iterator iter);
    /**
     * @param iter 
     * @brief getter
     */
    const configObject * operator [](iterator iter) const;
    /**
     * @param iter 
     * @brief getter
     */
    configObject * operator [](const_iterator iter);
    /**
     * @param iter 
     * @brief getter
     */
    const configObject * operator [](const_iterator iter) const;

private:
    map_type _observers; /// Storage or collector
    mutable std::recursive_mutex iterator_mutex;
}; // GenericConfigObserver<configObject>

struct OfDpLogicalSwT;
extern template class GenericObservableConfig<OfDpLogicalSwT>;
extern template class GenericConfigObserver<OfDpLogicalSwT>;
struct OfDpControllerT;
extern template class GenericObservableConfig<OfDpControllerT>;
extern template class GenericConfigObserver<OfDpControllerT>;
struct OfDpControllerAttachmentT;
extern template class GenericObservableConfig<OfDpControllerAttachmentT>;
extern template class GenericConfigObserver<OfDpControllerAttachmentT>;
struct OfDpPortInfoT;
extern template class GenericObservableConfig<OfDpPortInfoT>;
extern template class GenericConfigObserver<OfDpPortInfoT>;
struct OfDpPortAttachmentT;
extern template class GenericObservableConfig<OfDpPortAttachmentT>;
extern template class GenericConfigObserver<OfDpPortAttachmentT>;
struct OfDpGroupT;
extern template class GenericObservableConfig<OfDpGroupT>;
extern template class GenericConfigObserver<OfDpGroupT>;

#endif // __OFDPOBSERVEDCONFIG_H_INCL__
