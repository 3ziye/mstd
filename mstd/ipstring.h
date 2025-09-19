
#ifndef __IP_STRING_H_
#define __IP_STRING_H_

#ifdef MSTD_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#endif // MSTD_WINDOWS

#include <stddef.h>
#include <cstdio>
#include <algorithm>
#include <string>
#include <stdint.h>

namespace mstd {

std::string ip_to_string(const uint32_t ip);
uint32_t ip_from_string(const std::string &ip);

} // stdxx
#endif // #ifndef __IP_STRING_H_
