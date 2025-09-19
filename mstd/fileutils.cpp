
#include "fileutils.h"
#include <filesystem>
#include "string_convert.h"
#ifdef MSTD_WINDOWS
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
#endif

namespace mstd
{
	template<typename charT>
	bool _is_exists_file(const charT* path) {
		if (!std::filesystem::is_regular_file(path))
			return false;

		return true;
	}

    //文件是否存在
    bool is_exists_file(const char* path)
    {
        return _is_exists_file(path);
    }

	bool is_exists_file(const std::string& path)
	{
		return is_exists_file(path.c_str());
	}

    bool is_exists_file(const wchar_t* path)
    {
		return _is_exists_file(path);
    }

    bool is_exists_file(const std::wstring& path)
    {
		return is_exists_file(path.c_str());
    }

    //读文件
    template<typename charT>
    bool _read_file(const charT* filename, std::string& data) {
        std::basic_ifstream<char> file(filename, std::ios_base::binary | std::ios_base::in);
        if (!file.good())
        {
            return false;
        }

        data.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        return true;
    }

    template<typename charT>
    bool _read_file(const charT* filename, std::vector<char>& data) {
        std::basic_ifstream<char> file(filename, std::ios_base::binary | std::ios_base::in);
        if (!file.good())
        {
            return false;
        }

        data.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        return true;
    }

    bool read_file(const char* filename, std::string& data) {
        return _read_file(filename, data);
    }

    bool read_file(const std::string &filename, std::string& data) {
        return _read_file(filename.c_str(), data);
    }

#ifdef MSTD_WINDOWS
    bool read_file(const wchar_t* filename, std::string& data)
    {
        return _read_file(filename, data);
    }

    bool read_file(const std::wstring &filename, std::string& data)
    {
        return _read_file(filename.c_str(), data);
    }
#endif

	bool read_file(const char* filename, std::vector<char>& data) {
		return _read_file(filename, data);
	}

	bool read_file(const std::string& filename, std::vector<char>& data) {
		return _read_file(filename.c_str(), data);
	}

#ifdef MSTD_WINDOWS
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
    bool _write_file(const char* filename, const char* data, int size) {
		std::basic_ofstream<char> file(filename, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
        if (!file.good()) {
            return false;
        }

        if (data != nullptr || size != 0) {
            file.write(data, size);
            return !file.bad();
        }
        return false;
    }

    bool write_file(const char* filename, const char* data, int size) {
        return _write_file(filename, data, size);
    }

	bool write_file(const std::string& filename, const char* data, int size) {
		return _write_file(filename.c_str(), data, size);
	}

#ifdef MSTD_WINDOWS
    bool write_file(const wchar_t* filename, const char* data, int size) {
        return _write_file(mstd::to_string(filename).c_str(), data, size);
    }

    bool write_file(const std::wstring &filename, const char* data, int size) {
        return _write_file(mstd::to_string(filename).c_str(), data, size);
    }
#endif

	//获取文件大小
	template<typename charT>
	int64_t _get_file_size(const charT* filename) {
		if (std::filesystem::is_regular_file(filename)) {
			try {
				return std::filesystem::file_size(filename);
			}
			catch (std::filesystem::filesystem_error &ex) {
				ex;
				return -1;
			}
		}

		return -1;
	}

    int64_t get_file_size(const char* filename) {
		return _get_file_size(filename);
    }

    int64_t get_file_size(const std::string &filename) {
        return get_file_size(filename.c_str());
    }

#ifdef MSTD_WINDOWS
    int64_t get_file_size(const wchar_t* filename) {
		return _get_file_size(filename);
    }

    int64_t get_file_size(const std::wstring &filename) {
        return get_file_size(filename.c_str());
    }
#endif

    //获取文件大小
    template<typename charT>
    bool _get_file_stat(const charT* filename, fstat *stat) {
		if (!stat) return false;

		if (!std::filesystem::is_regular_file(filename))
			return false;

		auto sys_time = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            std::filesystem::last_write_time(filename) - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
			);

		stat->st_time = std::chrono::system_clock::to_time_t(sys_time);
		stat->st_size = std::filesystem::file_size(filename);
		return true;
    }

    bool get_file_stat(const char* filename, fstat *stat) {
        return _get_file_stat(filename, stat);
    }

	bool get_file_stat(const std::string& filename, fstat* stat) {
		return _get_file_stat(filename.c_str(), stat);
	}

#ifdef MSTD_WINDOWS
    bool get_file_stat(const wchar_t* filename, fstat *stat) {
        return _get_file_stat(filename, stat);
    }

    bool get_file_stat(const std::wstring & filename, fstat *stat) {
        return _get_file_stat(filename.c_str(), stat);
    }
#endif

