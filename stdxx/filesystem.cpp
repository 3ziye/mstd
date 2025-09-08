/*
 *	A ISO C++ FileSystem Implementation
 *	Copyright(C) 2003-2016 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/filesystem/filesystem.cpp
 *	@description:
 *		provide some interface for file managment
 */

#include "filesystem_ext.h"
#include "string_convert.h"
#include <vector>
#include <sstream>
#include <iomanip>	//put_time

#if defined(STDXX_WINDOWS)
    #include <windows.h>
    #if defined(STDXX_MINGW)
        #ifndef _WIN32_IE
            #define _WIN32_IE 0x0500
        #endif
    #endif

	#pragma warning(disable: 4091)
	#include <shlobj.h>
	#include "datetime.h"
#elif defined(STDXX_POSIX)
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <dirent.h>
	#include <cstdio>
	#include <cstring>
	#include <errno.h>
	#include <unistd.h>
	#include <stdlib.h>
	#include <fcntl.h>
#endif

#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__DragonFly__) || defined(__OpenBSD__) || defined(__APPLE__) || defined(__ANDROID__)
 #define fopen64 fopen
 #define ftello64 ftello
 #define fseeko64 fseeko
#endif
#ifdef _MSC_VER
 #define fopen64 fopen
 #if (_MSC_VER >= 1400) && (!(defined(NO_MSCVER_FILE64_FUNC)))
  #define ftello64 _ftelli64
  #define fseeko64 _fseeki64
 #else // old MSC
  #define ftello64 ftell
  #define fseeko64 fseek
 #endif
#endif

namespace fs = std::experimental::filesystem;

namespace stdxx
{
	namespace filesystem_ext
	{

		fs::path path_user()
		{
#if defined(STDXX_WINDOWS)
			wchar_t pstr[MAX_PATH];
			if (SUCCEEDED(SHGetFolderPath(0, CSIDL_PROFILE, 0, SHGFP_TYPE_CURRENT, pstr)))
				return pstr;
#elif defined(STDXX_LINUX) || defined(STDXX_MACOS)
			const char * pstr = ::getenv("HOME");
			if (pstr)
				return pstr;
#endif
			return fs::path();
		}

		std::string pretty_file_size(const std::string& path)
		{
			try {
				auto bytes = fs::file_size(path);
				const char * ustr[] = { " KB", " MB", " GB", " TB" };
				std::stringstream ss;
				if (bytes < 1024)
					ss << bytes << " Bytes";
				else
				{
					double cap = bytes / 1024.0;
					std::size_t uid = 0;
					while ((cap >= 1024.0) && (uid < sizeof(ustr) / sizeof(char *)))
					{
						cap /= 1024.0;
						++uid;
					}
					ss << cap;
					auto s = ss.str();
					auto pos = s.find('.');
					if (pos != s.npos)
					{
						if (pos + 2 < s.size())
							s.erase(pos + 2);
					}
					return s + ustr[uid];
				}

				return ss.str();
			}
			catch (...) {}
			return{};
		}

		std::string pretty_file_date(const std::string& path) // todo: move to .cpp
		{
			try {
				auto ftime = fs::last_write_time(path);

				// crash: VS2015 will not read the time for some files (for example: C:/hiberfil.sys)
				//   and will return file_time_type(-1) without throwing
				//   https://msdn.microsoft.com/en-us/library/dn823784.aspx

				if (ftime == ((fs::file_time_type::min)())) return{};

				//std::time_t cftime = decltype(ftime)::clock::to_time_t(ftime);
				
				//A workaround for VC2013
				using time_point = decltype(ftime);

				auto cftime = time_point::clock::to_time_t(ftime);

				//std::stringstream tm;
				//tm << std::put_time(std::localtime(&cftime), "%Y-%m-%d, %H:%M:%S");
				//return tm.str();

				char buffer[80] = { 0 };
				strftime(buffer, sizeof(buffer), "%Y-%m-%d, %H:%M:%S", std::localtime(&cftime));
				return static_cast<std::string>(buffer);
			}
			catch (...) {
				return{};
			}
		}

