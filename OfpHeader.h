#ifndef _OFPHEADER_H
#define _OFPHEADER_H
/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include <cstdint>
#include <iostream>
#include <string>
#include <ctime>
#include <ostream>
#include <sstream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <limits>
#include <type_traits>
#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <cassert>
#include <climits>



// OFDP_VLAN_NONE is defined in OfDpDefine.h
#define FROM_OFDP_VLAN_NONE_TO_0(vlan) ((vlan) == OFDP_VLAN_NONE ? 0 : (vlan))
#define FROM_0_TO_OFDP_VLAN_NONE(vlan) ((vlan) == 0 ? OFDP_VLAN_NONE : (vlan))
#define TEST_GOOD_ENCODING(BASE_POINTER, MAX_LENGTH, TEST_POINTER)	reinterpret_cast<const uint8_t*>((BASE_POINTER))+(MAX_LENGTH) > \
									reinterpret_cast<const uint8_t*>((TEST_POINTER))

/* OpenFlow: protocol between controller and datapath. */
/*Note:
* Do not add Virtual Method or virtual base class
*/
// imported from packet.h in openvswitch
static inline bool
ip_is_cidr(uint32_t netmask) {
    uint32_t x = ~ntohl(netmask);
    return !(x & (x + 1));
}

static inline uint64_t _htonll(uint64_t val) { return htonl(1) == 1 ? val : (uint64_t)htonl(val) << 32 | htonl(val >> 32);}
static inline uint64_t _ntohll(uint64_t val) { return ntohl(1) == 1 ? val : (uint64_t)ntohl(val) << 32 | ntohl(val >> 32);}

// Header on all OpenFlow packets.
struct ofp_message {
    uint8_t version = 0;

    uint8_t type = 0;

    uint16_t length = 0;

    uint32_t xid = 0;

    ofp_message(const uint8_t ver_param = 0,
                const uint8_t type_param = 0,
                const uint16_t length_param = 0,
                const uint32_t xid_param = 0) noexcept :version(ver_param),
    type(type_param),
    length(htons(length_param)),
    xid(htonl(xid_param)) { }

    inline uint32_t get_xid() const;

    inline void set_xid(const uint32_t value);

    inline uint16_t get_length() const;

    inline void set_length(const uint16_t value);

    static inline uint64_t ntohll(uint64_t val);

    static inline uint64_t htonll(uint64_t val);

    friend std::ostream& operator<<(std::ostream &out, const ofp_message &msg);

    inline unsigned get_minimum_length() const { return sizeof(*this);}

};
inline uint32_t ofp_message::get_xid() const {
    return ntohl(xid);
}

inline void ofp_message::set_xid(const uint32_t value) {
    xid =  htonl(value);
}

inline uint16_t ofp_message::get_length() const {
    return ntohs(length);
}

inline void ofp_message::set_length(const uint16_t value) {
    length = htons(value);
}

inline uint64_t ofp_message::ntohll(uint64_t val) {
    return htonl(1) == 1 ? val : ((uint64_t)ntohl(val) << 32) | ntohl(val >> 32);
}

inline uint64_t ofp_message::htonll(uint64_t val) {
    return htonl(1) == 1 ? val : ((uint64_t)htonl(val) << 32) | htonl(val >> 32);
}

//32 bit aligned base struct for instructions/actions or any thing that follow TLV
struct ofp_tlv_base {
    uint16_t type_ = 0;

    uint16_t len_ = 0;

    uint8_t data_[0];

    ofp_tlv_base(const uint16_t type_param = 0,
                 const uint16_t len_param = 0) noexcept :type_(htons(type_param)),
    len_(htons(len_param)) { }

    inline uint16_t get_type() const;

    inline void set_type(const uint16_t value);

    inline uint16_t get_len() const;

    inline void set_len(const uint16_t value);

    inline unsigned get_minimum_length() const { return sizeof(*this);}

    inline const uint8_t* get_value_as_uint8ptr() const { return this->data_;}