	template<typename charT>
	bool _set_file_size(const charT* filename, uint64_t size) {
		if (std::filesystem::is_regular_file(filename)) {
			try {
                std::filesystem::resize_file(filename, size);
				return true;
			} catch (std::filesystem::filesystem_error &ex) {
				ex;
				return false;
			}
		}

		return false;
	}

    bool set_file_size(const char *filename, uint64_t size) {
		return _set_file_size(filename, size);
    }

	bool set_file_size(const std::string& filename, uint64_t size) {
		return set_file_size(filename.c_str(), size);
	}

#ifdef MSTD_WINDOWS
	bool set_file_size(const wchar_t *filename, uint64_t size) {
		return _set_file_size(filename, size);
    }

    bool set_file_size(const std::wstring &filename, uint64_t size) {
		return set_file_size(filename.c_str(), size);
    }
#endif

	template<typename charT>
	bool _set_file_modify_time(const charT *filename, time_t mtime) {
        namespace fs = std::filesystem;
		auto system_time = std::chrono::system_clock::from_time_t(mtime);
		// 转换为 file_time_type（需要 duration 转换）
		auto file_time = fs::file_time_type(
			std::chrono::duration_cast<fs::file_time_type::duration>(system_time.time_since_epoch())
		);

		try {
            std::filesystem::last_write_time(filename, file_time);
			return true;
		} catch (std::filesystem::filesystem_error& ex) {
			ex;
		}

        return false;
	}

	bool set_file_modify_time(const char * filename, time_t mtime) {
		return _set_file_modify_time(filename, mtime);
	}

	bool set_file_modify_time(const std::string& filename, time_t mtime) {
		return set_file_modify_time(filename.c_str(), mtime);
	}

#ifdef MSTD_WINDOWS
	bool set_file_modify_time(const wchar_t *filename, time_t mtime) {
		return _set_file_modify_time(filename, mtime);
	}

	bool set_file_modify_time(const std::wstring &filename, time_t mtime) {
		return set_file_modify_time(filename.c_str(), mtime);
	}
#endif

	bool delete_file(const char* filename) {
		return std::filesystem::remove(filename);
    }

	bool delete_file(const std::string& filename) {
		return delete_file(filename.c_str());
	}

#ifdef MSTD_WINDOWS
    bool delete_file(const wchar_t* filename) {
		return std::filesystem::remove(filename);
    }

    bool delete_file(const std::wstring &filename) {
		return delete_file(filename.c_str());
    }
#endif

	template<typename charT>
	bool _rename_file(const charT* from, const charT* to)
	{
		namespace fs = std::filesystem;
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
    bool rename_file(const char *old_filename, const char *new_filename) {
		return _rename_file(old_filename, new_filename);
    }

	bool rename_file(const std::string& old_filename, const std::string& new_filename) {
		return rename_file(old_filename.c_str(), new_filename.c_str());
	}

#ifdef MSTD_WINDOWS
    bool rename_file(const wchar_t *old_filename, const wchar_t *new_filename) {
		return _rename_file(old_filename, new_filename);
    }

    bool rename_file(const std::wstring &old_filename, const std::wstring &new_filename) {
		return rename_file(old_filename.c_str(), new_filename.c_str());
    }
#endif

    //文件移动 
    bool move_file(const char *old_path, const char *new_path) {
#if defined(MSTD_WINDOWS)
        return move_file(std::string(old_path), std::string(new_path));
#else
        if (rename(old_path, new_path) == 0) return true;
        return false;
#endif
    }

	bool move_file(const std::string& old_path, const std::string& new_path) {
#if defined(MSTD_WINDOWS)
		return move_file(mstd::to_wstring(old_path).c_str(), mstd::to_wstring(new_path).c_str());
#else
		return move_file(old_path.c_str(), new_path.c_str());
#endif
	}

#ifdef MSTD_WINDOWS
    bool move_file(const wchar_t *old_path, const wchar_t *new_path) {
        if (FALSE == ::MoveFileExW(old_path, new_path, MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED)) return false;
        return true;
    }

    bool move_file(const std::wstring &old_path, const std::wstring &new_path) {
        return move_file(old_path.c_str(), new_path.c_str());
    }
#endif

#ifdef MSTD_LINUX
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
#ifdef MSTD_WINDOWS
        if (FALSE == ::CopyFileA(src_path, dest_path, FALSE)) return false;
        return true;
#else
        return 0 == create_copy_file(src_path, dest_path);
#endif
    }

