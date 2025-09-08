#ifndef __HEX_H_
#define __HEX_H_

#ifdef MSTD_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#endif // MSTD_WINDOWS

#include <string>
namespace mstd {
    std::string to_hex(const char* buff, const int buff_len);
    std::string from_hex(const char* buff, const int buff_len);
}

#endif // __HEX_H_