    template<typename dataType> static inline size_t get_size_64bit_aligned() {
        const size_t pad_offset = sizeof(dataType) % 8;
        return (sizeof(dataType) + (pad_offset ? 8 - pad_offset : 0));
    }

    static inline size_t get_size_64bit_aligned(size_t data_len) {
        const size_t pad_offset = data_len % 8;
        return (data_len + (pad_offset ? 8 - pad_offset : 0));
    }

    template<typename dataType> static inline size_t get_size_32bit_aligned() {
        const size_t pad_offset = sizeof(dataType) % 4;
        return (sizeof(dataType) + (pad_offset ? 4 - pad_offset : 0));
    }

    static inline size_t get_size_32bit_aligned(size_t data_len) {
        const size_t pad_offset = data_len % 4;
        return (data_len + (pad_offset ? 4 - pad_offset : 0));
    }

    template<typename dataType> inline bool get_value(dataType &retVal) const {
        if (get_len() >= sizeof(dataType) + get_minimum_length()) {
            const dataType cur_data = *(reinterpret_cast<const dataType *>(this->data_));

            if (sizeof(dataType) == 2) retVal = ntohs(cur_data);
            else if (sizeof(dataType) == 4) retVal = ntohl(cur_data);
            else if (sizeof(dataType) == 8) retVal = ofp_message::ntohll(cur_data);
            else retVal = cur_data;
            return true;
        }
        return false;
    }


    template<typename dataType> inline bool set_value(const dataType param) {
        if (get_len() >= sizeof(dataType) + get_minimum_length()) {
            uint8_t *cur_data_ptr = this->data_;
            if (sizeof(dataType) == 2) *reinterpret_cast<dataType *>(cur_data_ptr) = htons(param);
            else if (sizeof(dataType) == 4) *reinterpret_cast<dataType *>(cur_data_ptr) = htonl(param);
            else if (sizeof(dataType) == 8) *reinterpret_cast<dataType *>(cur_data_ptr) = ofp_message::htonll(param);
            else *reinterpret_cast<dataType *>(cur_data_ptr) = param;
            return true;
        }
        return false;
    }

    inline const ofp_tlv_base* get_next() const;
    inline ofp_tlv_base* get_next();
    inline const ofp_tlv_base* get_next_padded() const;
    inline ofp_tlv_base* get_next_padded();

    friend std::ostream& operator<<(std::ostream &out, const ofp_tlv_base &msg);
};

inline const ofp_tlv_base* ofp_tlv_base::get_next() const {
    const ofp_tlv_base *next = reinterpret_cast<const ofp_tlv_base *>(reinterpret_cast<const uint8_t *>(this) + get_len());
    return next;
}

inline ofp_tlv_base* ofp_tlv_base::get_next() {
    ofp_tlv_base *next = reinterpret_cast<ofp_tlv_base *>(reinterpret_cast<uint8_t *>(this) + get_len());
    return next;
}

inline const ofp_tlv_base* ofp_tlv_base::get_next_padded() const {
    const ofp_tlv_base *next = reinterpret_cast<const ofp_tlv_base *>(reinterpret_cast<const uint8_t *>(this) +
                                                                      ofp_tlv_base::get_size_64bit_aligned(get_len()));
    return next;
}

inline ofp_tlv_base* ofp_tlv_base::get_next_padded() {
    ofp_tlv_base *next = reinterpret_cast<ofp_tlv_base *>(reinterpret_cast<uint8_t *>(this) +
                                                          ofp_tlv_base::get_size_64bit_aligned(get_len()));
    return next;
}

inline uint16_t ofp_tlv_base::get_type() const {
    return ntohs(type_);
}

inline void ofp_tlv_base::set_type(const uint16_t value) {
    type_ = htons(value);
}

inline uint16_t ofp_tlv_base::get_len() const {
    return ntohs(len_);
}

inline void ofp_tlv_base::set_len(const uint16_t value) {
    len_ = htons(value);
}

struct oxm_experimenter_fields /* public oxm_fields_t */ {

