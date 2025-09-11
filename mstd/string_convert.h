#ifndef __STRING_CONVERT_H_
#define __STRING_CONVERT_H_

#include "platform.h"
#if defined(_MSC_VER) && _MSC_VER >= 1800
#define CODECVT_ENABLE 1
#elif !defined(__GNUC__) || __GNUC__ >= 5
#define CODECVT_ENABLE 1
#endif

//Support GCC < 5 or clang < 6.1 and Don't want to use libiconv
//#define STDXX_CODECVT_ENABLE

#include <string>
#include <locale>
#include <vector>

#if defined(CODECVT_ENABLE)
#if defined(_LIBCPP_VERSION) || defined(_MSC_VER)
////GCC 5.0 add codecvt
#include <codecvt>
#elif defined(__GNUC__) && __GNUC__ >= 5
#include <codecvt>
#else
#include <bits/codecvt.h>
#endif
#else
#if !defined(STDXX_CODECVT_ENABLE)
#include <stdio.h>
#include <iconv.h>
#else
#if defined(__cplusplus) && __cplusplus >= 201103L
#define CODECVT_ENABLE 1
#include "wstring_convert.h"
#include "codecvt.h"
#endif
#endif
#endif

namespace mstd
{
#ifdef MSTD_WINDOWS
	// WBCS to MBCS
	std::string to_string(const std::wstring& wstring, const std::locale& locale = std::locale{ "" });
	// MBCS to WBCS
	std::wstring to_wstring(const std::string& string, const std::locale& locale = std::locale{ "" });
	// string to UTF-8
	std::string to_u8string(const std::string &string, const std::locale& locale = std::locale{ "" });
#endif
	// wstring to UTF-8
	std::string to_u8string(const std::wstring &wstring, const std::locale& locale = std::locale{ "" });
#ifdef MSTD_WINDOWS
	// UTF-8 to string
    std::string u8_to_string(const std::string &u8string);
#endif
    // UTF-8 to wstring
    std::wstring u8_to_wstring(const std::string &u8string);

}

#endif // __STRING_CONVERT_H_