		bool set_modified_file_time(const std::string& path, const struct utimbuf *t)
		{
#if defined(STDXX_WINDOWS)
			HANDLE handle;
			FILETIME atime = { 0 }, mtime = { 0 };
			__int64 time64;
			int rc;

			handle = CreateFileA(path.c_str(), GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL, OPEN_EXISTING, 0, 0);

			if (handle == INVALID_HANDLE_VALUE)
				return false;

			if (t->actime != 0)
			{
				time64 = Int32x32To64(t->actime, 10000000) + 116444736000000000;
				atime.dwLowDateTime = (DWORD)time64;
				atime.dwHighDateTime = (DWORD)(time64 >> 32);
			}
			if (t->modtime != 0)
			{
				time64 = Int32x32To64(t->modtime, 10000000) + 116444736000000000;
				mtime.dwLowDateTime = (DWORD)time64;
				mtime.dwHighDateTime = (DWORD)(time64 >> 32);
			}

			rc = SetFileTime(handle, NULL, &atime, &mtime);
            ::CloseHandle(handle);
			return rc == TRUE ? true : false;
#elif defined(STDXX_POSIX)
			if (::utime(path.c_str(), (const struct utimbuf *)t) < 0)
			{
				return false;
			}
			return true;
#endif
		}
	}
}

#if _USING_STDXX_FILESYSTEM 

namespace stdxx_fs = stdxx::experimental::filesystem;

