
#include "ipstring.h"

namespace mstd {
std::string ip_to_string(const uint32_t ipaddr) {
	uint8_t ip[4];
	ip[0] = (ipaddr >> 24) & 0xFF;
	ip[1] = (ipaddr >> 16) & 0xFF;
	ip[2] = (ipaddr >> 8) & 0xFF;
	ip[3] = ipaddr & 0xFF;

	char buf[16] = { 0 };
	std::sprintf(buf, "%u.%u.%u.%u", uint32_t(ip[0]), uint32_t(ip[1]), uint32_t(ip[2]), uint32_t(ip[3]));
	return buf;
}

uint32_t ip_from_string(const std::string &ip) {
	int buf[4];
	if (std::sscanf(ip.c_str(), "%d.%d.%d.%d", &buf[0], &buf[1], &buf[2], &buf[3]) != 4) {
		return 0;
	}

	uint32_t ipv4 = (
		buf[0] << 24 |
		buf[1] << 16 |
		buf[2] << 8 |
		buf[3]
		);
	return ipv4;
}

} // stdxx
