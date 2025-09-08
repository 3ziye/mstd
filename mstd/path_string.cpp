
#include "path_string.h"
#include <assert.h>

namespace mstd
{
template<typename CharT>
typename std::basic_string<CharT>::size_type get_last_slash_pos(
    const std::basic_string<CharT>& path)
{
    std::basic_string<CharT> the_path(path);
#ifdef MSTD_WINDOWS
    typename std::basic_string<CharT>::size_type pos =
        the_path.rfind(system_slash<CharT>());
    typename std::basic_string<CharT>::size_type reverse_pos =
        the_path.rfind(reverse_system_slash<CharT>());
    if (pos != std::basic_string<CharT>::npos &&
        reverse_pos != std::basic_string<CharT>::npos)
    {
        return pos > reverse_pos ? pos : reverse_pos;
    }
    else if (pos != std::basic_string<CharT>::npos &&
        reverse_pos == std::basic_string<CharT>::npos)
    {
        return pos;
    }
    else if (pos == std::basic_string<CharT>::npos &&
        reverse_pos != std::basic_string<CharT>::npos)
    {
        return reverse_pos;
    }
    else
    {
        return std::basic_string<CharT>::npos;
    }
#else
    return the_path.rfind(system_slash<CharT>());
#endif
}

template<typename CharT>
std::basic_string<CharT> trim_slash(const CharT* path)
{
    std::basic_string<CharT> better_path;
    if (path != nullptr)
        better_path.assign(path);

    while (!better_path.empty() && is_system_slash<CharT>(better_path.back()))
    {
        better_path.pop_back();
    }
    return better_path;
}

template<typename CharT>
std::basic_string<CharT> _compose_path(
    const CharT* first,
    const CharT* last)
{
    std::basic_string<CharT> path;
    if (first != nullptr) 
        path.assign(first);

    // slash('\\' or '/')个数
    int slash_count = 0;
    if (!path.empty() && is_system_slash<CharT>(path.back()))
        slash_count++;
    if (last != nullptr && is_system_slash<CharT>(*last))
        slash_count++;

    // 修正slash('\\' or '/')
    if (slash_count == 0 && !path.empty())
        path.append(1, system_slash<CharT>());
    else if (slash_count == 2)
        last += 1;

    // 合并路径
    if (last != nullptr) 
        path.append(last);

    return path;
}

template<typename CharT>
std::basic_string<CharT> _get_parent_path(
    const CharT* path)
{
    std::basic_string<CharT> the_path = trim_slash(path);
    size_t pos = get_last_slash_pos(the_path);
    if (pos != std::basic_string<CharT>::npos)
    {
        return the_path.substr(0, pos);
    }
    else
    {
        return std::basic_string<CharT>();
    }
}

template<typename CharT>
std::basic_string<CharT> _get_last_path(
    const CharT* path)
{
    std::basic_string<CharT> the_path = trim_slash(path);
    size_t pos = get_last_slash_pos(the_path);
    if (pos != std::basic_string<CharT>::npos)
    {
        return the_path.substr(pos + 1);
    }
    else
    {
        return std::basic_string<CharT>(path);
    }
}

template<typename CharT>
std::basic_string<CharT> _get_file_suffix(
    const CharT* path)
{
    std::basic_string<CharT> str = path;
    std::basic_string<CharT> suffix;
    size_t pos = str.rfind(file_suffix<CharT>());
    if (pos != std::basic_string<CharT>::npos) {
        suffix = str.substr(pos, str.size() - pos);
    }
    
    return suffix;
}

std::string compose_path(const char* first, const char* last)
{
    return _compose_path(first, last);
}

std::string compose_path(const std::string& first, const std::string& last)
{
    return _compose_path(first.c_str(), last.c_str());
}

std::wstring compose_path(const wchar_t* first, const wchar_t* last)
{
    return _compose_path(first, last);
}

std::wstring compose_path(const std::wstring& first, const std::wstring& last)
{
    return _compose_path(first.c_str(), last.c_str());
}

std::string  compose_path(const char* first, const char* second, const char* third) {
    return _compose_path(_compose_path(first, second).c_str(), third);
}

std::string  compose_path(const std::string& first, const std::string& second, const std::string& third) {
    return _compose_path(_compose_path(first.c_str(), second.c_str()).c_str(), third.c_str());
}

std::wstring compose_path(const wchar_t* first, const wchar_t* second, const wchar_t* third) {
    return _compose_path(_compose_path(first, second).c_str(), third);
}

std::wstring compose_path(const std::wstring& first, const std::wstring& second, const std::wstring& third) {
    return _compose_path(_compose_path(first.c_str(), second.c_str()).c_str(), third.c_str());
}

std::string get_parent_path(const char* path)
{
    return _get_parent_path(path);
}

std::string get_parent_path(const std::string& path)
{
    return _get_parent_path(path.c_str());
}

std::wstring get_parent_path(const wchar_t* path)
{
    return _get_parent_path(path);
}

std::wstring get_parent_path(const std::wstring& path)
{
    return _get_parent_path(path.c_str());
}

std::string get_last_path(const char* path)
{
    return _get_last_path(path);
}

std::string get_last_path(const std::string& path)
{
    return _get_last_path(path.c_str());
}

std::wstring get_last_path(const wchar_t* path)
{
    return _get_last_path(path);
}

std::wstring get_last_path(const std::wstring& path)
{
    return _get_last_path(path.c_str());
}

std::string  get_file_suffix(const char* path)
{
    return _get_file_suffix(path);
}

std::string  get_file_suffix(const std::string& path)
{
    return _get_file_suffix(path.c_str());
}

std::wstring get_file_suffix(const wchar_t* path)
{
    return _get_file_suffix(path);
}

std::wstring get_file_suffix(const std::wstring& path)
{
    return _get_file_suffix(path.c_str());
}

template<typename CharT>
std::basic_string<CharT> _get_local_sys_path(
    const CharT* s) {
    std::basic_string<CharT> path = s;
    for (auto &c : path) {
        if (c == reverse_system_slash<CharT>()) c = system_slash<CharT>();
    }

    return path;
}

std::string  get_local_sys_path(const char* path)
{
    return _get_local_sys_path(path);
}
std::string  get_local_sys_path(const std::string& path)
{
    return _get_local_sys_path(path.c_str());
}

std::wstring get_local_sys_path(const wchar_t* path)
{
    return _get_local_sys_path(path);
}

std::wstring get_local_sys_path(const std::wstring& path)
{
    return _get_local_sys_path(path.c_str());
}

} 
