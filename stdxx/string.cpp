/**
* @filename string.cpp
* @brief 字符串操作
* @author 
* @date 2015年6月26日
* @note
*/

#pragma once
#if _MSC_VER >= 1100
#pragma warning(disable: 4996)
#endif

#include <vector>
#include <string>
#include <stdarg.h>
#include <algorithm>
#include "string.h"
#include "stdsafe.h"
#include <string.h>
#include <time.h>

# ifdef _WIN32
#  define FNM_NOMATCH 1
# endif

#ifdef _WIN32
# include <Shlwapi.h>
# pragma comment(lib, "shlwapi.lib")
#endif

namespace stdxx
{

	std::string format(const char *fmt, ...)
	{
		char msg[4096] = { 0 };
		va_list args;
		va_start(args, fmt);
		vsnprintf(msg, _countof(msg), fmt, args);
		va_end(args);

		return std::move(std::string(msg));
	}

	std::wstring format(const wchar_t *fmt, ...)
	{
		wchar_t msg[4096] = { 0 };
		va_list args;
		va_start(args, fmt);
		vswprintf(msg, _countof(msg), fmt, args);
		va_end(args);

		return std::move(std::wstring(msg));
	}

	std::string format_datatime(time_t t)
	{
		char buffer[64] = { 0 };
		struct tm *timeinfo = localtime(&t);
		strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
		return std::string(buffer);
	}

    std::string format_num_thousandth(int64_t num) {
        std::string tmp = format("%ld", num);
        std::string s;

        int n = 0;
        auto it = tmp.rbegin();
        while (it != tmp.rend()) {
            n++;

            s.insert(s.begin(), *it);
            ++it;
            if (it != tmp.rend() && (n % 3 == 0)) {
                s.insert(s.begin(), ',');
            }
        }

        return std::move(s);
    }

	//template<class T>
    //std::string to_string(const T& t)
    //{
    //    std::ostringstream oss;//创建一个流
    //    oss << t;//把值传递如流中
    //    return oss.str();//获取转换后的字符转并将其写入result
    //}

	std::vector<std::string> split_string(const std::string &str, const std::string &delim, const bool trim_empty /*= false*/)
	{
		size_t pos, last_pos = 0, len;
		std::vector<std::string> tokens;

		while (true) 
		{
			pos = str.find(delim, last_pos);
			if (pos == std::string::npos) {
				pos = str.size();
			}

			len = pos - last_pos;
			if (!trim_empty || len != 0) {
				tokens.push_back(str.substr(last_pos, len));
			}

			if (pos == str.size()) {
				break;
			}
			else {
				last_pos = pos + delim.size();
			}
		}

		return tokens;
	}

    std::vector<std::wstring> split_string(const std::wstring &str, const std::wstring &delim, const bool trim_empty)
    {
        size_t pos, last_pos = 0, len;
        std::vector<std::wstring> tokens;

        while (true)
        {
            pos = str.find(delim, last_pos);
            if (pos == std::wstring::npos) {
                pos = str.size();
            }

            len = pos - last_pos;
            if (!trim_empty || len != 0) {
                tokens.push_back(str.substr(last_pos, len));
            }

            if (pos == str.size()) {
                break;
            }
            else {
                last_pos = pos + delim.size();
            }
        }

        return tokens;
    }

	std::string compose_string(const std::vector<std::string> &s, const std::string &delim) {
		std::string str;
		for (uint32_t n = 0; n < s.size(); ++n) {
			str += s[n];
			if (n != s.size() - 1) {
				str += delim;
			}
		}

		return std::move(str);
	}

	std::wstring compose_string(const std::vector<std::wstring> &s, const std::wstring &delim) {
		std::wstring str;
		for (uint32_t n = 0; n < s.size(); ++n) {
			str += s[n];
			if (n != s.size() - 1) {
				str += delim;
			}
		}

		return std::move(str);
	}

	void trim_left(std::string& str)
	{
        size_t pos = str.find_first_not_of(' ');
        if (pos != std::string::npos)
        {
            str.erase(0, pos);
        }
        else
        {
            str.clear();
        }
	}

