#ifndef __STRING_CONVERT_H_
#define __STRING_CONVERT_H_

#include "platform.h"
#include <string>
#include <locale>
#include <vector>

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
