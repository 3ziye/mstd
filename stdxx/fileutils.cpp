
#include "fileutils.h"
#include "filesystem_ext.h"
#include "string_convert.h"
#include "fstream.h"
#ifdef WIN32
#include <Windows.h>
#else
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#ifndef __ANDROID__
#include <sys/statvfs.h>
#endif
#endif // WIN32

namespace stdxx
{

	template<typename charT>
	bool _is_exists_file(const charT* path)
	{
		namespace fs = stdxx::experimental::filesystem;

		if (!fs::is_regular_file(path))
			return false;

		return true;
	}

    //文件是否存在
    bool is_exists_file(const char* path)
    {
        return _is_exists_file(path);
    }

    bool is_exists_file(const wchar_t* path)
    {
		return _is_exists_file(path);
    }

    bool is_exists_file(const std::string& path)
    {
		return is_exists_file(path.c_str());
    }

    bool is_exists_file(const std::wstring& path)
    {
		return is_exists_file(path.c_str());
    }

    //读文件
    template<typename charT>
    bool _read_file(const charT* filename, std::string& data)
    {
        stdxx::basic_ifstream<char> file(filename, std::ios_base::binary | std::ios_base::in);
        if (!file.good())
        {
            return false;
        }

        data.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        return true;
    }

    template<typename charT>
    bool _read_file(const charT* filename, std::vector<char>& data)
    {
        stdxx::basic_ifstream<char> file(filename, std::ios_base::binary | std::ios_base::in);
        if (!file.good())
        {
            return false;
        }

        data.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        return true;
    }

    bool read_file(const char* filename, std::string& data)
    {
        return _read_file(filename, data);
    }

    bool read_file(const std::string &filename, std::string& data)
    {
        return _read_file(filename.c_str(), data);
    }

    bool read_file(const char* filename, std::vector<char>& data)
    {
        return _read_file(filename, data);
    }

    bool read_file(const std::string &filename, std::vector<char>& data)
    {
        return _read_file(filename.c_str(), data);
    }

#if defined(STDXX_WINDOWS)
    bool read_file(const wchar_t* filename, std::string& data)
    {
        return _read_file(filename, data);
    }

    bool read_file(const std::wstring &filename, std::string& data)
    {
        return _read_file(filename.c_str(), data);
    }

    bool read_file(const wchar_t* filename, std::vector<char>& data)
    {
        return _read_file(filename, data);
    }

