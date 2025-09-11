#include "string_convert.h"

#ifdef MSTD_WINDOWS
#include <Windows.h>
#endif

namespace mstd
{
#ifdef MSTD_WINDOWS
	int get_code_page_from_locale(const std::locale& locale) {
		// 获取locale的名称（如"Chinese (Simplified)_China.936"）
		const std::string loc_name = locale.name();

		// 尝试从locale名称提取代码页
		size_t dot_pos = loc_name.find('.');
		if (dot_pos != std::string::npos) {
			try {
				return std::stoi(loc_name.substr(dot_pos + 1));
			} catch (...) {
				// 提取失败时使用系统默认ANSI代码页
			}
		}
		// 获取系统默认ANSI代码页
		return GetACP();
	}


	std::string to_string(const std::wstring& wstr, const std::locale& locale) {
		if (wstr.empty()) {
			return std::string();
		}

		// 根据locale获取对应的代码页
		int code_page = get_code_page_from_locale(locale);
		// 计算需要的字节数（不包含终止符）
		int byte_count = WideCharToMultiByte(
			code_page,               // 使用locale对应的代码页
			0,                       // 转换标志
			wstr.c_str(),            // 源宽字符串
			static_cast<int>(wstr.size()),  // 源字符串长度
			nullptr,                 // 目标缓冲区（先计算长度）
			0,                       // 目标缓冲区大小（0表示仅计算）
			nullptr,                 // 无效字符替换（使用系统默认）
			nullptr                  // 是否使用了替换字符的标志
		);

		if (byte_count == 0) {
			return std::string();  // 转换失败
		}

		// 分配缓冲区并进行转换
		std::string result(byte_count, '\0');
		if (WideCharToMultiByte(
			code_page,
			0,
			wstr.c_str(),
			static_cast<int>(wstr.size()),
			&result[0],
			byte_count,
			nullptr,
			nullptr
		) == 0) {
			return std::string();  // 转换失败
		}

		return result;
	}

	std::wstring to_wstring(const std::string& str, const std::locale& locale) {
		if (str.empty()) {
			return std::wstring();
		}

		// 根据locale获取对应的代码页
		int code_page = get_code_page_from_locale(locale);

		// 计算需要的宽字符数（不包含终止符）
		int wide_char_count = MultiByteToWideChar(
			code_page,               // 使用locale对应的代码页
			0,                       // 转换标志
			str.c_str(),             // 源窄字符串
			static_cast<int>(str.size()),  // 源字符串长度
			nullptr,                 // 目标缓冲区（先计算长度）
			0                        // 目标缓冲区大小（0表示仅计算）
		);

		if (wide_char_count == 0) {
			return std::wstring();  // 转换失败
		}

		// 分配缓冲区并进行转换
		std::wstring result(wide_char_count, L'\0');
		if (MultiByteToWideChar(
			code_page,
			0,
			str.c_str(),
			static_cast<int>(str.size()),
			&result[0],
			wide_char_count
		) == 0) {
			return std::wstring();  // 转换失败
		}

		return result;
	}

	std::string to_u8string(const std::string &string, const std::locale& locale) {
		std::wstring wstr = to_wstring(string, locale);
		return to_u8string(wstr);
	}
#endif

	std::string to_u8string(const std::wstring &wstr, const std::locale& locale) {
		if (wstr.empty()) {
			return "";
		}
#ifdef MSTD_WINDOWS
		// 计算转换所需的缓冲区大小（UTF-8）
		int utf8_size = WideCharToMultiByte(
			CP_UTF8,               // 目标编码为UTF-8
			WC_ERR_INVALID_CHARS,  // 遇到无效字符时返回错误
			wstr.c_str(),          // 源宽字符串
			static_cast<int>(wstr.size()),  // 源字符串长度
			nullptr,               // 目标缓冲区（先不提供，仅计算大小）
			0,                     // 目标缓冲区大小（0表示仅计算所需大小）
			nullptr,               // 不使用默认字符
			nullptr                // 不跟踪是否使用了默认字符
		);

		// 检查是否计算大小失败
		if (utf8_size == 0) {
			return "";
		}

		// 分配缓冲区并进行转换
		std::string utf8_str(utf8_size, '\0');
		int result = WideCharToMultiByte(
			CP_UTF8,
			WC_ERR_INVALID_CHARS,
			wstr.c_str(),
			static_cast<int>(wstr.size()),
			&utf8_str[0],          // 目标缓冲区
			utf8_size,             // 目标缓冲区大小
			nullptr,
			nullptr
		);

		// 检查转换是否成功
		if (result == 0) {
			return "";
		}

		return utf8_str;
#else
		// 使用UTF-8编码转换器
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
		try {
			// 进行转换
			return converter.to_bytes(wstr);
		} catch (const std::range_error& e) {
			return std::string();
		}
#endif
	}

#ifdef MSTD_WINDOWS
    std::string u8_to_string(const std::string &u8string) {
		return to_string(u8_to_wstring(u8string));
	}
#endif

    std::wstring u8_to_wstring(const std::string &utf8_str) {
		if (utf8_str.empty()) {
			return L"";
		}

#ifdef MSTD_WINDOWS
		// 第一步：计算转换所需的宽字符缓冲区大小
		int wide_size = MultiByteToWideChar(
			CP_UTF8,               // 源编码为UTF-8
			MB_ERR_INVALID_CHARS,  // 遇到无效UTF-8序列时返回错误
			utf8_str.c_str(),      // 源UTF-8字符串
			static_cast<int>(utf8_str.size()),  // 源字符串长度（字节数）
			nullptr,               // 目标缓冲区（先不提供，仅计算大小）
			0                      // 目标缓冲区大小（0表示仅计算所需大小）
		);

		// 检查大小计算是否失败
		if (wide_size == 0) {
			return std::wstring();
		}

		// 第二步：分配缓冲区并执行实际转换
		std::wstring wstr(wide_size, L'\0');
		int result = MultiByteToWideChar(
			CP_UTF8,
			MB_ERR_INVALID_CHARS,
			utf8_str.c_str(),
			static_cast<int>(utf8_str.size()),
			&wstr[0],              // 目标宽字符串缓冲区
			wide_size              // 目标缓冲区大小（宽字符数）
		);

		// 检查转换是否成功
		if (result == 0) {
			return std::wstring();
		}

		return wstr;
#else
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    try {
        // 执行转换
        return converter.from_bytes(utf8_str);
    } catch (const std::range_error& e) {
		return std::wstring();
    }
#endif
    }

}