    uint32_t experimenter = 0;

    inline uint32_t get_experimenter() const;

    inline void set_experimenter(const uint32_t value);
    inline uint16_t get_minimum_length() const{ return sizeof(*this); }

};

inline uint32_t oxm_experimenter_fields::get_experimenter() const {
    return ntohl(experimenter);
}

inline void oxm_experimenter_fields::set_experimenter(const uint32_t value) {
    experimenter = htonl(value);
}


//OXM Class IDs.
//The high order bit differentiate reserved classes from member classes.
//Classes 0x0000 to 0x7FFF are member classes, allocated by ONF.
//Classes 0x8000 to 0xFFFE are reserved classes, reserved for standardisation.

enum ofp_oxm_class {
    OFPXMC_NXM_0 =    0x0000,
    OFPXMC_NXM_1 =    0x0001,
    OFPXMC_OPENFLOW_BASIC =    0x8000,
    OFPXMC_EXPERIMENTER =    0xFFFF
};

//Components of a OXM TLV header.
struct oxm_fields_t {
    uint32_t header_ = 0;
    uint8_t  data_[0];
    oxm_fields_t(const uint32_t header_param = 0) noexcept : header_(htonl(header_param)) { }
    oxm_fields_t(const oxm_fields_t&) noexcept = default;
    oxm_fields_t(oxm_fields_t&&)noexcept = default;

    inline unsigned get_minimum_length() const { return sizeof(*this);}

    //#define OXM_LENGTH(HEADER) ((HEADER) & 0xff)
    inline uint8_t getLength() const;

    //#define OXM_HASMASK(HEADER) (((HEADER) >> 8) & 1)
    inline bool hasMask() const;

    //#define OXM_FIELD(HEADER) (((HEADER) >> 9) & 0x7f)
    inline uint8_t getField() const;

    //#define OXM_CLASS(HEADER) ((HEADER) >> 16)
    inline uint16_t getClass() const;

    //0x8000 or 0xFFFF
    inline bool isOpenflowBasic() const;


    //#define OXM_HEADER__(CLASS, FIELD, HASMASK, LENGTH)  (((CLASS) << 16) | ((FIELD) << 9) | ((HASMASK) << 8) | (LENGTH))
    //##########################################
    //#define OXM_HEADER(CLASS, FIELD, LENGTH) OXM_HEADER__(CLASS, FIELD, 0, LENGTH)
    inline void setHeader(uint16_t class_, uint8_t field, uint8_t length = 0, uint8_t hasMask = false);

    //#define OXM_HEADER_W(CLASS, FIELD, LENGTH) OXM_HEADER__(CLASS, FIELD, 1, (LENGTH) * 2)
    inline void setHeaderW(uint16_t class_, uint8_t field, uint8_t length);

    //#define OXM_MAKE_WILD_HEADER(HEADER) OXM_HEADER_W(OXM_CLASS(HEADER), OXM_FIELD(HEADER), OXM_LENGTH(HEADER))
    inline void makeWildHeader(uint32_t header);
    // following functions interprets value that usually follows the header field
    // which is not shown as a member
    inline const oxm_fields_t* get_next() const;

    inline oxm_fields_t* get_next();

    inline const oxm_fields_t* get_next_exp() const;

    inline oxm_fields_t* get_next_exp();

    inline bool get_value_as_uint8ptr(uint8_t *&retval) const;

    inline bool get_mask_as_uint8ptr(uint8_t *&retval) const;

    template<typename dataType> 
    inline bool get_value(dataType &retVal) const {

        uint8_t *cur_data_ptr = 0;
        if (getLength() >= sizeof(dataType) && get_value_as_uint8ptr(cur_data_ptr)) {
            const dataType cur_data = *(reinterpret_cast<const dataType *>(cur_data_ptr));

            if (sizeof(dataType) == 2) retVal = ntohs(cur_data);
            else if (sizeof(dataType) == 4) retVal = ntohl(cur_data);
            else if (sizeof(dataType) == 8) retVal = ofp_message::ntohll(cur_data);
            else retVal = cur_data;
            return true;
        }
        retVal = 0;
        return false;
    }

