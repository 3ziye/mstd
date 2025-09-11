#include "file.h"
#include "string_convert.h"

namespace mstd {

file::file() :
#ifdef MSTD_WINDOWS
	file_handle_(INVALID_HANDLE_VALUE)
#else
	file_handle_(-1)
#endif
{
}


file::~file()
{
	close();
}

std::string file::path() {
	return file_path_;
}

bool file::open(const std::string& path, FileMode mode) {
	file_path_ = path;
#if defined(MSTD_WINDOWS)
	DWORD access = 0;
	DWORD shared_mode = 0;
	DWORD creation_dispostion = 0;
	switch (mode)
	{
	case FileMode::ReadMode:
		access = GENERIC_READ;
		shared_mode = FILE_SHARE_READ;
		creation_dispostion = OPEN_EXISTING;
		break;
	case FileMode::WriteMode:
		access = GENERIC_WRITE | GENERIC_EXECUTE;
		creation_dispostion = OPEN_ALWAYS;
		break;
	case FileMode::ReadWrite:
		access = GENERIC_ALL; // GENERIC_READ | GENERIC_WRITE;
		shared_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;
		creation_dispostion = OPEN_ALWAYS;
		break;
	default:
		;
	}
	file_handle_ = ::CreateFileW(
		mstd::to_wstring(path).c_str(),
		access,
		shared_mode,
		nullptr,
		creation_dispostion,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);
	if (file_handle_ == INVALID_HANDLE_VALUE) {
		return false;
	}
#else
	int creation_flags;
	switch (mode)
	{
	case FileMode::ReadMode:
		creation_flags = O_RDONLY;
		break;
	case FileMode::WriteMode:
		creation_flags = O_WRONLY | O_CREAT;
		break;
	case FileMode::ReadWrite:
		creation_flags = O_RDWR | O_CREAT;
		break;
	default:
		;
	}

	file_handle_ = ::open(file_path_.c_str(), creation_flags, 0644);
	if (file_handle_ == -1)
		return false;
#endif
	return true;
}

std::size_t file::read(char* buffer, std::size_t size)
{
#ifdef MSTD_WINDOWS
	DWORD bytes_read = 0;
	::ReadFile(file_handle_, buffer, size, &bytes_read, nullptr);
	return static_cast<std::size_t>(bytes_read);
#else
	return ::read(file_handle_, buffer, size);
#endif
}

std::size_t file::write(const char* buffer, std::size_t size)
{
#ifdef MSTD_WINDOWS
	DWORD bytes_written = 0;
	::WriteFile(file_handle_, buffer, size, &bytes_written, nullptr);
	return static_cast<std::size_t>(bytes_written);
#else
	return ::write(file_handle_, buffer, size);
#endif
}

uint64_t file::seek(uint64_t size, FileSeek seek)
{
#ifdef MSTD_WINDOWS
    LARGE_INTEGER length;
    length.QuadPart = size;
    LARGE_INTEGER new_offset;
    if (!::SetFilePointerEx(file_handle_, length, &new_offset, seek)) {
        return -1;
    }

    return new_offset.QuadPart;
#else
	uint64_t fpos = ::lseek(file_handle_, size, seek);
	return static_cast<uint64_t>(fpos);
#endif
}


uint64_t file::length()
{
#ifdef MSTD_WINDOWS
	//Some compilation environment may fail to link to GetFileSizeEx
	typedef BOOL(__stdcall *GetFileSizeEx_Fun)(HANDLE, PLARGE_INTEGER);
	GetFileSizeEx_Fun get_file_size_ex = reinterpret_cast<GetFileSizeEx_Fun>(::GetProcAddress(::GetModuleHandleW(L"Kernel32.DLL"), "GetFileSizeEx"));
	if (get_file_size_ex)
	{
		if (INVALID_HANDLE_VALUE != file_handle_)
		{
			LARGE_INTEGER li;
			if (!get_file_size_ex(file_handle_, &li))
				li.QuadPart = 0;

			return li.QuadPart;
		}
	}
#else
	struct stat stbuf;
	if ((fstat(file_handle_, &stbuf) == 0) && (S_ISREG(stbuf.st_mode))) 
	{
		return stbuf.st_size;
	}

#endif
    return 0;
}

bool file::length(uint64_t size)
{
#ifdef MSTD_WINDOWS
    LARGE_INTEGER length;
    length.QuadPart = size;
    if (!::SetFilePointerEx(file_handle_, length, NULL, FILE_BEGIN)) {
        return false;
    }

    if (!::SetEndOfFile(file_handle_))
    {
        return false;
    }
#else
	if (::ftruncate(file_handle_, size) != 0)
	{
		return false;
	}
#endif
	return true;
}

bool file::last_modify_time(time_t tm) {
#ifdef MSTD_WINDOWS
	FILETIME mtime = { 0 };
	__int64 time64;
	int rc;

	if (tm != 0)
	{
		time64 = Int32x32To64(tm, 10000000) + 116444736000000000;
		mtime.dwLowDateTime = (DWORD)time64;
		mtime.dwHighDateTime = (DWORD)(time64 >> 32);
	}

	rc = SetFileTime(file_handle_, NULL, NULL, &mtime);
	return rc == TRUE ? true : false;
#else
	struct utimbuf timebuf;
	timebuf.actime = 0;
	timebuf.modtime = tm;
	if (::utime(file_path_.c_str(), &timebuf) < 0) {
		return false;
	}
	return true;
#endif
}

time_t file::last_modify_time()
{
#ifdef MSTD_WINDOWS
	FILETIME creation_time, last_access_time, last_write_time;
	if (::GetFileTime(file_handle_, &creation_time, &last_access_time, &last_write_time))
	{
		ULARGE_INTEGER ull;
		ull.LowPart = last_write_time.dwLowDateTime;
		ull.HighPart = last_write_time.dwHighDateTime;
		return ull.QuadPart / 10000000ULL - 11644473600ULL;
	}
#else
	struct stat stbuf;
	if ((fstat(file_handle_, &stbuf) == 0) && (S_ISREG(stbuf.st_mode)))
	{
		return stbuf.st_mtime;
	}
#endif
    return 0;
}


void file::close()
{
#ifdef MSTD_WINDOWS
    if (file_handle_ != INVALID_HANDLE_VALUE)
		::CloseHandle(file_handle_);

    file_handle_ = INVALID_HANDLE_VALUE;
#else
    if (file_handle_ != -1)
        ::close(file_handle_);

    file_handle_ = -1;
#endif
}

}
