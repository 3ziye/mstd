
#ifndef __FILESYSTEM_EXT_H_
#define __FILESYSTEM_EXT_H_

#include "filesystem.h"

namespace fs = std::experimental::filesystem;

#if defined(STDXX_WINDOWS)
#include <time.h>
struct utimbuf {
	time_t actime;    /* access time */
	time_t modtime;    /* modification time */
};
#elif defined(STDXX_POSIX)
#include <utime.h>
#endif

namespace stdxx 
{
namespace filesystem_ext 
{

#if defined(STDXX_WINDOWS)
    constexpr auto const def_root = "C:";
    constexpr auto const def_rootstr = "C:\\";
    constexpr auto const def_rootname = "Local Drive(C:)";
#elif defined(STDXX_POSIX)
    constexpr auto const def_root = "/";
    constexpr auto const def_rootstr = "/";
    constexpr auto const def_rootname = "Root/";
#endif
 
std::experimental::filesystem::path path_user();    ///< extention ?

inline bool is_directory(const std::experimental::filesystem::directory_entry& dir) noexcept
{
    return is_directory(dir.status());
}

//template<class DI> // DI = directory_iterator from std, boost, or nana : return directory_entry
class directory_only_iterator : public std::experimental::filesystem::directory_iterator
{ 
	using directory_iterator = std::experimental::filesystem::directory_iterator;

	directory_only_iterator& find_first()
	{
		auto end = directory_only_iterator{};
		while (*this != end)
		{
			if (is_directory((**this).status())) 
				return *this;
			this->directory_iterator::operator++();
		}
		return *this;
	}
public:
	directory_only_iterator() = default;

	template <typename Arg, typename... Args>
	directory_only_iterator(Arg&& arg, Args&&... args) : directory_iterator(arg, std::forward<Args>(args)...)
	{
		find_first();
	}

    directory_only_iterator& operator++()
    {
		this->directory_iterator::operator++();
        return find_first();
    }
};

inline directory_only_iterator begin(directory_only_iterator iter) noexcept
{
	return iter;
}

inline directory_only_iterator end(const directory_only_iterator&) noexcept
{
	return{};
}

//template<class DI> // DI = directory_iterator from std, boost, or nana : value_type directory_entry
class regular_file_only_iterator : public std::experimental::filesystem::directory_iterator
{
	using directory_iterator = std::experimental::filesystem::directory_iterator;
	regular_file_only_iterator& find_first()
	{
		while (((*this) != directory_iterator{}) && !is_regular_file((**this).status()))
			this->directory_iterator::operator++();
		return (*this);
	}
public:
	regular_file_only_iterator() = default;

	template <typename Arg, typename... Args>
	regular_file_only_iterator(Arg&& arg, Args&&... args) : directory_iterator(std::forward<Arg>(arg), std::forward<Args>(args)...)
	{
		find_first();
	}
	
	regular_file_only_iterator& operator++()
	{
		this->directory_iterator::operator++();
		return find_first();
	}
};

inline regular_file_only_iterator begin(regular_file_only_iterator iter) noexcept
{
	return iter;
}

inline regular_file_only_iterator end(const regular_file_only_iterator&) noexcept
{
	return{};
}

std::string pretty_file_size(const std::string& path);
std::string pretty_file_date(const std::string& path);

bool set_modified_file_time(const std::string& path, const struct utimbuf *t);

}  // filesystem_ext
}  // stdxx

#endif //__FILESYSTEM_EXT_H_
