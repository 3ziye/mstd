
#ifndef MSTD_PATH_STRING_H_
#define MSTD_PATH_STRING_H_

#include <string>

namespace mstd
{
    template<typename CharT>
    CharT system_slash();
    template<typename CharT>
    CharT file_suffix();

#ifdef MSTD_WINDOWS
	template<>
	inline char system_slash<char>() { return '\\'; }
	template<>
	inline char file_suffix<char>() { return '.'; }
	template<>
	inline wchar_t system_slash<wchar_t>() { return L'\\'; }
	template<>
	inline wchar_t file_suffix<wchar_t>() { return L'.'; }
#else
	template<>
	inline char system_slash<char>() { return '/'; }
	template<>
	inline char file_suffix<char>() { return '.'; }
	template<>
	inline wchar_t system_slash<wchar_t>() { return L'/'; }
	template<>
	inline wchar_t file_suffix<wchar_t>() { return L'.'; }
#endif

	template<typename CharT>
	CharT reverse_system_slash();

#ifdef MSTD_WINDOWS
	template<>
	inline char reverse_system_slash<char>() { return '/'; }
	template<>
	inline wchar_t reverse_system_slash<wchar_t>() { return L'/'; }
#else
	template<>
	inline char reverse_system_slash<char>() { return '\\'; }
	template<>
	inline wchar_t reverse_system_slash<wchar_t>() { return L'\\'; }
#endif

	template<typename CharT>
	bool is_system_slash(CharT slash)
	{
#ifdef MSTD_WINDOWS
		return
			slash == system_slash<CharT>() ||
			slash == reverse_system_slash<CharT>();
#else
		return slash == system_slash<CharT>();
#endif
	}

std::string  compose_path(const char* first, const char* last);
std::string  compose_path(const std::string& first, const std::string& last);
std::wstring compose_path(const wchar_t* first, const wchar_t* last);
std::wstring compose_path(const std::wstring& first, const std::wstring& last);

std::string  compose_path(const char* first, const char* second, const char* third);
std::string  compose_path(const std::string& first, const std::string& second, const std::string& third);
std::wstring compose_path(const wchar_t* first, const wchar_t* last, const wchar_t* third);
std::wstring compose_path(const std::wstring& first, const std::wstring& second, const std::wstring& third);


std::string  get_parent_path(const char* path);
std::string  get_parent_path(const std::string& path);
std::wstring get_parent_path(const wchar_t* path);
std::wstring get_parent_path(const std::wstring& path);

std::string  get_last_path(const char* path);
std::string  get_last_path(const std::string& path);
std::wstring get_last_path(const wchar_t* path);
std::wstring get_last_path(const std::wstring& path);

std::string  get_file_suffix(const char* path);
std::string  get_file_suffix(const std::string& path);
std::wstring get_file_suffix(const wchar_t* path);
std::wstring get_file_suffix(const std::wstring& path);

std::string  get_local_sys_path(const char* path);
std::string  get_local_sys_path(const std::string& path);
std::wstring get_local_sys_path(const wchar_t* path);
std::wstring get_local_sys_path(const std::wstring& path);

}  // namespace stdxx

#endif
