#ifndef __PLATFORM_DEFINE_H_
#define __PLATFORM_DEFINE_H_


//C++ language
#if defined(_MSC_VER)
#	if (_MSC_VER < 1900)
#		//About std.experimental.filesystem.
#		//Through VC2013 has provided <filesystem>, but all the names are given in namespace std. It's hard to alias these names into std::experimental,
#		//So Nana use nana.filesystem implement instead for VC2013
#
#		//Nana defines some macros for lack of support of keywords
#		define _ALLOW_KEYWORD_MACROS
#
#		define CXX_NO_INLINE_NAMESPACE //no support of C++11 inline namespace until Visual C++ 2015
#		define noexcept		//no support of noexcept until Visual C++ 2015
#
#		define constexpr	//no support of constexpr until Visual C++ 2015 ? const ??
#	else
#		undef STD_FILESYSTEM_NOT_SUPPORTED
#	endif
#endif //C++ language

// Select platform
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)	//Microsoft Windows
#define STDXX_WINDOWS
#endif

// MINGW ...
#if defined(__MINGW32__) || defined(__MINGW64__) || defined(MINGW)
#define STDXX_MINGW
#endif // MINGW

#if defined(APPLE) || defined(__APPLE__)	//Mac OS X
//Symbols for MACOS
#define STDXX_MACOS
#define STDXX_X11
#endif

#if (defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)) && !defined(_CRAYC)	//Linux
#define STDXX_LINUX
#define STDXX_X11
#endif

#if defined(__OpenBSD__)
#define STDXX_UNIX
#endif

#if defined(__NetBSD__) || defined(__FreeBSD__) || defined(__DragonFly__)
#define STDXX_UNIX
#endif // Select platform


//Define a symbol for POSIX operating system.
#if defined(STDXX_LINUX) || defined(STDXX_MACOS) || defined(STDXX_UNIX)
#define STDXX_POSIX
#endif

// Select compiler begin
#if defined(_MSC_VER)	//Microsoft Visual C++
#define _SCL_SECURE_NO_WARNNGS
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable : 4996)

#	if (_MSC_VER >= 1900)
// google: break any code that tries to use codecvt<char16_t> or codecvt<char32_t>.
// google: It appears the C++ libs haven't been compiled with native char16_t/char32_t support.
// google: Those definitions are for codecvt<wchar_t>::id, codecvt<unsigned short>::id and codecvt<char>::id respectively.
// However, the codecvt<char16_t>::id and codecvt<char32_t>::id definitions aren't there, and indeed, if you look at locale0.cpp in the CRT source code you'll see they're not defined at all.
// google: That's a known issue, tracked by an active bug (DevDiv#1060849). We were able to update the STL's headers in response to char16_t/char32_t, but we still need to update the separately compiled sources.
#		define STD_CODECVT_NOT_SUPPORTED
#	endif // _MSC_VER == 1900

#	if (_MSC_VER < 1910) //VS2017 RTM
#		define _enable_std_clamp
#	endif

#elif defined(__clang__)	//Clang

#include <iosfwd>	//Introduces some implement-specific flags of ISO C++ Library
#if defined(__GLIBCPP__) || defined(__GLIBCXX__)
//<codecvt> is a known issue on libstdc++, it works on libc++
#define STD_CODECVT_NOT_SUPPORTED

#if !defined(__cpp_lib_make_unique) || (__cpp_lib_make_unique != 201304)
#ifndef _enable_std_make_unique
#define _enable_std_make_unique
#endif
#endif

#endif
#elif defined(__GNUC__) //GCC

#include <iosfwd>	//Introduces some implement-specific flags of ISO C++ Library
#if defined(__GLIBCPP__) || defined(__GLIBCXX__)
//<codecvt> is a known issue on libstdc++, it works on libc++
#define STD_CODECVT_NOT_SUPPORTED

//It's a known issue of libstdc++ on MinGW
//introduce to_string/to_wstring workarounds for disabled capacity of stdlib
#ifdef _GLIBCXX_HAVE_BROKEN_VSWPRINTF
#if (__GNUC__ < 5)
#	define STD_TO_STRING_NOT_SUPPORTED
#endif

#define STD_TO_WSTRING_NOT_SUPPORTED
#endif
#endif


#	if ((__GNUC__ < 5)   )
#		define _enable_std_put_time
#	endif

#if ((__GNUC__ > 5) || ((__GNUC__ == 5) && (__GNUC_MINOR__ >= 3 ) ) )
#	undef STD_FILESYSTEM_NOT_SUPPORTED
/// \todo define the namespace ????
#endif

#if (__GNUC__ == 4)
#if ((__GNUC_MINOR__ < 8) || (__GNUC_MINOR__ == 8 && __GNUC_PATCHLEVEL__ < 1))
#define STD_THREAD_NOT_SUPPORTED
#endif

#if (__GNUC_MINOR__ < 9)
#define _enable_std_make_unique
#endif

#if defined(NANA_MINGW)
#ifndef __MINGW64_VERSION_MAJOR
//It's a knonwn issue under MinGW(except MinGW-W64)
#define STD_NUMERIC_CONVERSIONS_NOT_SUPPORTED
#endif
#endif

#if (__GNUC_MINOR__ < 8)
//introduce to_string/to_wstring workaround for lack of stdlib definitions
#ifndef STD_TO_STRING_NOT_SUPPORTED
#	define STD_TO_STRING_NOT_SUPPORTED
#endif

#ifndef STD_TO_WSTRING_NOT_SUPPORTED
#	define STD_TO_WSTRING_NOT_SUPPORTED
#endif
#endif
#endif
#endif // Select compiler begin

#if defined(STDXX_WINDOWS)
#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef UNICODE
#define UNICODE
#endif
#endif
#endif // __PLATFORM_DEFINE_H_
