
#include "platform.h"

namespace mstd {
	bool is_little_endian() {
		static bool ret = false;

		int n = 0x01;
		if (*(char*)&n == 0x01) {
			ret = true;
		}

		return ret;
	}

	bool is_big_endian() {
		static bool ret = !is_little_endian();
		return ret;
	}

	uint16_t htons(uint16_t host_value) {
		if (is_big_endian()) return host_value;

		return((host_value & 0xFF00) >> 8) | ((host_value & 0x00FF) << 8);
	}

	uint32_t htonl(uint32_t host_value) {
		if (is_big_endian()) return host_value;

		return ((host_value & 0xFF000000) >> 24) |
			((host_value & 0x00FF0000) >> 8) |
			((host_value & 0x0000FF00) << 8) |
			((host_value & 0x000000FF) << 24);
	}

	uint64_t htonll(uint64_t host_value) {
		if (is_big_endian()) return host_value;

		return ((host_value & 0xFF00000000000000ULL) >> 56) |
			((host_value & 0x00FF000000000000ULL) >> 40) |
			((host_value & 0x0000FF0000000000ULL) >> 24) |
			((host_value & 0x000000FF00000000ULL) >> 8) |
			((host_value & 0x00000000FF000000ULL) << 8) |
			((host_value & 0x0000000000FF0000ULL) << 24) |
			((host_value & 0x000000000000FF00ULL) << 40) |
			((host_value & 0x00000000000000FFULL) << 56);
	}

	uint16_t ntohs(uint16_t net_value) {
		if (is_big_endian()) return net_value;

		return htons(net_value);
	}

	uint32_t ntohl(uint32_t net_value) {
		if (is_big_endian()) return net_value;

		return htonl(net_value);
	}

	uint64_t ntohll(uint64_t net_value) {
		if (is_big_endian()) return net_value;

		return htonll(net_value);
	}
};