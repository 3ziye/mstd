#ifndef __PLATFORM_H_
#define __PLATFORM_H_

// MSTD_WINDOWS MSTD_LINUX MSTD_UNIX MSTD_APPLE

template <typename T = void>
struct is_x64 {
	static constexpr bool value = sizeof(T*) == 8;
};

#endif // __PLATFORM_DEFINE_H_
