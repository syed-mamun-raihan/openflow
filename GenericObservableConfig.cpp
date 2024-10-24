/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include <iostream>
#include <utility>
#include <boost/range/adaptor/map.hpp>
#include "GenericObservableConfig.hpp"
#include "OfDpDefine.hpp"

/**
 *\param configuration object
 *\brief update based on name key and given command
 */
template<typename configObject>
GenericObservableConfig<configObject>::GenericObservableConfig(const configObject &param) : configObject(param) {
//    std::cout << "object key: " << configObject::key() << std::endl;
}

/**
 *\param configuration object
 *\brief update based on name key and given command
 */
template<typename configObject>
void GenericObservableConfig<configObject>::Update(const configObject &config) {
    if (this->key() == config.key()) {
        if (config.command == OFDP_CONFIG_MODIFY) {
            *this = config;
//            std::cout << "object key: " << configObject::key() << std::endl;
        } else if (config.command == OFDP_CONFIG_UPDATE_STATE) {
            this->adminState = config.adminState;
        }
    }
}

/**
 *\param generic observable configuration object
 *\brief compare using name key
 */
template<typename configObject>
bool GenericObservableConfig<configObject>::operator ==(const GenericObservableConfig<configObject> &obj) const {
    const configObject &rhs = static_cast<const configObject&>(obj);
    if (this->key() == rhs.key()) {
        return true;
    }
    return false;
}

/**
 *\param configuration object
 *\brief compare using name key
 */
template<typename configObject>
bool GenericObservableConfig<configObject>::operator ==(const configObject &config) const {
    if (this->key() == config.key()) {
        return true;
    }
    return false;
}

/**
 * Default constructor
 */
template<typename configObject>
GenericConfigObserver<configObject>::GenericConfigObserver() {
    // TODO: Add constructor code here
}

/**
 * Destructor
 */
template<typename configObject>
GenericConfigObserver<configObject>::~GenericConfigObserver() {
}

/**
 * Adds an observer to the internal notification list.
 * An observer is any object of a class that implements
 * the IOfDpObservedConfigObserver interface.
 *
 * @param o An object that wants to be added to the
 *          notification list
 */
template<typename configObject>
bool GenericConfigObserver<configObject>::Attach(GenericObservableConfig<configObject> *obj) {
    try {
        if (_observers.find(obj->GetConfigObject().key()) == _observers.end()) {
            _observers[obj->GetConfigObject().key()] = std::unique_ptr<GenericObservableConfig<configObject> >(obj);
            return true;
        }
    } catch (...) {
        std::cout << " cant attach a configObject [key=" << obj->GetConfigObject().key() << "]\n";
    }
    return false;
}

/**
 * Removes an observer from the internal notification list.
 * and also deletes it from the memory
 * @param o An object that wants to be removed from the
 *          notification list
 */

template<typename configObject>
bool GenericConfigObserver<configObject>::Detach(const key_type &key) {
    const auto& i= _observers.find(key);
    if (i != _observers.end()) {
        _observers.erase(i);
        return true;
    }
    return false;
}

/**
 * fetch an observer to the internal notification list.
 * @param name An object that wants to be fetched
 */
template<typename configObject>
const configObject* GenericConfigObserver<configObject>::operator [](const key_type &key) const {
    const auto& i= _observers.find(key);
    return ((i != _observers.end()) ? &(i->second.get()->GetConfigObject()) : nullptr);
}

/**
 * fetch an observer to the internal notification list.
 * @param name An object that wants to be fetched
 */
template<typename configObject>
configObject* GenericConfigObserver<configObject>::operator [](const key_type &key) {
    auto i= _observers.find(key);
    return ((i != _observers.end()) ? &(i->second.get()->GetConfigObject()) : nullptr);
}

/**
 * fetch an observer to the internal notification list.
 * @param iterator that wants to be fetched
 */
template<typename configObject>
configObject* GenericConfigObserver<configObject>::operator[](iterator i) {
    return (i != _observers.end() ? &(i->second.get()->GetConfigObject()) : nullptr);
}

/**
 * fetch an observer to the internal notification list.
 * @param iterator that wants to be fetched
 */
template<typename configObject>
const configObject* GenericConfigObserver<configObject>::operator[](iterator i) const {
    return (i != _observers.end() ? &(i->second.get()->GetConfigObject()) : nullptr);
}

/**
 * fetch an observer to the internal notification list.
 * @param iterator that wants to be fetched
 */
template<typename configObject>
configObject* GenericConfigObserver<configObject>::operator[](const_iterator i) {
    return (i != _observers.end() ? &(i->second.get()->GetConfigObject()) : nullptr);
}

/**
 * fetch an observer to the internal notification list.
 * @param iterator that wants to be fetched
 */
template<typename configObject>
const configObject* GenericConfigObserver<configObject>::operator[](const_iterator i) const {
    return (i != _observers.end() ? &(i->second.get()->GetConfigObject()) : nullptr);
}

template<typename configObject>
std::recursive_mutex& GenericConfigObserver<configObject>::mutex() const { return iterator_mutex; }

/**
 * caller Must check size and index overflow/underflow
 * @param
 * @return number of observers
 */
template<typename configObject>
size_t GenericConfigObserver<configObject>::size() const { return _observers.size();}

/**
 *\brief implements iterator concepts
 */
template<typename configObject>
typename GenericConfigObserver<configObject>::iterator GenericConfigObserver<configObject>::begin()
{ return _observers.begin();}

/**
 *\brief implements iterator concepts
 */
template<typename configObject>
typename GenericConfigObserver<configObject>::iterator GenericConfigObserver<configObject>::end()
{ return _observers.end();}

/**
 *\brief implements iterator concepts
 */
template<typename configObject>
typename GenericConfigObserver<configObject>::iterator GenericConfigObserver<configObject>::find(const key_type &key)
{ return _observers.find(key);}

/**
 *\brief implements iterator concepts
 */
template<typename configObject>
typename GenericConfigObserver<configObject>::const_iterator GenericConfigObserver<configObject>::begin() const
{ return _observers.begin();}

/**
 *\brief implements iterator concepts
 */
template<typename configObject>
typename GenericConfigObserver<configObject>::const_iterator  GenericConfigObserver<configObject>::end() const
{ return _observers.end();}

/**
 *\brief implements iterator concepts
 */
template<typename configObject>
typename GenericConfigObserver<configObject>::const_iterator  GenericConfigObserver<configObject>::find(const key_type &key) const
{ return _observers.find(key);}

/**
 * The list of objects are traversed and Update is called on 
 * target. 
 */
template<typename configObject>
bool GenericConfigObserver<configObject>::Notify(const configObject &o) {
    const auto& i= _observers.find(o.key());
    if (i != _observers.end()) {
        i->second->Update(o);
        return true;
    }
    return false;
}


// class instantiations
template class GenericObservableConfig<OfDpLogicalSwT>;
template class GenericConfigObserver<OfDpLogicalSwT>;

template class GenericObservableConfig<OfDpControllerT>;
template class GenericConfigObserver<OfDpControllerT>;

template class GenericObservableConfig<OfDpControllerAttachmentT>;
template class GenericConfigObserver<OfDpControllerAttachmentT>;

template class GenericObservableConfig<OfDpPortInfoT>;
template class GenericConfigObserver<OfDpPortInfoT>;

template class GenericObservableConfig<OfDpPortAttachmentT>;
template class GenericConfigObserver<OfDpPortAttachmentT>;

template class GenericObservableConfig<OfDpGroupT>;
template class GenericConfigObserver<OfDpGroupT>;