	bool copy_file(const std::string& old_path, const std::string& new_path) {
#ifdef MSTD_WINDOWS
		return copy_file(old_path.c_str(), new_path.c_str());
#else
		return 0 == create_copy_file(old_path.c_str(), new_path.c_str());
#endif
	}

#ifdef MSTD_WINDOWS
    bool copy_file(const wchar_t *src_path, const wchar_t *dest_path) {
        if (FALSE == ::CopyFileW(src_path, dest_path, FALSE)) return false;
        return true;
    }

    bool copy_file(const std::wstring &old_path, const std::wstring &new_path) {
        return copy_file(old_path.c_str(), new_path.c_str());
    }
#endif
    ///////////////////////////////目录操作////////////////////////////////////////////////////////////////
    bool delete_directory(const char* path) {
		return std::filesystem::remove_all(path);
    }

	bool delete_directory(const std::string& path) {
		return std::filesystem::remove_all(path);
	}

#ifdef MSTD_WINDOWS
    bool delete_directory(const wchar_t* path) {
		return std::filesystem::remove_all(path);
    }

	bool delete_directory(const std::wstring& path) {
		return std::filesystem::remove_all(path);
	}
#endif

    bool is_exists_directory(const char* path) {
		return std::filesystem::is_directory(path);
    }

    bool is_exists_directory(const std::string& path) {
        return is_exists_directory(path.c_str());
    }

#ifdef MSTD_WINDOWS
    bool is_exists_directory(const std::wstring& path) {
        return is_exists_directory(path.c_str());
    }

	bool is_exists_directory(const wchar_t* path) {
		return std::filesystem::is_directory(path);
	}
#endif
	template<typename charT>
	bool _create_directorys(const charT* path) {
		if (!std::filesystem::is_directory(path)) {
			try {
				return std::filesystem::create_directories(path);
			} catch (std::filesystem::filesystem_error &ex) {
				ex;
				return false;
			}
		}

		return false;
	}

    bool create_directorys(const char* path) {
		return _create_directorys(path);
    }

    bool create_directorys(const std::string& path) {
        return create_directorys(path.c_str());
    }

#ifdef MSTD_WINDOWS
	bool create_directorys(const wchar_t* path) {
		return _create_directorys(path);
	}

    bool create_directorys(const std::wstring& path) {
        return create_directorys(path.c_str());
    }
#endif