    template<typename dataType> 
    inline bool get_mask(dataType &retVal) const {

        uint8_t *cur_mask_ptr = 0;
        if (getLength() / 2 == sizeof(dataType) && get_mask_as_uint8ptr(cur_mask_ptr)) {
            const dataType cur_data = *(reinterpret_cast<const dataType *>(cur_mask_ptr));

            if (sizeof(dataType) == 2) retVal = ntohs(cur_data);
            else if (sizeof(dataType) == 4) retVal = ntohl(cur_data);
            else if (sizeof(dataType) == 8) retVal = ofp_message::ntohll(cur_data);
            else retVal = cur_data;
            return true;
        }
        retVal = 0;
        return false;
    }

    // assumption is, dataType is basic type of 1,2,4,8 bytes size
    template<typename dataType> 
    inline bool set_value(const dataType param) {
        uint8_t *cur_data_ptr = 0;
        if (getLength() >= sizeof(dataType)
            && get_value_as_uint8ptr(cur_data_ptr)
            && !std::is_class<dataType>::value) {
            if (sizeof(dataType) == 2) {
                *reinterpret_cast<dataType *>(cur_data_ptr) = htons(param);
            } else if (sizeof(dataType) == 4) {
                *reinterpret_cast<dataType *>(cur_data_ptr) = htonl(param);
            } else if (sizeof(dataType) == 8) {
                *reinterpret_cast<dataType *>(cur_data_ptr) = ofp_message::htonll(param);
            } else { // object type
                *reinterpret_cast<dataType *>(cur_data_ptr) = param;
            } // we still return true - left the user to assign as needed
            return true;
        }
        return false;
    }

    template<typename dataType> 
    inline bool set_object(const dataType param) {
        uint8_t *cur_data_ptr = 0;
        if (getLength() >= sizeof(dataType)
            && get_value_as_uint8ptr(cur_data_ptr)
            && std::is_class<dataType>::value) {
            *reinterpret_cast<dataType *>(cur_data_ptr) = param;
        }
        return true;
    }

    // assumption is, dataType is basic type of 1,2,4,8 bytes size
    template<typename dataType>
    inline bool set_mask(const dataType param) {
        uint8_t *cur_mask_ptr = 0;
        if (getLength() / 2 == sizeof(dataType) && get_mask_as_uint8ptr(cur_mask_ptr)) {
            if (sizeof(dataType) == 2) *reinterpret_cast<dataType *>(cur_mask_ptr) = htons(param);
            else if (sizeof(dataType) == 4) *reinterpret_cast<dataType *>(cur_mask_ptr) = htonl(param);
            else if (sizeof(dataType) == 8) *reinterpret_cast<dataType *>(cur_mask_ptr) = ofp_message::htonll(param);
            else *reinterpret_cast<dataType *>(cur_mask_ptr) = param;
            return true;
        }
        return false;
    }

    friend std::ostream& operator<<(std::ostream &out, const oxm_fields_t &msg);

};

//#define OXM_LENGTH(HEADER) ((HEADER) & 0xff)
inline uint8_t oxm_fields_t::getLength() const {
    return ntohl(header_) & 0xff; // 1st byte
}

//#define OXM_HASMASK(HEADER) (((HEADER) >> 8) & 1)
inline bool oxm_fields_t::hasMask() const {
    return ((ntohl(header_) >> 8) & 0x01);
}

//#define OXM_FIELD(HEADER) (((HEADER) >> 9) & 0x7f)
inline uint8_t oxm_fields_t::getField() const {
    return ((ntohl(header_) >> 9) & 0x7f);
}

//#define OXM_CLASS(HEADER) ((HEADER) >> 16)
inline uint16_t oxm_fields_t::getClass() const {
    const uint16_t temp = (ntohl(header_) >> 16);
    return temp;
}

