#ifndef __PLATFORM_H_
#define __PLATFORM_H_
#include <stdio.h>
//MSTD_WINDOWS MSTD_LINUX MSTD_UNIX MSTD_APPLE
namespace mstd {
	template <typename T = void>
	struct is_x64 {
		static constexpr bool value = sizeof(T*) == 8;
	};
	
	bool is_little_endian();
	bool is_big_endian();
};

#ifdef MSTD_WINDOWS
#define mstd_sprintf(buffer, fmt, ...) sprintf_s(buffer, sizeof(buffer), fmt, __VA_ARGS__)
#define mstd_strcpy(buffer, src) strcpy_s(buffer, sizeof(buffer), src)
#else
#define mstd_sprintf(buffer, fmt, ...) snprintf(buffer, sizeof(buffer), fmt, __VA_ARGS__)
#define mstd_strcpy(buffer, src) strncpy(buffer, src, sizeof(buffer))
#define _countof(arr) (sizeof(arr) / sizeof(arr[0]))
#endif

#endif // __PLATFORM_DEFINE_H_
