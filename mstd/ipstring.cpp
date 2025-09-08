
#include "ipstring.h"

namespace mstd {

const ipv4_address ipv4_address::any       = ipv4_address(  0,  0,  0,  0);
const ipv4_address ipv4_address::broadcast = ipv4_address(255,255,255,255);
const ipv4_address ipv4_address::loopback  = ipv4_address(127,  0,  0,  1);
const ipv4_address ipv4_address::none      = ipv4_address(255,255,255,255);

std::string ip_to_string(const uint32_t ip) {
    ipv4_address addr(ip);
    return addr.to_string();
}

uint32_t ip_from_string(const std::string &ip) {
    ipv4_address addr(ip);
    return addr.to_number();
}

} // stdxx