inline bool oxm_fields_t::get_value_as_uint8ptr(uint8_t *&retVal) const {
    if (getLength() >= sizeof(uint8_t)) {
        retVal = const_cast<oxm_fields_t *>(this)->data_;
        return true;
    }
    retVal = 0;
    return false;
}

inline bool oxm_fields_t::get_mask_as_uint8ptr(uint8_t *&retVal) const {
    if (getLength() / 2 >= sizeof(uint8_t) && hasMask()) {
        const uint8_t *cur_data_ptr = this->data_ + getLength() / 2;
        retVal = const_cast<uint8_t *>(cur_data_ptr);
        return true;
    }
    retVal = 0;
    return false;
}

inline const oxm_fields_t* oxm_fields_t::get_next() const {
    // this + header_length + data_length
    const oxm_fields_t *next_element = reinterpret_cast<const oxm_fields_t *>(this->data_ + getLength());
    return next_element;
}

inline oxm_fields_t* oxm_fields_t::get_next() {
    // this + header_length + data_length
    oxm_fields_t *next_element = reinterpret_cast<oxm_fields_t *>(this->data_ + getLength());
    return next_element;
}

inline const oxm_fields_t* oxm_fields_t::get_next_exp() const {
    // this + header_length + data_length
    const oxm_fields_t *next_element = reinterpret_cast<const oxm_fields_t *>(this->data_ + getLength() + sizeof(oxm_experimenter_fields));
    return next_element;
}

inline oxm_fields_t* oxm_fields_t::get_next_exp() {
    // this + header_length + data_length
    oxm_fields_t *next_element = reinterpret_cast<oxm_fields_t *>(this->data_ + getLength() + sizeof(oxm_experimenter_fields));
    return next_element;
}

//0x8000
inline bool oxm_fields_t::isOpenflowBasic() const {
    return ((this->getClass() == ::OFPXMC_OPENFLOW_BASIC) ||
            (this->getClass() == ::OFPXMC_EXPERIMENTER));
}

//#define OXM_HEADER__(CLASS, FIELD, HASMASK, LENGTH) (((CLASS) << 16) | ((FIELD) << 9) | ((HASMASK) << 8) | (LENGTH))
//##########################################
//#define OXM_HEADER(CLASS, FIELD, LENGTH) OXM_HEADER__(CLASS, FIELD, 0, LENGTH)
inline void oxm_fields_t::setHeader(uint16_t class_, uint8_t field, uint8_t length, uint8_t hasMask) {
    header_ = htonl(class_ << 16 | ((field & 0x7F) << 9) | ((hasMask & 0x01) << 8) | length);
}

//#define OXM_HEADER_W(CLASS, FIELD, LENGTH) OXM_HEADER__(CLASS, FIELD, 1, (LENGTH) * 2)
inline void oxm_fields_t::setHeaderW(uint16_t class_, uint8_t field, uint8_t length) {
    setHeader(class_, field, length * 2, 1);
}

//#define OXM_MAKE_WILD_HEADER(HEADER) OXM_HEADER_W(OXM_CLASS(HEADER), OXM_FIELD(HEADER), OXM_LENGTH(HEADER))
inline void oxm_fields_t::makeWildHeader(uint32_t header) {
    oxm_fields_t temp;
    temp.header_ = htonl(header);
    setHeaderW(temp.getClass(), temp.getField(), temp.getLength());
}

// OFPT_ERROR: Error message (datapath -> controller).
struct ofp_error_msg : public ofp_message {
    uint16_t error_type_ = 0;

    uint16_t error_code_ = 0;
    ofp_error_msg(const uint8_t ver_param = 0,
                  const uint8_t type_param = 0,
                  const uint16_t length_param = 0,
                  const uint32_t xid_param = 0,
                  const uint16_t error_type = 0,
                  const uint32_t error_code = 0) : ofp_message(ver_param, type_param, length_param, xid_param),
        error_type_(htons(error_type)), error_code_(htons(error_code)) { }
    uint8_t data[0];

