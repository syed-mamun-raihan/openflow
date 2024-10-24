/**
 * For commercial, educational or non-profit use please contact 
 * mamun@sraihan.com 
 * Otherwise use of this software is free to evaluate and test 
 * Copyright and Licenses owned by Syed Mamun Raihan 2018
 */
#include "OfpHeader.h"
#include "openflow_1.3.h"
extern "C"
int le_be_test() {
    std::cout << "le_be_test\n";
    const uint64_t le64 = 1;
    const uint64_t be64 = _htonll(le64);
    const uint64_t newle64 = _ntohll(be64);
    std::cout << " le64 " << le64 << " be64 " << be64 << " new le64 " << newle64 << "\n";
    return 0;
}

std::ostream& operator<<(std::ostream &out, const ofp_message &msg) {
    out << " msg(V: " << (unsigned)msg.version << " T: " <<  (unsigned)msg.type
        << " L: " << msg.get_length() << " X: " << msg.get_xid() << ") ";
    return out;
}

std::ostream& operator<<(std::ostream &out, const ofp_error_msg &msg) {
    out << "\n\tVersion: " << (int)msg.version << " error_type: " << msg.get_type() << " error_code: " << msg.get_code() << "\n";
    return out;
}

std::ostream& operator<<(std::ostream &out, const ofp_tlv_base &msg) {
    out << "\tofp_tlv_base:: type: " << msg.get_type() << " Length: " << msg.get_len() << "\n";
    return out;
}

std::ostream& operator<<(std::ostream &out, const oxm_fields_t &msg) {
    out << "\toxm_fields_t:: Class " << msg.getClass() << " Data type: " << msg.getField() << " Data Length: " << msg.getLength() << "\n";
    return out;
}

std::ostream& operator<<(std::ostream &out, const ofp_stats_msg &msg) {
    out << "\n\tVersion: " << (unsigned)msg.version << " Stats Type: " << msg.get_stats_type() << " Stats flags: " << msg.get_flags() << std::endl;
    return out;
}

