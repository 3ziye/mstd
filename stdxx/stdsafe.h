
#ifndef __STDSAFE_H_
#define __STDSAFE_H_

#include <stdio.h>

#ifdef WIN32
#define safe_sprintf(buffer, size, fmt, ...) sprintf_s(buffer, size, fmt, __VA_ARGS__)
#define safe_strcpy(dst_str, src_str, size) strcpy_s(dst_str, size, src_str)
#else
#define safe_sprintf(buffer, size, fmt, ...) snprintf(buffer, size, fmt, __VA_ARGS__)
#define safe_strcpy(dst_str, src_str, size) strncpy(dst_str, src_str, size)
#define _countof(data)	(sizeof(data) / sizeof(data[0]))
#endif
#endif // __STDSAFE_H_