    inline uint16_t get_type() const;

    inline void set_type(const uint16_t value);

    inline uint16_t get_code() const;

    inline void set_code(const uint16_t value);

    friend std::ostream& operator<<(std::ostream &out, const ofp_error_msg &msg);
    inline unsigned get_minimum_length() const { return sizeof(*this);}

};
inline uint16_t ofp_error_msg::get_type() const {
    return ntohs(error_type_);
}

inline void ofp_error_msg::set_type(const uint16_t value) {
    error_type_ = htons(value);
}

inline uint16_t ofp_error_msg::get_code() const {
    return ntohs(error_code_);
}

inline void ofp_error_msg::set_code(const uint16_t value) {
    error_code_ = htons(value);
}

typedef ofp_message ofp_hello;
typedef ofp_message ofp_echo;
typedef ofp_message ofp_echo_reply;
typedef ofp_message ofp_header;
typedef ofp_message ofp_get_config;

struct ofp_stats_msg : public ::ofp_message {
    uint16_t stats_type = 0;

    uint16_t flags = 0;

    uint32_t pad = 0;
    uint8_t body[0]; /* Body of the request. */
    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint16_t get_stats_type() const;

    inline void set_stats_type(const uint16_t value);

    inline uint16_t get_flags() const;

    inline void set_flags(const uint16_t value);

    friend std::ostream& operator<<(std::ostream &out, const ofp_stats_msg &msg);

};

inline uint16_t ofp_stats_msg::get_stats_type() const {
    return ntohs(stats_type);
}

inline void ofp_stats_msg::set_stats_type(const uint16_t value) {
    stats_type = htons(value);
}

inline uint16_t ofp_stats_msg::get_flags() const {
    return ntohs(ofp_stats_msg::flags);
}

inline void ofp_stats_msg::set_flags(const uint16_t value) {
    ofp_stats_msg::flags = htons(value);
}

enum ofp_stats_flags {
    OFPMPF_REQ_MORE = 1 << 0 /* More requests to follow. */
};
// More replies to follow.
enum ofp_stats_reply_flags {
    OFPSF_REPLY_MORE =    1 << 0
};

// OFPET_EXPERIMENTER: Error message (datapath -> controller).
struct ofp_error_experimenter_msg : public ::ofp_message {
    uint16_t exp_err_type = 0;

    uint16_t exp_type = 0;

    uint32_t experimenter = 0;

    uint8_t data[0];

    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint16_t get_type() const;

    inline void set_type(const uint16_t value);

    inline uint16_t get_exp_type() const;

    inline void set_exp_type(const uint16_t value);

    inline uint32_t get_experimenter() const;

    inline void set_experimenter(const uint32_t value);

};
inline uint16_t ofp_error_experimenter_msg::get_type() const {
    return ntohs(exp_err_type);
}

inline void ofp_error_experimenter_msg::set_type(const uint16_t value) {
    exp_err_type = htons(value);
}

inline uint16_t ofp_error_experimenter_msg::get_exp_type() const {
    return ntohs(exp_type);
}

inline void ofp_error_experimenter_msg::set_exp_type(const uint16_t value) {
    exp_type = htons(value);
}

inline uint32_t ofp_error_experimenter_msg::get_experimenter() const {
    return ntohl(experimenter);
}

inline void ofp_error_experimenter_msg::set_experimenter(const uint32_t value) {
    experimenter = htonl(value);
}

// Body for ofp_stats_request/reply of type OFPST_EXPERIMENTER.
// OFP_ASSERT(sizeof(ofp_experimenter_stats_header) == 8+16);
struct ofp_experimenter_stats_header : public ofp_stats_msg {
    uint32_t experimenter = 0;

    uint32_t exp_type = 0;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint32_t get_experimenter() const;

    inline void set_experimenter(const uint32_t value);

    inline uint32_t get_exp_type() const;

    inline void set_exp_type(const uint32_t value);

};
inline uint32_t ofp_experimenter_stats_header::get_experimenter() const {
    return ntohl(experimenter);
}