namespace stdxx {	namespace experimental {	namespace filesystem
	{
#ifndef CXX_NO_INLINE_NAMESPACE
			inline namespace v1 {
#endif

		//class filesystem_error
			filesystem_error::filesystem_error(const std::string& msg, std::error_code err)
				: std::system_error(err, msg)
			{}

			filesystem_error::filesystem_error(const std::string& msg, const path& path1, std::error_code err)
				:	std::system_error(err, msg),
					path1_(path1)
			{}

			filesystem_error::filesystem_error(const std::string& msg, const path& path1, const path& path2, std::error_code err)
				:	std::system_error(err, msg),
					path1_(path1),
					path2_(path2)
			{}

			const path& filesystem_error::path1() const noexcept
			{
				return path1_;
			}

			const path& filesystem_error::path2() const noexcept
			{
				return path2_;
			}
		//end class filesystem_error

#if defined(STDXX_WINDOWS)
			std::error_code make_system_error(uint32_t err)
			{
				return std::error_code(err ? static_cast<int>(err) : static_cast<int>(::GetLastError()), std::system_category());
			}
#else
			std::error_code make_system_error(int err)
			{
				return std::error_code(err ? err : errno, std::system_category());
			}
#endif


		//Because of No wide character version of POSIX
#if defined(STDXX_POSIX)
		const char* splstr = "/";
#else
		const wchar_t* splstr = L"/\\";
#endif

	//class file_status
		file_status::file_status(file_type ft, perms prms)
			: value_{ft}, perms_{prms}
		{}

		file_type file_status::type() const
		{
			return value_;
		}

		void file_status::type(file_type ft)
		{
			value_ = ft;
		}

		perms file_status::permissions() const
		{
			return perms_;
		}

		void file_status::permissions(perms prms)
		{
			perms_ = prms;
		}
	//end filestatus

	//class path
		int path::compare(const path& p) const
		{
			return pathstr_.compare(p.pathstr_);
		}

		/// true if the path is empty, false otherwise. ??
		bool path::empty() const noexcept
		{
#if defined(STDXX_WINDOWS)
			return (::GetFileAttributes(pathstr_.c_str()) == INVALID_FILE_ATTRIBUTES);
#elif defined(STDXX_LINUX) || defined(STDXX_MACOS)
			struct stat sta;
			return (::stat(pathstr_.c_str(), &sta) == -1);
#endif
		}

		path path::extension() const
		{
			// todo: make more globlal
#if defined(STDXX_WINDOWS)
            auto SLorP=L"\\/.";
			auto P=L'.';
#else
			auto SLorP="\\/.";
			auto P='.';
#endif
			auto pos = pathstr_.find_last_of(SLorP);

			if (    ( pos == pathstr_.npos)
				 || ( pathstr_[pos] != P )
				 || ( pos + 1 == pathstr_.size()  ))
			   return path();

			return path(pathstr_.substr(pos));
		}

		path path::parent_path() const
		{
			return{stdxx_fs::parent_path(pathstr_)};
		}

		file_type path::what() const
		{
#if defined(STDXX_WINDOWS)
			unsigned long attr = ::GetFileAttributes(pathstr_.c_str());
			if (INVALID_FILE_ATTRIBUTES == attr)
				return file_type::not_found; //??

			if (FILE_ATTRIBUTE_DIRECTORY & attr)
				return file_type::directory;

			return file_type::regular;
#elif defined(STDXX_LINUX) || defined(STDXX_MACOS)
			struct stat sta;
			if (-1 == ::stat(pathstr_.c_str(), &sta))
				return file_type::not_found; //??

			if ((S_IFDIR & sta.st_mode) == S_IFDIR)
				return file_type::directory;

			if ((S_IFREG & sta.st_mode) == S_IFREG)
				return file_type::regular;

			return file_type::none;
#endif
		}

		path path::filename() const
		{
			auto pos = pathstr_.find_last_of(splstr);
			if (pos != pathstr_.npos)
			{
				if (pos + 1 == pathstr_.size())
				{
					value_type tmp[2] = {preferred_separator, 0};

					if (pathstr_.npos != pathstr_.find_last_not_of(splstr, pos))
						tmp[0] = '.';

					return{ tmp };
				}
				return{ pathstr_.substr(pos + 1) };
			}

			return{ pathstr_ };
		}

		path& path::remove_filename()
		{
#if defined(STDXX_WINDOWS)
			wchar_t seps[] = L"/\\";
#else
			char seps[] = "/\\";
#endif
			auto pos = pathstr_.find_last_of(seps);
			if (pos != pathstr_.npos)
			{
				pos = pathstr_.find_last_not_of(seps, pos);
				if (pos != pathstr_.npos)
				{
#if defined(STDXX_WINDOWS)
					if (pathstr_[pos] == L':')
					{
						pathstr_.erase(pos + 1);
						pathstr_ += L'\\';

						return *this;
					}
#endif
					++pos;
				}
				else
					pos = 0;
			}
			else
				pos = 0;

			pathstr_.erase(pos);

			return *this;
		}

		const path::value_type* path::c_str() const
		{
			return native().c_str();
		}

		const path::string_type& path::native() const
		{
			return pathstr_;
		}
			
		path::operator string_type() const
		{
			return native();
		}

		std::string path::string() const
		{
#if defined(STDXX_WINDOWS)
            return stdxx::to_string(pathstr_);
#else
            return stdxx::u8_to_string(pathstr_);
#endif
		}

		std::wstring path::wstring() const
		{
			// convert utf8 to wstring
#if defined(STDXX_WINDOWS)
            return pathstr_;
#else
            return stdxx::u8_to_wstring(pathstr_);
#endif
		}

		std::string path::u8string() const
		{

#if defined(STDXX_WINDOWS)
			return stdxx::to_u8string(pathstr_);
#else
            return pathstr_;
#endif
		}

		std::string path::generic_string() const  
		{
#if defined(STDXX_WINDOWS)
			auto str = string();
			std::replace(str.begin(), str.end(), '\\', '/');
			return str;
#else
			return pathstr_;
#endif
		}
		std::wstring path::generic_wstring() const
		{
#if defined(STDXX_WINDOWS)
			auto str = wstring();
			std::replace(str.begin(), str.end(), L'\\', L'/');
			return str;
#else
			return wstring();
#endif
		}
		std::string path::generic_u8string() const   
		{
#if defined(STDXX_WINDOWS)
			auto str = pathstr_;
			std::replace(str.begin(), str.end(), '\\', '/');
			return stdxx::to_u8string(str);
#else
			return pathstr_;
#endif
		}
		path & path::operator/=(const path& p)
		{
			if (p.empty())
				return *this;

			if (this == &p)
			{
				auto other = p.pathstr_;
				if ((other.front() != '/') && (other.front() == path::preferred_separator))
				{
					if (!this->pathstr_.empty() && (pathstr_.back() != '/' && pathstr_.back() == path::preferred_separator))
						pathstr_.append(path::preferred_separator, 1);
				}

				pathstr_ += other;
			}
			else
			{
				auto & other = p.pathstr_;
				if ((other.front() != '/') && (other.front() == path::preferred_separator))
				{
					if (!this->pathstr_.empty() && (pathstr_.back() != '/' && pathstr_.back() == path::preferred_separator))
						pathstr_.append(path::preferred_separator, 1);
				}

				pathstr_ += p.pathstr_;
			}
			return *this;
		}

        void path::_m_assign(const std::string& source)
		{
#if defined(STDXX_WINDOWS)
            pathstr_ = stdxx::to_wstring(source);
#else
            pathstr_ = source;
#endif
		}

		void path::_m_assign(const std::wstring& source)
		{
#if defined(STDXX_WINDOWS)
			pathstr_ = source;
#else
			pathstr_ = stdxx::to_u8string(source);
#endif
		}
		//end class path

		bool operator==(const path& lhs, const path& rhs)
		{
			return (lhs.compare(rhs) == 0);
		}

		bool operator!=(const path& lhs, const path& rhs)
		{
			return (lhs.native() != rhs.native());
		}

		bool operator<(const path& lhs, const path& rhs)
		{
			return (lhs.compare(rhs) < 0);
		}

		bool operator>(const path& lhs, const path& rhs)
		{
			return (rhs.compare(lhs) < 0);
		}

		path operator/(const path& lhs, const path& rhs)
		{
			auto path = lhs;
			return (path /= rhs);
		}

		//class directory_entry
		directory_entry::directory_entry(const stdxx_fs::path& p)
			:path_{ p }
		{}

		//modifiers
		void directory_entry::assign(const  stdxx_fs::path& p)
		{
			path_ = p;
		}

		void directory_entry::replace_filename(const  stdxx_fs::path& p)
		{
			path_ = path_.parent_path() / p;
		}

		//observers
		file_status directory_entry::status() const
		{
			return stdxx_fs::status(path_);
		}

		//directory_entry::operator const nana_fs::path&() const
		//{
		//	return path_;
		//}

		const stdxx_fs::path& directory_entry::path() const
		{
			return path_;
		}
		//end class directory_entry


		//class directory_iterator
		struct inner_handle_deleter
		{
			void operator()(void** handle)
			{
#if defined(STDXX_WINDOWS)
				::FindClose(*handle);
#elif defined(STDXX_LINUX) || defined(STDXX_MACOS)
				::closedir(*reinterpret_cast<DIR**>(handle));
#endif
			}
		};

		directory_iterator::directory_iterator() noexcept
			:	end_(true),
			handle_(nullptr)
		{}

		directory_iterator::directory_iterator(const path& file_path)
		{
			_m_prepare(file_path);
		}

		const directory_iterator::value_type& directory_iterator::operator*() const { return value_; }

		const directory_iterator::value_type*
			directory_iterator::operator->() const { return &(operator*()); }

		directory_iterator& directory_iterator::operator++()
		{
			_m_read(); return *this;
		}

		directory_iterator directory_iterator::operator++(int)
		{
			directory_iterator tmp = *this;
			_m_read();
			return tmp;
		}

		bool directory_iterator::equal(const directory_iterator& x) const
		{
			if (end_ && (end_ == x.end_)) return true;
			return (value_.path().filename() == x.value_.path().filename());
		}


		void directory_iterator::_m_prepare(const path& file_path)
		{
			path_ = file_path.native();
#if defined(STDXX_WINDOWS)
			if (!path_.empty() && (path_.back() != L'/' && path_.back() != L'\\'))
				path_ += L'\\';

			auto pat = path_;
			DWORD attr = ::GetFileAttributes(pat.data());
			if ((attr != INVALID_FILE_ATTRIBUTES) && (attr & FILE_ATTRIBUTE_DIRECTORY))
				pat += L"*";

			WIN32_FIND_DATAW wfd;
			::HANDLE handle = ::FindFirstFile(pat.data(), &wfd);

			if (handle == INVALID_HANDLE_VALUE)
			{
				end_ = true;
				return;
			}

			while (_m_ignore(wfd.cFileName))
			{
				if (::FindNextFile(handle, &wfd) == 0)
				{
					end_ = true;
					::FindClose(handle);
					return;
				}
			}

			value_ = value_type(path(path_ + wfd.cFileName));

#elif defined(STDXX_POSIX)
			if (path_.size() && (path_.back() != '/'))
				path_ += '/';
			auto handle = opendir(path_.c_str());
			end_ = true;
			if (handle)
			{
				struct dirent * dnt = readdir(handle);
				if (dnt)
				{
					while (_m_ignore(dnt->d_name))
					{
						dnt = readdir(handle);
						if (dnt == 0)
						{
							closedir(handle);
							return;
						}
					}

					value_ = value_type(path_ + dnt->d_name);
					end_ = false;
				}
			}
#endif
			if (false == end_)
			{
				find_ptr_ = std::shared_ptr<find_handle>(new find_handle(handle), inner_handle_deleter());
				handle_ = handle;
			}
		}

		void directory_iterator::_m_read()
		{
			if (handle_)
			{
#if defined(STDXX_WINDOWS)
				WIN32_FIND_DATAW wfd;
				if (::FindNextFile(handle_, &wfd) != 0)
				{
					while (_m_ignore(wfd.cFileName))
					{
						if (::FindNextFile(handle_, &wfd) == 0)
						{
							end_ = true;
							return;
						}
					}
					value_ = value_type(path(path_ + wfd.cFileName));
				}
				else
					end_ = true;
#elif defined(STDXX_POSIX)
				struct dirent * dnt = readdir(reinterpret_cast<DIR*>(handle_));
				if (dnt)
				{
					while (_m_ignore(dnt->d_name))
					{
						dnt = readdir(reinterpret_cast<DIR*>(handle_));
						if (!dnt)
						{
							end_ = true;
							return;
						}
					}

					value_ = value_type(path(path_ + dnt->d_name));
				}
				else
					end_ = true;
#endif
			}
		}
		//end class directory_iterator

		bool not_found_error(int errval)
		{
#if defined(STDXX_WINDOWS)
			switch (errval)
			{
			case ERROR_FILE_NOT_FOUND:
			case ERROR_PATH_NOT_FOUND:
			case ERROR_INVALID_NAME:
			case ERROR_INVALID_DRIVE:
			case ERROR_NOT_READY:
			case ERROR_INVALID_PARAMETER:
			case ERROR_BAD_PATHNAME:
			case ERROR_BAD_NETPATH:
				return true;
			}
			return false;
#elif defined(STDXX_POSIX)
			return (errval == ENOENT || errval == ENOTDIR);
#else
			static_assert(false, "Only Windows and Unix are supported now (Mac OS is experimental)");
#endif
		}

		namespace detail
		{
			bool rm_file(const path& p)
			{
				if (p.empty())
					return false;
#if defined(STDXX_WINDOWS)	
				return (FALSE != ::DeleteFileW(p.c_str()));
#elif defined(STDXX_POSIX)
				return (!std::remove(p.c_str()));
#endif			
			}

			bool rm_dir(const path& p)
			{
#if defined(STDXX_WINDOWS)
				return (FALSE != ::RemoveDirectoryW(p.c_str())) || not_found_error(static_cast<int>(::GetLastError()));
#elif defined(STDXX_POSIX)
				return (!::rmdir(p.c_str())) || not_found_error(errno);
#endif			
			}

			bool rm_dir(const path& p, bool fails_if_not_empty);

			//rm_dir_recursive
			//@brief: remove a directory, if it is not empty, recursively remove it's subfiles and sub directories
			template<typename CharT>
			bool rm_dir_recursive(const CharT* dir)
			{
				std::vector<directory_iterator::value_type> files;
				std::basic_string<CharT> path = dir;
				path += '\\';

				std::copy(directory_iterator(dir), directory_iterator(), std::back_inserter(files));

				for (auto & f : files)
				{
					auto subpath = path + f.path().filename().native();

					if (is_directory(f))
						rm_dir_recursive(subpath.c_str());
					else
						rm_file(subpath.c_str());
				}
				return rm_dir(dir, true);
			}

			bool rm_dir(const path& p, bool fails_if_not_empty)
			{
				if (p.empty())
					return false;

#if defined(STDXX_WINDOWS)
				if (FALSE != ::RemoveDirectoryW(p.c_str()))
					return true;

				if (!fails_if_not_empty && (ERROR_DIR_NOT_EMPTY == ::GetLastError()))
					return detail::rm_dir_recursive(p.c_str());

				return false;
#elif defined(STDXX_POSIX)
				if (::rmdir(p.c_str()))
				{
					if (!fails_if_not_empty && (errno == EEXIST || errno == ENOTEMPTY))
						return detail::rm_dir_recursive(p.c_str());

					return false;
				}
				return true;
#endif
			}

#if defined(STDXX_WINDOWS)
			void filetime_to_c_tm(FILETIME& ft, struct tm& t)
			{
				FILETIME local_file_time;
				if (::FileTimeToLocalFileTime(&ft, &local_file_time))
				{
					SYSTEMTIME st;
					::FileTimeToSystemTime(&local_file_time, &st);
					t.tm_year = st.wYear - 1900;
					t.tm_mon = st.wMonth - 1;
					t.tm_mday = st.wDay;
					t.tm_wday = st.wDayOfWeek - 1;
					t.tm_yday = stdxx::date::day_in_year(st.wYear, st.wMonth, st.wDay);

					t.tm_hour = st.wHour;
					t.tm_min = st.wMinute;
					t.tm_sec = st.wSecond;
				}
			}
#endif
		}//end namespace detail


		file_status status(const path& p)
		{
			std::error_code err;
			auto stat = status(p, err);

			if (err != std::error_code())
				throw filesystem_error("stdxx::experimental::filesystem::status", p, err);

			return stat;
		}

		file_status status(const path& p, std::error_code& ec)
		{
			ec = std::error_code();
#if defined(STDXX_WINDOWS)
            auto attr = ::GetFileAttributesW(p.wstring().c_str());
			if (INVALID_FILE_ATTRIBUTES == attr)
			{
				if (not_found_error(static_cast<int>(::GetLastError())))
					return file_status{ file_type::not_found };
				return file_status{ file_type::unknown };
			}
			return file_status{ (FILE_ATTRIBUTE_DIRECTORY & attr) ? file_type::directory : file_type::regular, perms::all };
#elif defined(STDXX_POSIX)
			struct stat path_stat;
			if (0 != ::stat(p.c_str(), &path_stat))
			{
				if (errno == ENOENT || errno == ENOTDIR)
					return file_status{ file_type::not_found };

				return file_status{ file_type::unknown };
			}

			auto prms = static_cast<perms>(path_stat.st_mode & static_cast<unsigned>(perms::mask));

			if (S_ISREG(path_stat.st_mode))
				return file_status{ file_type::regular, prms };

			if (S_ISDIR(path_stat.st_mode))
				return file_status{ file_type::directory, prms };

			if (S_ISLNK(path_stat.st_mode))
				return file_status{ file_type::symlink, prms };

			if (S_ISBLK(path_stat.st_mode))
				return file_status{ file_type::block, prms };

			if (S_ISCHR(path_stat.st_mode))
				return file_status{ file_type::character, prms };

			if (S_ISFIFO(path_stat.st_mode))
				return file_status{ file_type::fifo, prms };

			if (S_ISSOCK(path_stat.st_mode))
				return file_status{ file_type::socket, prms };

			return file_status{ file_type::unknown };
#endif		
		}

		bool is_directory(const path& p)
		{
			return (status(p).type() == file_type::directory);
		}

		std::uintmax_t file_size(const path& p)
		{
#if defined(STDXX_WINDOWS)
			//Some compilation environment may fail to link to GetFileSizeEx
			typedef BOOL(__stdcall *GetFileSizeEx_fptr_t)(HANDLE, PLARGE_INTEGER);
			GetFileSizeEx_fptr_t get_file_size_ex = reinterpret_cast<GetFileSizeEx_fptr_t>(::GetProcAddress(::GetModuleHandleA("Kernel32.DLL"), "GetFileSizeEx"));
			if (get_file_size_ex)
			{
				HANDLE handle = ::CreateFile(p.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
				if (INVALID_HANDLE_VALUE != handle)
				{
					LARGE_INTEGER li;
					if (!get_file_size_ex(handle, &li))
						li.QuadPart = 0;

					::CloseHandle(handle);
					return li.QuadPart;
				}
			}
			return 0;
#elif defined(STDXX_POSIX)
			FILE * stream = ::fopen(p.c_str(), "rb");
			long long size = 0;
			if (stream)
			{
#if defined(STDXX_LINUX) || defined(STDXX_UNIX)
				fseeko64(stream, 0, SEEK_END);
				size = ftello64(stream);
#elif defined(STDXX_MACOS)
				fseeko(stream, 0, SEEK_END);
				size = ftello(stream);
#endif
				::fclose(stream);
			}
			return size;
#endif
		}


		file_time_type last_write_time(const path& p)
		{
			std::error_code ec;
			file_time_type t = last_write_time(p, ec);
			if (ec != std::error_code())
				throw filesystem_error("last_write_time(p): failed", p, ec);
			return t;
		}

		file_time_type last_write_time(const path & p, std::error_code & ec) noexcept
		{
			if (ec)
				ec.clear();

			struct tm t;
#if defined(STDXX_WINDOWS)
			WIN32_FILE_ATTRIBUTE_DATA attr;
			if (!::GetFileAttributesExW(p.generic_wstring().c_str(), GetFileExInfoStandard, &attr)) {
				ec = make_system_error();
			}
			FILETIME local_file_time;
			if (!::FileTimeToLocalFileTime(&attr.ftLastWriteTime, &local_file_time))
			{
				ec = make_system_error();
			}
			SYSTEMTIME st;
			::FileTimeToSystemTime(&local_file_time, &st);
			t.tm_year = st.wYear - 1900;
			t.tm_mon = st.wMonth - 1;
			t.tm_mday = st.wDay;
			t.tm_wday = st.wDayOfWeek - 1;
			t.tm_yday = stdxx::date::day_in_year(st.wYear, st.wMonth, st.wDay);

			t.tm_hour = st.wHour;
			t.tm_min = st.wMinute;
			t.tm_sec = st.wSecond;
#elif defined(STDXX_POSIX)
			struct stat attr;
			if (0 == ::stat(p.c_str(), &attr)) {
				t = *(::localtime((time_t*)&attr.st_mtime));
			}
			else {
				ec = make_system_error();
			}
#endif
			std::chrono::system_clock::time_point dateTime = std::chrono::system_clock::from_time_t(mktime(&t));
			return 	dateTime;
		}

		bool last_write_time(const path & p, file_time_type new_time)
		{
			std::error_code ec;
			bool result = last_write_time(p, new_time, ec);
			if (ec != std::error_code())
				throw filesystem_error("last_write_time(p): failed", p, ec);
			return result;
		}

		bool last_write_time(const path & p, file_time_type new_time, std::error_code & ec) noexcept
		{
			if (ec)
				ec.clear();

#if defined(STDXX_WINDOWS)
			HANDLE handle;
			FILETIME atime = { 0 }, mtime = { 0 };
			__int64 time64;
			handle = CreateFileW(p.generic_wstring().c_str(), GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL, OPEN_EXISTING, 0, 0);

			if (handle == INVALID_HANDLE_VALUE) {
				ec = make_system_error();
				return false;
			}

			time64 = Int32x32To64(new_time.time_since_epoch().count(), 10000000) + 116444736000000000;
			mtime.dwLowDateTime = (DWORD)time64;
			mtime.dwHighDateTime = (DWORD)(time64 >> 32);

			if (!::SetFileTime(handle, NULL, &atime, &mtime)) {
				ec = make_system_error();
				::CloseHandle(handle);
				return false;
			}
			::CloseHandle(handle);
#elif defined(STDXX_POSIX)
			::utimbuf buf;
			buf.actime = 0;
			buf.modtime = new_time.time_since_epoch().count();
            if (::utime(p.c_str(), (const struct utimbuf *)&buf) < 0) {
				ec = make_system_error();
				return false;
			}
#endif

			return true;
		}

		bool create_directories(const path& p)
		{
			// create directories chain
			std::error_code ec;
			bool result = create_directories(p, ec);
			if (ec != std::error_code())
				throw filesystem_error("create_directories(p): invalid argument", p, ec);
			return result;
		}

		bool create_directories(const path& p, std::error_code& ec) noexcept
		{
			// create directory chain
			ec = std::error_code();
			if (p.generic_string().empty())
				return false;
			else if (!exists(p))
			{	// recursively create parent directories, then current
				create_directories(p.parent_path(), ec);
				if (ec)
					return false;	// report error

				if(!create_directory(p))
					return false;	// report error

				return true;
			}
			else if (is_directory(p))
				return false;	// directory already there
			else
			{	// report bad argument
				ec = std::make_error_code(std::errc::operation_not_permitted);
				return false;
			}
		}

		bool create_directory(const path& p)
		{
			std::error_code ec;
			bool result = create_directory(p, ec);
			if (ec != std::error_code())
				throw filesystem_error("create_directory(p): failed", p, ec);
			return result;
		}

		bool create_directory(const path & p, std::error_code & ec) noexcept
		{
			if (ec)
				ec.clear();

#if defined(STDXX_WINDOWS)
			if (!::CreateDirectoryW(p.c_str(), 0)) {
#else
			if (0 != ::mkdir(p.c_str(), static_cast<int>(perms::all))) {
#endif
				ec = make_system_error();

				return false;
			}

			return true;
		}

		bool remove(const path& p)
		{
			auto stat = status(p);
			if (stat.type() == file_type::directory)
				return detail::rm_dir(p);

			return detail::rm_file(p);
		}

		bool remove(const path& p, std::error_code & ec) noexcept
		{
			ec.clear();
			auto stat = status(p);
			if (stat.type() == file_type::directory)
				return detail::rm_dir(p);

			return detail::rm_file(p);
		}

		bool remove_all(const path& p, std::uintmax_t& count, std::error_code& ec) noexcept
		{
			// recursively remove a file or directory, count removed files
			ec = std::error_code();

			std::vector<directory_iterator::value_type> files;
			std::copy(directory_iterator(p.string().c_str()), directory_iterator(), std::back_inserter(files));

			for (auto & it : files)
			{
				if (is_directory(it))
				{
					remove_all(it, count, ec);
				}
				else
				{ 
					if (remove(it))
					{
						++count;
					}
				}
			}

			return detail::rm_dir(p);
		}

		std::uintmax_t remove_all(const path& p)
		{
			std::error_code ec;
			std::uintmax_t count = remove_all(p, ec);
			if (ec != std::error_code())
				throw filesystem_error("remove_all(p): invalid argument", p, ec);
			return count;
		}

		std::uintmax_t remove_all(const path& p, std::error_code& ec) noexcept
		{
			std::uintmax_t count = 0;
			remove_all(p, count, ec);
			if (ec)
				return (static_cast<std::uintmax_t>(-1));
			return count;
		}

		void rename(const path & from, const path & to)
		{
			std::error_code ec;
			rename(from, to, ec);
			if (ec) throw filesystem_error("Could not rename file", from, to, ec);
		}

		void rename(const path & from, const path & to, std::error_code & ec) noexcept
		{
#if defined(STDXX_WINDOWS)
			if (::_wrename(from.c_str(), to.c_str()) != 0)
#elif defined(STDXX_POSIX)
			if (renameat(AT_FDCWD, from.c_str(), AT_FDCWD, to.c_str()) != 0)
#endif
				ec = std::make_error_code(std::errc::operation_not_permitted);
		}

		void resize_file(const path & p, std::uintmax_t size)
		{
			std::error_code ec;
			resize_file(p, size, ec);
			if (ec) throw filesystem_error("Could not trucate/resize file", p, ec);
		}

		void resize_file(const path & p, std::uintmax_t size, std::error_code & ec) noexcept
		{
			if (ec)
				ec.clear();

#if defined(STDXX_WINDOWS)
			HANDLE handle = ::CreateFile(p.c_str()
				, GENERIC_WRITE
				, FILE_SHARE_WRITE | FILE_SHARE_READ
				, 0
				, OPEN_EXISTING
				, FILE_ATTRIBUTE_NORMAL, 0);

			LARGE_INTEGER sz;
			sz.QuadPart = size;
			if (handle == INVALID_HANDLE_VALUE)
			{
				ec = std::make_error_code(std::errc::operation_not_permitted);
				return;
			}
			::SetFilePointerEx(handle, sz, 0, FILE_BEGIN);
			::SetEndOfFile(handle);
			::CloseHandle(handle);
#elif defined(STDXX_POSIX)
			if (truncate(p.c_str(), size) != 0)
				ec = std::make_error_code(std::errc::operation_not_permitted);
#endif
		}

		path current_path()
		{
#if defined(STDXX_WINDOWS)
			wchar_t buf[MAX_PATH];
			DWORD len = ::GetCurrentDirectoryW(MAX_PATH, buf);
			if (len)
			{
				if (len > MAX_PATH)
				{
					wchar_t * p = new wchar_t[len + 1];
					::GetCurrentDirectoryW(len + 1, p);
					std::wstring s = p;
					delete[] p;
					return s;
				}
				return buf;
			}
#elif defined(STDXX_POSIX)
			char buf[260];
			auto pstr = ::getcwd(buf, 260);
			if (pstr)
				return pstr;

			int bytes = 260 + 260;
			while (ERANGE == errno)
			{
				std::unique_ptr<char[]> buf(new char[bytes]);
				auto pstr = ::getcwd(buf.get(), bytes);
				if (pstr)
					return path(pstr);

				bytes += 260;
			}
#endif
			return path();
		}

		void current_path(const path& p)
		{
#if defined(STDXX_WINDOWS)
			::SetCurrentDirectoryW(p.c_str());
#elif defined(STDXX_POSIX)
			::chdir(p.c_str());
#endif
		}

#ifndef CXX_NO_INLINE_NAMESPACE
		} //end namespace v1
#endif
		}//end namespace filesystem
	} //end namespace experimental
}//end namespace stdxx
#endif