    bool read_file(const std::wstring &filename, std::vector<char>& data)
    {
        return _read_file(filename.c_str(), data);
    }
#endif
    //写入文件
    bool _write_file(const char* filename, const char* data, int size)
    {
		stdxx::basic_ofstream<char> file(filename, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
        if (!file.good())
        {
            return false;
        }

        if (data != nullptr || size != 0)
        {
            file.write(data, size);
            return !file.bad();
        }
        return false;
    }

    bool write_file(const char* filename, const char* data, int size)
    {
        return _write_file(filename, data, size);
    }

    bool write_file(const wchar_t* filename, const char* data, int size)
    {
        return _write_file(stdxx::to_string(filename).c_str(), data, size);
    }

    bool write_file(const std::string &filename, const char* data, int size)
    {
        return _write_file(filename.c_str(), data, size);
    }

    bool write_file(const std::wstring &filename, const char* data, int size)
    {
        return _write_file(stdxx::to_string(filename).c_str(), data, size);
    }

	//获取文件大小
	template<typename charT>
	int64_t _get_file_size(const charT* filename)
	{
		namespace fs = stdxx::experimental::filesystem;
		if (fs::is_regular_file(filename))
		{
			try
			{
				return fs::file_size(filename);
			}
			catch (fs::filesystem_error &ex)
			{
				ex;
				return -1;
			}
		}

		return -1;
	}


    int64_t get_file_size(const char* filename)
    {
		return _get_file_size(filename);
    }

    int64_t get_file_size(const wchar_t* filename)
    {
		return _get_file_size(filename);
    }

    int64_t get_file_size(const std::string &filename)
    {
        return get_file_size(filename.c_str());
    }

    int64_t get_file_size(const std::wstring &filename)
    {
        return get_file_size(filename.c_str());
    }

    //获取文件大小
    template<typename charT>
    bool _get_file_stat(const charT* filename, fstat *stat)
    {
		if (!stat) return false;

		namespace fs = stdxx::experimental::filesystem;

		if (!fs::is_regular_file(filename))
			return false;

		stat->st_time = std::chrono::system_clock::to_time_t(fs::last_write_time(filename));
		stat->st_size = fs::file_size(filename);

		return true;
    }

    bool get_file_stat(const char* filename, fstat *stat)
    {
        return _get_file_stat(filename, stat);
    }

    bool get_file_stat(const wchar_t* filename, fstat *stat)
    {
        return _get_file_stat(filename, stat);
    }

    bool get_file_stat(const std::string & filename, fstat *stat)
    {
        return _get_file_stat(filename.c_str(), stat);
    }

    bool get_file_stat(const std::wstring & filename, fstat *stat)
    {
        return _get_file_stat(filename.c_str(), stat);
    }


	template<typename charT>
	bool _set_file_size(const charT* filename, uint64_t size)
	{
		namespace fs = stdxx::experimental::filesystem;
		if (fs::is_regular_file(filename))
		{
			try {
				fs::resize_file(filename, size);
				return true;
			}
			catch (fs::filesystem_error &ex)
			{
				ex;
				return false;
			}
		}

		return false;
	}

    bool set_file_size(const char *filename, uint64_t size)
    {
		return _set_file_size(filename, size);
    }

	bool set_file_size(const wchar_t *filename, uint64_t size)
    {
		return _set_file_size(filename, size);
    }

    bool set_file_size(const std::string &filename, uint64_t size)
    {
		return set_file_size(filename.c_str(), size);
    }

    bool set_file_size(const std::wstring &filename, uint64_t size)
    {
		return set_file_size(filename.c_str(), size);
    }

	template<typename charT>
	bool _set_file_modify_time(const charT *filename, time_t mtime)
	{
        namespace fs = stdxx::experimental::filesystem;
        return fs::last_write_time(filename, std::chrono::system_clock::from_time_t(mtime));
	}

	bool set_file_modify_time(const char * filename, time_t mtime)
	{
		return _set_file_modify_time(filename, mtime);
	}

	bool set_file_modify_time(const wchar_t *filename, time_t mtime)
	{
		return _set_file_modify_time(filename, mtime);
	}

	bool set_file_modify_time(const std::string &filename, time_t mtime)
	{
		return set_file_modify_time(filename.c_str(), mtime);
	}

	bool set_file_modify_time(const std::wstring &filename, time_t mtime)
	{
		return set_file_modify_time(filename.c_str(), mtime);
	}

	bool delete_file(const char* filename)
    {
		return stdxx::experimental::filesystem::remove(filename);
    }

    bool delete_file(const wchar_t* filename)
    {
		return stdxx::experimental::filesystem::remove(filename);
    }

    bool delete_file(const std::string &filename)
    {
        return delete_file(filename.c_str());
    }

    bool delete_file(const std::wstring &filename)
    {
		return delete_file(filename.c_str());
    }

	template<typename charT>
	bool _rename_file(const charT* from, const charT* to)
	{
		namespace fs = stdxx::experimental::filesystem;
		if (fs::is_regular_file(from))
		{
			try {
				fs::rename(from, to);
				return true;
			}
			catch (fs::filesystem_error &ex)
			{
				ex;
				return false;
			}
		}

		return false;
	}

    //文件改名
    bool rename_file(const char *old_filename, const char *new_filename)
    {
		return _rename_file(old_filename, new_filename);
    }

    bool rename_file(const wchar_t *old_filename, const wchar_t *new_filename)
    {
		return _rename_file(old_filename, new_filename);
    }

    bool rename_file(const std::string &old_filename, const std::string &new_filename)
    {
		return rename_file(old_filename.c_str(), new_filename.c_str());
    }

    bool rename_file(const std::wstring &old_filename, const std::wstring &new_filename)
    {
		return rename_file(old_filename.c_str(), new_filename.c_str());
    }

    //文件移动 
    bool move_file(const char *old_path, const char *new_path) {
#if defined(STDXX_WINDOWS)
        return move_file(std::string(old_path), std::string(new_path));
#elif defined(STDXX_POSIX)
        if (rename(old_path, new_path) == 0) return true;
        return false;
#endif
    }

    bool move_file(const wchar_t *old_path, const wchar_t *new_path) {
#if defined(STDXX_WINDOWS)
        if (FALSE == ::MoveFileExW(old_path, new_path, MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED)) return false;
        return true;
#elif defined(STDXX_POSIX)
        return move_file(std::wstring(old_path), std::wstring(new_path));
#endif
        return true;
    }

    bool move_file(const std::string &old_path, const std::string &new_path) {
#if defined(STDXX_WINDOWS)
        return move_file(stdxx::to_wstring(old_path).c_str(), stdxx::to_wstring(new_path).c_str());
#elif defined(STDXX_POSIX)
        return move_file(old_path.c_str(), new_path.c_str());
#endif
    }

    bool move_file(const std::wstring &old_path, const std::wstring &new_path) {
#if defined(STDXX_WINDOWS)
        return move_file(old_path.c_str(), new_path.c_str());
#elif defined(STDXX_POSIX)
        return move_file(stdxx::to_string(old_path).c_str(), stdxx::to_string(new_path).c_str());
#endif
    }

#ifdef STDXX_POSIX
    int create_copy_file(const char *source, const char *target)
    {
        struct stat src_stat;
        if (lstat(source, &src_stat) == -1)
            return -1;

        if (S_ISLNK(src_stat.st_mode)) {
            char lnk[1024];
            int lnk_size;
            if ((lnk_size = readlink(source, lnk, 1023)) == -1)
                return -2;
            lnk[lnk_size] = '\0';
            if (symlink(lnk, target) == -1)
                return -3;
        }
        else if (S_ISREG(src_stat.st_mode)) {
            int src, dst;
            int rsize;
            char buf[1024];
            if ((src = open(source, O_RDONLY)) == -1) {
                close(dst);
                return -4;
            }

            if ((dst = creat(target, src_stat.st_mode)) == -1)
                return -5;

            while ((rsize = read(src, buf, 1024))) {
                if (rsize == -1 && errno == EINTR)
                    continue;

                if (rsize == -1) {
                    close(src);
                    close(dst);
                    return -6;
                }

                while (write(dst, buf, rsize) == -1)
                    if (errno != EINTR) {
                        close(src);
                        close(dst);
                        return -7;
                    }
            }
            close(src);
            close(dst);
        }
        else {
            return -8;
        }
        return 0;
    }
#endif

    //文件拷贝
    bool copy_file(const char *src_path, const char *dest_path) {
#if defined(STDXX_WINDOWS)
        if (FALSE == ::CopyFileA(src_path, dest_path, FALSE)) return false;
        return true;
#elif defined(STDXX_POSIX)
        return 0 == create_copy_file(src_path, dest_path);
#endif
    }

    bool copy_file(const wchar_t *src_path, const wchar_t *dest_path) {
#if defined(STDXX_WINDOWS)
        if (FALSE == ::CopyFileW(src_path, dest_path, FALSE)) return false;
        return true;
#elif defined(STDXX_POSIX)
        return 0 == create_copy_file(stdxx::to_string(std::wstring(src_path)).c_str(), stdxx::to_string(std::wstring(dest_path)).c_str());
#endif
    }

    bool copy_file(const std::string &old_path, const std::string &new_path) {
#if defined(STDXX_WINDOWS)
        return copy_file(old_path.c_str(), new_path.c_str());
#elif defined(STDXX_POSIX)
        return 0 == create_copy_file(old_path.c_str(), new_path.c_str());
#endif
    }

    bool copy_file(const std::wstring &old_path, const std::wstring &new_path) {
#if defined(STDXX_WINDOWS)
        return copy_file(old_path.c_str(), new_path.c_str());
#elif defined(STDXX_POSIX)
        return 0 == create_copy_file(stdxx::to_string(old_path).c_str(), stdxx::to_string(new_path).c_str());
#endif
    }

    ///////////////////////////////目录操作////////////////////////////////////////////////////////////////

    bool delete_directory(const char* path)
    {
		return stdxx::experimental::filesystem::remove(path);
    }

    bool delete_directory(const wchar_t* path)
    {
		return stdxx::experimental::filesystem::remove(path);
    }

    bool is_exists_directory(const char* path)
    {
		return stdxx::experimental::filesystem::is_directory(path);
    }

    bool is_exists_directory(const wchar_t* path)
    {
		return stdxx::experimental::filesystem::is_directory(path);
    }

    bool is_exists_directory(const std::string& path)
    {
        return is_exists_directory(path.c_str());
    }

    bool is_exists_directory(const std::wstring& path)
    {
        return is_exists_directory(path.c_str());
    }


	template<typename charT>
	bool _create_directorys(const charT* path)
	{
		namespace fs = stdxx::experimental::filesystem;
		if (!fs::is_directory(path))
		{
			try {
				return fs::create_directories(path);
			}
			catch (fs::filesystem_error &ex)
			{
				ex;
				return false;
			}
		}

		return false;
	}

    bool create_directorys(const char* path)
    {
		return _create_directorys(path);
    }

    bool create_directorys(const wchar_t* path)
    {
		return _create_directorys(path);
    }

    bool create_directorys(const std::string& path)
    {
        return create_directorys(path.c_str());
    }

    bool create_directorys(const std::wstring& path)
    {
        return create_directorys(path.c_str());
    }

	template<typename charT>
	bool _delete_directorys(const charT* path)
	{
		namespace fs = stdxx::experimental::filesystem;
		if (fs::is_directory(path))
		{
			try {
				fs::remove_all(path);
				return true;
			}
			catch (fs::filesystem_error &ex)
			{
				ex;
				return false;
			}
		}

		return false;
	}

	bool delete_directorys(const char* path)
	{
		return _delete_directorys(path);
	}

	bool delete_directorys(const wchar_t* path)
	{
		return _delete_directorys(path);
	}

	bool delete_directorys(const std::string& path)
	{
		return delete_directorys(path.c_str());
	}

	bool delete_directorys(const std::wstring& path)
	{
		return delete_directorys(path.c_str());
	}

    //目录不存在则创建
    template<typename charT>
    bool _verify_directory(const charT* path)
    {
        if (is_exists_directory(path))
        {
            return true;
        }

        return _create_directorys(path);
    }


    bool verify_directory(const char* path)
    {
        return _verify_directory(path);
    }

    bool verify_directory(const wchar_t* path)
    {
        return _verify_directory(path);
    }

    bool verify_directory(const std::string& path)
    {
        return _verify_directory(path.c_str());
    }

    bool verify_directory(const std::wstring& path)
    {
        return _verify_directory(path.c_str());
    }

    //
    bool enum_directory(const char* dir, std::vector<std::string> &dirs, std::vector<std::string> &files, const uint32_t level)
    {
        if (0 == level) return true;

        namespace fs = stdxx::experimental::filesystem;
        fs::path path(dir);
        if (!fs::exists(path)) return false;

        fs::directory_iterator end_iter;
        for (fs::directory_iterator iter(path); iter != end_iter; ++iter) {
            if (fs::is_regular_file(iter->status())) {
#ifdef WIN32
                files.push_back(iter->path().string());
#else 
                files.push_back(iter->path().c_str());
#endif
            }

            if (fs::is_directory(iter->status())) {
#ifdef WIN32
                dirs.push_back(iter->path().string());
                enum_directory(iter->path().string().c_str(), dirs, files, level - 1);
#else 
                dirs.push_back(iter->path().c_str());
                enum_directory(iter->path().c_str(), dirs, files, level - 1);
#endif
            }
        }

        return true;
    }

    bool enum_directory(const wchar_t* dir, std::vector<std::wstring> &dirs, std::vector<std::wstring> &files, const uint32_t level)
    {
        if (0 == level) return true;

        namespace fs = stdxx::experimental::filesystem;
        fs::path path(dir);
        if (!fs::exists(path)) return false;

        fs::directory_iterator end_iter;
        for (fs::directory_iterator iter(path); iter != end_iter; ++iter) {
            if (fs::is_regular_file(iter->status())) {
                files.push_back(iter->path().wstring());
            }

            if (fs::is_directory(iter->status())) {
                dirs.push_back(iter->path().wstring());
                enum_directory(iter->path().wstring().c_str(), dirs, files, level - 1);
            }
        }

        return true;
    }

    bool enum_directory(const std::string& dir, std::vector<std::string> &dirs, std::vector<std::string> &files, uint32_t level)
    {
        return enum_directory(dir.c_str(), dirs, files, level);
    }

    bool enum_directory(const std::wstring& dir, std::vector<std::wstring> &dirs, std::vector<std::wstring> &files, uint32_t level)
    {
        return enum_directory(dir.c_str(), dirs, files, level);
    }


    template<typename charT>
    bool _rename_directory(const charT* from, const charT* to){
        namespace fs = stdxx::experimental::filesystem;
        if (stdxx::experimental::filesystem::is_directory(from)){
            try {
                fs::rename(from, to);
                return true;
            }
            catch (fs::filesystem_error &ex)
            {
                ex;
                return false;
            }
        }

        return false;
    }

    //目录改名
    bool rename_directory(const char *old_filename, const char *new_filename) {
        return _rename_directory(old_filename, new_filename);
    }

    bool rename_directory(const wchar_t *old_filename, const wchar_t *new_filename) {
        return _rename_directory(old_filename, new_filename);
    }

    bool rename_directory(const std::string &old_filename, const std::string &new_filename) {
        return _rename_directory(old_filename.c_str(), new_filename.c_str());
    }

    bool rename_directory(const std::wstring &old_filename, const std::wstring &new_filename) {
        return _rename_directory(old_filename.c_str(), new_filename.c_str());
    }

#if defined(STDXX_WINDOWS)
    BOOL MoveFolder(const wchar_t * lpszFromPath, const wchar_t * lpszToPath)
    {
        int nLengthFrm = int(wcslen(lpszFromPath));
        wchar_t *NewPathFrm = new TCHAR[nLengthFrm + 2];
        wcscpy(NewPathFrm, lpszFromPath);
        NewPathFrm[nLengthFrm] = L'\0';
        NewPathFrm[nLengthFrm + 1] = L'\0';

        SHFILEOPSTRUCT FileOp;
        ZeroMemory((void*)&FileOp, sizeof(SHFILEOPSTRUCT));

        FileOp.fFlags = FOF_NOCONFIRMATION;
        FileOp.hNameMappings = NULL;
        FileOp.hwnd = NULL;
        FileOp.lpszProgressTitle = NULL;
        FileOp.pFrom = NewPathFrm;
        FileOp.pTo = lpszToPath;
        FileOp.wFunc = FO_MOVE;

        BOOL bRet = SHFileOperationW(&FileOp) == 0;
        delete[]NewPathFrm;
        return bRet;
    }
#endif

    //目录移动
    bool move_directory(const char *old_path, const char *new_path) {
		bool result = false;
#if defined(STDXX_WINDOWS)
        result = MoveFolder(stdxx::u8_to_wstring(old_path).c_str(), stdxx::u8_to_wstring(new_path).c_str()) ? true : false;
#elif defined(STDXX_POSIX)
		result = (rename(old_path, new_path) == 0) ? true : false;
#endif
		return result;
    }

    bool move_directory(const wchar_t *old_path, const wchar_t *new_path) {
#if defined(STDXX_WINDOWS)
        if (FALSE == MoveFolder(old_path, new_path)) return false;
        return true;
#elif defined(STDXX_POSIX)
        std::string src = stdxx::to_string(std::wstring(old_path));
        std::string dst = stdxx::to_string(std::wstring(new_path));
        return move_directory(src.c_str(), dst.c_str());
        //return move_directory(stdxx::to_string(std::wstring(old_path)).c_str(), std::to_string(std::wstring(new_path)).c_str());
#endif
    }

    bool move_directory(const std::string &old_path, const std::string &new_path) {
        return move_directory(old_path.c_str(), new_path.c_str());
    }

    bool move_directory(const std::wstring &old_path, const std::wstring &new_path) {
        return move_directory(old_path.c_str(), new_path.c_str());
    }

    ///////////////////////////////////////////////////////////////////////////////////////
}