	void trim_left(std::wstring& str) 
	{
        size_t pos = str.find_first_not_of(L' ');
        if (pos != std::wstring::npos)
        {
            str.erase(0, pos);
        }
        else
        {
            str.clear();
        }
	}

	void trim_right(std::string& str) 
	{
        size_t pos = str.find_last_not_of(' ');
        if (pos != std::string::npos)
        {
            str.erase(pos + 1);
        }
        else
        {
            str.clear();
        }
	}

    void trim_right(std::wstring& str)
	{
        size_t pos = str.find_last_not_of(L' ');
        if (pos != std::wstring::npos)
        {
            str.erase(pos + 1);
        }
        else
        {
            str.clear();
        }
    }

	void trim(std::string& str)
	{
		trim_left(str);
		trim_right(str);
	}

    void trim(std::wstring& str)
    {
        trim_left(str);
        trim_right(str);
    }

	std::string to_lower(const std::string& str) 
	{
		std::string t = str;
		transform(t.begin(), t.end(), t.begin(), tolower);
		return t;
	}

	std::wstring to_lower(const std::wstring& str) 
	{
		std::wstring t = str;
		transform(t.begin(), t.end(), t.begin(), tolower);
		return t;
	}

	std::string to_upper(const std::string& str) 
	{
		std::string t = str;
		transform(t.begin(), t.end(), t.begin(), toupper);
		return t;
	}

	std::wstring to_upper(const std::wstring& str) 
	{
		std::wstring t = str;
		transform(t.begin(), t.end(), t.begin(), toupper);
		return t;
	}

	bool starts_with(const std::string & value, const std::string & start)
	{
		if (start.size() > value.size())
			return false;

		return value.substr(0, start.size()) == start;
	}

	bool ends_with(const std::string & value, const std::string & ending)
	{
		if (ending.size() > value.size())
			return false;

		std::string source =
			value.substr(value.size() - ending.size(), ending.size());

		return source == ending;
	}

	std::string string_to_hex(const std::string& str)
	{
		static const char* const lut = "0123456789ABCDEF";
		size_t len = str.length();

		std::string output;
		output.reserve(2 * len);
		for (size_t i = 0; i < len; ++i)
		{
			const unsigned char c = str[i];
			output.push_back(lut[c >> 4]);
			output.push_back(lut[c & 15]);
		}
		return output;
	}

	std::string hex_to_string(const std::string& str)
	{
		static const char* const lut = "0123456789ABCDEF";
		size_t len = str.length();
		if (len & 1) return "";

		std::string output;
		output.reserve(len / 2);
		for (size_t i = 0; i < len; i += 2)
		{
			char a = str[i];
			const char* p = std::lower_bound(lut, lut + 16, a);
			if (*p != a) return "";

			char b = str[i + 1];
			const char* q = std::lower_bound(lut, lut + 16, b);
			if (*q != b) return "";

			output.push_back(((p - lut) << 4) | (q - lut));
		}
		return output;
	}


    template<typename CharT>
    void _replace_string(
        std::basic_string<CharT>& source,
        const std::basic_string<CharT>& old_value,
        const std::basic_string<CharT>& new_value)
    {
        for (
            size_t pos = source.find(old_value);
            pos != std::basic_string<CharT>::npos;
            pos = source.find(old_value, pos))
        {
            source.replace(pos, old_value.size(), new_value);
            pos += new_value.size();
        }
    }

    std::string replace_string(const std::string& source, const std::string& old_value, const std::string& new_value)
    {
        std::string tmp = source;
        _replace_string(tmp, old_value, new_value);
        return tmp;
    }

    std::wstring replace_string(const std::wstring& source, const std::wstring& old_value, const std::wstring& new_value)
    {
        std::wstring tmp = source;
        _replace_string(tmp, old_value, new_value);
        return tmp;
    }

    int string_comparenocase(const std::string &str1, const std::string &str2) {
        std::string s1 = to_lower(str1);
        std::string s2 = to_lower(str2);
        return strcmp(s1.c_str(), s2.c_str());
    }

    int string_comparenocase(const std::wstring &str1, const std::wstring &str2) {
        std::wstring s1 = to_lower(str1);
        std::wstring s2 = to_lower(str2);
        return wcscmp(s1.c_str(), s2.c_str());
    }

