
#include "platform.h"

namespace mstd {
	bool is_little_endian() {
		int n = 0x01;
		char* p = (char*)&n;
		if (*p == 0x01) {
			return true;
		}

		return false;
	}

	bool is_big_endian() {
		return !is_little_endian();
	}
};