	template<typename charT>
	bool _delete_directorys(const charT* path) {
		if (std::filesystem::is_directory(path)) {
			try {
                std::filesystem::remove_all(path);
				return true;
			} catch (std::filesystem::filesystem_error &ex) {
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

	bool delete_directorys(const std::string& path)
	{
		return delete_directorys(path.c_str());
	}

#ifdef MSTD_WINDOWS
	bool delete_directorys(const wchar_t* path) {
		return _delete_directorys(path);
	}

	bool delete_directorys(const std::wstring& path) {
		return delete_directorys(path.c_str());
	}
#endif
    //目录不存在则创建
    template<typename charT>
    bool _verify_directory(const charT* path) {
        if (is_exists_directory(path)) {
            return true;
        }

        return _create_directorys(path);
    }

    bool verify_directory(const char* path) {
        return _verify_directory(path);
    }

    bool verify_directory(const std::string& path) {
        return _verify_directory(path.c_str());
    }

#ifdef MSTD_WINDOWS
    bool verify_directory(const wchar_t* path) {
        return _verify_directory(path);
    }

    bool verify_directory(const std::wstring& path) {
        return _verify_directory(path.c_str());
    }
#endif

    //
    bool enum_directory(const char* dir, std::vector<std::string> &dirs, std::vector<std::string> &files, const uint32_t level) {
        if (0 == level) return true;

        namespace fs = std::filesystem;
		try {
			// 检查目录是否存在且是一个有效的目录
			if (!fs::exists(dir) || !fs::is_directory(dir)) {
				return false;
			}

			// 清空输出容器
			dirs.clear();
			files.clear();

			// 遍历目录中的所有条目
			for (const auto& entry : fs::directory_iterator(dir)) {
				// 获取路径的字符串表示
				std::string path_str = entry.path().string();

				if (entry.is_directory()) {
					dirs.push_back(path_str);
					enum_directory(path_str.c_str(), dirs, files, level - 1);
				} else if (entry.is_regular_file()) {
					files.push_back(path_str);
				}
				// 可以根据需要添加对其他类型文件（如符号链接）的处理
			}

			return true;
		}
		catch (const fs::filesystem_error& e) {
			// 处理文件系统错误（如权限不足）
			return false;
		} catch (...) {
			// 处理其他未知错误
			return false;
		}

        return true;
    }

    bool enum_directory(const std::string& dir, std::vector<std::string> &dirs, std::vector<std::string> &files, uint32_t level) {
        return enum_directory(dir.c_str(), dirs, files, level);
    }

#ifdef MSTD_WINDOWS
	bool enum_directory(const wchar_t* dir, std::vector<std::wstring>& dirs, std::vector<std::wstring>& files, const uint32_t level) {
		if (0 == level) return true;

		namespace fs = std::filesystem;
		try {
			// 检查目录是否存在且是一个有效的目录
			if (!fs::exists(dir) || !fs::is_directory(dir)) {
				return false;
			}

			// 清空输出容器
			dirs.clear();
			files.clear();

			// 遍历目录中的所有条目
			for (const auto& entry : fs::directory_iterator(dir)) {
				// 获取路径的字符串表示
				std::wstring path_str = entry.path().wstring();
				if (entry.is_directory()) {
					dirs.push_back(path_str);
					enum_directory(path_str.c_str(), dirs, files, level - 1);
				} else if (entry.is_regular_file()) {
					files.push_back(path_str);
				}
				// 可以根据需要添加对其他类型文件（如符号链接）的处理
			}
		} catch (const fs::filesystem_error& e) {
			// 处理文件系统错误（如权限不足）
			return false;
		} catch (...) {
			// 处理其他未知错误
			return false;
		}

		return true;
	}

    bool enum_directory(const std::wstring& dir, std::vector<std::wstring> &dirs, std::vector<std::wstring> &files, uint32_t level){
        return enum_directory(dir.c_str(), dirs, files, level);
    }
#endif

    template<typename charT>
    bool _rename_directory(const charT* from, const charT* to){
        namespace fs = std::filesystem;
        if (fs::is_directory(from)){
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

	bool rename_directory(const std::string& old_filename, const std::string& new_filename) {
		return _rename_directory(old_filename.c_str(), new_filename.c_str());
	}

#ifdef MSTD_WINDOWS
    bool rename_directory(const wchar_t *old_filename, const wchar_t *new_filename) {
        return _rename_directory(old_filename, new_filename);
    }

    bool rename_directory(const std::wstring &old_filename, const std::wstring &new_filename) {
        return _rename_directory(old_filename.c_str(), new_filename.c_str());
    }
#endif

#if defined(MSTD_WINDOWS)
    BOOL MoveFolder(const wchar_t * lpszFromPath, const wchar_t * lpszToPath) {
		if (lpszFromPath == nullptr || lpszToPath == nullptr)
			return FALSE;

		int nLengthFrm = static_cast<int>(wcslen(lpszFromPath));
		// 分配足够的空间，包含原字符串、两个终止符和nullptr
		size_t newPathSize = nLengthFrm + 2;
		wchar_t* NewPathFrm = new wchar_t[newPathSize];

		// 使用wcscpy_s替代wcscpy，指定目标缓冲区大小
		errno_t err = wcscpy_s(NewPathFrm, newPathSize, lpszFromPath);
		if (err != 0) {
			delete[] NewPathFrm;
			return FALSE;
		}

		// 确保有两个终止符（SHFileOperation要求的格式）
		NewPathFrm[nLengthFrm] = L'\0';
		NewPathFrm[nLengthFrm + 1] = L'\0';

		SHFILEOPSTRUCTW FileOp;
		ZeroMemory(&FileOp, sizeof(SHFILEOPSTRUCTW));
		FileOp.fFlags = FOF_NOCONFIRMATION;
		FileOp.hNameMappings = NULL;
		FileOp.hwnd = NULL;
		FileOp.lpszProgressTitle = NULL;
		FileOp.pFrom = NewPathFrm;  // 需要以双null结尾的字符串
		FileOp.pTo = lpszToPath;
		FileOp.wFunc = FO_MOVE;

		BOOL bRet = (SHFileOperationW(&FileOp) == 0);
		delete[] NewPathFrm;
        return bRet;
    }
#endif

    //目录移动
    bool move_directory(const char *old_path, const char *new_path) {
		bool result = false;
#if defined(MSTD_WINDOWS)
        result = MoveFolder(mstd::to_wstring(old_path).c_str(), mstd::to_wstring(new_path).c_str()) ? true : false;
#else
		result = (rename(old_path, new_path) == 0) ? true : false;
#endif
		return result;
    }

	bool move_directory(const std::string& old_path, const std::string& new_path) {
		return move_directory(old_path.c_str(), new_path.c_str());
	}

#ifdef MSTD_WINDOWS
    bool move_directory(const wchar_t *old_path, const wchar_t *new_path) {
        if (FALSE == MoveFolder(old_path, new_path)) return false;
        return true;
    }

    bool move_directory(const std::wstring &old_path, const std::wstring &new_path) {
        return move_directory(old_path.c_str(), new_path.c_str());
    }
#endif
    ///////////////////////////////////////////////////////////////////////////////////////
}