	bool string_strstrnocase(const std::string &str1, const std::string &str2)
	{
		std::string s1 = to_lower(str1);
		std::string s2 = to_lower(str2);
		return strstr(s1.c_str(), s2.c_str()) != NULL;
	}

	int string_fnmatch(const std::string& pattern, const std::string& str)
	{
#if defined(STDXX_WINDOWS)
		return ::PathMatchSpecA(str.c_str(), pattern.c_str()) ? 0 : FNM_NOMATCH;
#elif defined(STDXX_POSIX)
		return fnmatch(pattern.c_str(), str.c_str(), 0);
#endif
	}

	std::string generator_rand_string(const uint32_t count /*= 8*/)
	{
		std::string result = "";
		if (count <= 0) {
			return result;
		}
		result.resize(count);
		char meta_char[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
		srand(int(time(nullptr)));
		for (uint32_t i = 0; i < count; i++) {
			result[i] = meta_char[rand() % 62];
		}
        return result;
    }

    // http://stackoverflow.com/questions/1031645/how-to-detect-utf-8-in-plain-c
    bool is_utf8(const char *str)
    {
        if (!str) {
            return 0;
        }

        const unsigned char* bytes = (const unsigned char*)str;
        while (*bytes) {
            if ((// ASCII
                    bytes[0] == 0x09 ||
                    bytes[0] == 0x0A ||
                    bytes[0] == 0x0D ||
                    (0x20 <= bytes[0] && bytes[0] <= 0x7F)
                )
               ) {
                bytes += 1;
                continue;
            }

            if ((// non-overlong 2-byte
                    (0xC2 <= bytes[0] && bytes[0] <= 0xDF) &&
                    (0x80 <= bytes[1] && bytes[1] <= 0xBF)
                )
               ) {
                bytes += 2;
                continue;
            }

            if ((// excluding overlongs
                    bytes[0] == 0xE0 &&
                    (0xA0 <= bytes[1] && bytes[1] <= 0xBF) &&
                    (0x80 <= bytes[2] && bytes[2] <= 0xBF)
                ) ||
                (// straight 3-byte
                    ((0xE1 <= bytes[0] && bytes[0] <= 0xEC) ||
                     bytes[0] == 0xEE ||
                     bytes[0] == 0xEF) &&
                    (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
                    (0x80 <= bytes[2] && bytes[2] <= 0xBF)
                ) ||
                (// excluding surrogates
                    bytes[0] == 0xED &&
                    (0x80 <= bytes[1] && bytes[1] <= 0x9F) &&
                    (0x80 <= bytes[2] && bytes[2] <= 0xBF)
                )
               ) {
                bytes += 3;
                continue;
            }

            if ((// planes 1-3
                    bytes[0] == 0xF0 &&
                    (0x90 <= bytes[1] && bytes[1] <= 0xBF) &&
                    (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                    (0x80 <= bytes[3] && bytes[3] <= 0xBF)
                ) ||
                (// planes 4-15
                    (0xF1 <= bytes[0] && bytes[0] <= 0xF3) &&
                    (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
                    (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                    (0x80 <= bytes[3] && bytes[3] <= 0xBF)
                ) ||
                (// plane 16
                    bytes[0] == 0xF4 &&
                    (0x80 <= bytes[1] && bytes[1] <= 0x8F) &&
                    (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                    (0x80 <= bytes[3] && bytes[3] <= 0xBF)
                )
               ) {
                bytes += 4;
                continue;
            }

            return false;
        }

        return true;
    }

    bool string_compare(const std::string& str1, const std::string& str2, bool caseInsensitive) {
        if (!caseInsensitive) {
            return stdxx::to_lower(str1).compare(stdxx::to_lower(str2)) == 0;
        }
        return str1.compare(str2) == 0;
    }

    bool string_compare(const std::wstring& str1, const std::wstring& str2, bool caseInsensitive) {
        if (!caseInsensitive) {
            return stdxx::to_lower(str1).compare(stdxx::to_lower(str2)) == 0;
        }
        return str1.compare(str2) == 0;
    }
}