inline void ofp_experimenter_stats_header::set_experimenter(const uint32_t value) {
    experimenter = htonl(value);
}

inline uint32_t ofp_experimenter_stats_header::get_exp_type() const {
    return ntohl(exp_type);
}

inline void ofp_experimenter_stats_header::set_exp_type(const uint32_t value) {
    exp_type = value;
}

// Experimenter extension - additional customs messages
// OFP_ASSERT(sizeof(ofp_experimenter_header) == 16);
struct ofp_experimenter_header : public ::ofp_message {
    uint32_t experimenter = 0;

    uint32_t exp_type = 0;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint32_t get_experimenter() const;

    inline void set_experimenter(const uint32_t value);

    inline uint32_t get_exp_type() const;

    inline void set_exp_type(const uint32_t value);

};
inline uint32_t ofp_experimenter_header::get_experimenter() const {
    return ntohl(experimenter);
}

inline void ofp_experimenter_header::set_experimenter(const uint32_t value) {
    experimenter = htonl(value);
}

inline uint32_t ofp_experimenter_header::get_exp_type() const {
    return ntohl(exp_type);
}

inline void ofp_experimenter_header::set_exp_type(const uint32_t value) {
    exp_type = htonl(value);
}

// Action header for OFPAT_EXPERIMENTER.
// The rest of the body is experimenter-defined.
struct ofp_action_experimenter_header : public ::ofp_tlv_base {
    uint32_t experimenter = 0;

    inline uint32_t get_experimenter() const;

    inline void set_experimenter(const uint32_t value);

};
inline uint32_t ofp_action_experimenter_header::get_experimenter() const {
    return ntohl(experimenter);
}

inline void ofp_action_experimenter_header::set_experimenter(const uint32_t value) {
    experimenter = htonl(value);
}

// Instruction structure for experimental instructions
struct ofp_instruction_experimenter : public ::ofp_tlv_base {
    uint32_t experimenter = 0;

    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    inline uint32_t get_experimenter() const;

    inline void set_experimenter(const uint32_t value);

};
inline uint32_t ofp_instruction_experimenter::get_experimenter() const {
    return ntohl(experimenter) = 0;
}

inline void ofp_instruction_experimenter::set_experimenter(const uint32_t value) {
    experimenter = htonl(value);
}

/* Experimenter property for any thing*/
struct ofp_prop_experimenter : public ::ofp_tlv_base {
    uint32_t         experimenter = 0;  /* Experimenter ID which takes the same
                                       form as in struct
                                       ofp_experimenter_header. */
    uint32_t         exp_type = 0;      /* Experimenter defined. */
    /* Followed by:
     *   - Exactly (length - 12) bytes containing the experimenter data, then
     *   - Exactly (length + 7)/8*8 - (length) (between 0 and 7)
     *     bytes of all-zero bytes */
    uint32_t         experimenter_data[0];
    inline uint32_t get_experimenter() const;

    inline void set_experimenter(const uint32_t value);
    inline uint32_t get_exp_type() const;

    inline void set_exp_type(const uint32_t value);
};
inline uint32_t ofp_prop_experimenter::get_experimenter() const {
    return ntohl(experimenter);
}

inline void ofp_prop_experimenter::set_experimenter(const uint32_t value) {
    experimenter = htonl(value);
}
inline uint32_t ofp_prop_experimenter::get_exp_type() const {
    return ntohl(exp_type);
}

inline void ofp_prop_experimenter::set_exp_type(const uint32_t value) {
    exp_type = htonl(value);
}

//Bucket for use in groups.
struct ofp_bucket {
    uint16_t len = 0;

    uint16_t weight = 0;

    uint32_t watch_port = 0;

    uint32_t watch_group = 0;

    uint32_t pad = 0;

    ::ofp_tlv_base actions[0]; // struct ofp_action_header (1.2) 64bit aligned

    inline unsigned int get_minimum_length() const { return sizeof(*this);};

    inline uint16_t get_len() const;

    inline void set_len(const uint16_t value);

    inline uint16_t get_weight() const;

    inline void set_weight(const uint16_t value);

    inline uint32_t get_watch_port() const;

    inline void set_watch_port(const uint32_t value);

    inline uint32_t get_watch_group() const;

    inline void set_watch_group(const uint32_t value);

    inline ofp_bucket* get_next();

    inline const ofp_bucket* get_next() const;

};
inline uint16_t ofp_bucket::get_len() const {
    return ntohs(len);
}

inline void ofp_bucket::set_len(const uint16_t value) {
    len = htons(value);
}

inline uint16_t ofp_bucket::get_weight() const {
    return ntohs(weight);
}

inline void ofp_bucket::set_weight(const uint16_t value) {
    weight = htons(value);
}

inline uint32_t ofp_bucket::get_watch_port() const {
    return ntohl(watch_port);
}

inline void ofp_bucket::set_watch_port(const uint32_t value) {
    watch_port = htonl(value);
}

inline uint32_t ofp_bucket::get_watch_group() const {
    return ntohl(watch_group);
}

inline void ofp_bucket::set_watch_group(const uint32_t value) {
    watch_group = htonl(value);
}

inline ofp_bucket* ofp_bucket::get_next() {
    return reinterpret_cast<ofp_bucket *>(reinterpret_cast<uint8_t *>(this) + get_len());
}

inline const ofp_bucket* ofp_bucket::get_next() const {
    return reinterpret_cast<const ofp_bucket *>(reinterpret_cast<const uint8_t *>(this) + get_len());
}

// Action structure for OFPAT_SET_FIELD.
struct ofp_action_set_field : public ::ofp_tlv_base {
    oxm_fields_t fields;
    inline unsigned int get_minimum_length() const { return sizeof(*this);};
    // this template need to be used on a trusted basis
    template<typename dataType> 
    static inline size_t get_size(const unsigned extra_len = 0) {
        const size_t action_len_without_padding = sizeof(ofp_action_set_field) + sizeof(dataType) + extra_len;
        const size_t pad_offset = action_len_without_padding % 8;
        return (action_len_without_padding + (pad_offset ? 8 - pad_offset : 0));
    }

    // this template need to be used on a trusted basis
    template<typename dataType> 
    inline bool get_data(dataType *&val) const {
        val = 0;
        if (fields.get_value<dataType>(val)) return true;
        else return false;
    }

    template<typename dataType> 
    inline bool set_data(const uint16_t class_, const uint8_t field, const dataType &val) {
        fields.setHeader(class_, field, sizeof(dataType)); //
        return fields.set_value<dataType>(val);
    }

    template<typename dataType> 
    inline bool set_object(const uint16_t class_, const uint8_t field, const dataType &val) {
        fields.setHeader(class_, field, sizeof(dataType)); //
        return fields.set_object<dataType>(val);
    }

    inline bool set_byte_array(const uint16_t class_, const uint8_t field, const uint8_t len, const uint8_t *val) {
        fields.setHeader(class_, field, len); //
        uint8_t *data_ptr = 0;
        if (fields.get_value_as_uint8ptr(data_ptr)) {
            memcpy(data_ptr, val, len);
            return true;
        }
        return false;
    }
};

/* instructions header that is common to all instructions. The length includes the
* header and any padding used to make the action 64-bit aligned.
* NB: The length of an action *must* always be a multiple of eight. */
struct ofp_instruction_header  : public ::ofp_tlv_base {
    uint32_t pad = 0;
    inline unsigned int get_minimum_length() const { return sizeof(*this);};

};

/* Action header that is common to all actions. The length includes the
* header and any padding used to make the action 64-bit aligned.
* NB: The length of an action *must* always be a multiple of eight. */
struct ofp_action_header  : public ::ofp_tlv_base {
    uint32_t pad = 0;
    inline unsigned int get_minimum_length() const { return sizeof(*this);};

};


#endif
