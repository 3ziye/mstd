/*
 * SPDX-FileCopyrightText: Copyright 2025 (C) 3ziye org. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <vector>
#include <fstream>
#include <stdint.h>

namespace mstd
{
    //文件是否存在
    bool is_exists_file(const char* path);
    bool is_exists_file(const std::string& path);
#ifdef MSTD_WINDOWS
    bool is_exists_file(const wchar_t* path);
    bool is_exists_file(const std::wstring& path);
#endif
    //读取文件
    bool read_file(const char* filename, std::string& data);
    bool read_file(const std::string& filename, std::string& data);
#ifdef MSTD_WINDOWS
    bool read_file(const wchar_t* filename, std::string& data);
    bool read_file(const std::wstring &filename, std::string& data);
#endif
    //读取文件
    bool read_file(const char* filename, std::vector<char>& data);
    bool read_file(const std::string& filename, std::vector<char>& data);
    bool read_file(const wchar_t* filename, std::vector<char>& data);
    bool read_file(const std::wstring &filename, std::vector<char>& data);
    //写入文件
    bool write_file(const char* filename, const char* data, int size);
    bool write_file(const std::string& filename, const char* data, int size);
    bool write_file(const wchar_t* filename, const char* data, int size);
    bool write_file(const std::wstring &filename, const char* data, int size);
    //返回文件大小，失败返回-1
    int64_t get_file_size(const char* filename);
    int64_t get_file_size(const std::string& filename);
    int64_t get_file_size(const wchar_t* filename);
    int64_t get_file_size(const std::wstring &filename);

    struct fstat {
        //time_t     st_atime;    //文件最后被访问的时间
        time_t     st_time;       //文件内容最后被修改的时间
        int64_t    st_size;       //普通文件，对应的文件字节数
    };
    //获取文件属性
    bool get_file_stat(const char* filename, fstat *stat);
    bool get_file_stat(const std::string& filename, fstat* stat);
    bool get_file_stat(const wchar_t* filename, fstat *stat);
    bool get_file_stat(const std::wstring & filename, fstat *stat);
    //设置文件大小
    bool set_file_size(const char *filename, uint64_t size);
    bool set_file_size(const std::string& filename, uint64_t size);
    bool set_file_size(const wchar_t *filename, uint64_t size);
    bool set_file_size(const std::wstring &filename, uint64_t size);
	//设置文件时间
	bool set_file_modify_time(const char *filename, time_t mtime);
    bool set_file_modify_time(const std::string& filename, time_t mtime);
	bool set_file_modify_time(const wchar_t *filename, time_t mtime);
	bool set_file_modify_time(const std::wstring &filename, time_t mtime);

    //删除文件
    bool delete_file(const char* filename);
    bool delete_file(const std::string& filename);
    bool delete_file(const wchar_t* filename);
    bool delete_file(const std::wstring &filename);

    //文件改名
    bool rename_file(const char *old_filename, const char *new_filename);
    bool rename_file(const std::string& old_filename, const std::string& new_filename);
    bool rename_file(const wchar_t *old_filename, const wchar_t *new_filename);
    bool rename_file(const std::wstring &old_filename, const std::wstring &new_filename);

    //文件移动 
    bool move_file(const char *old_path, const char *new_path);
    bool move_file(const std::string& old_path, const std::string& new_path);
    bool move_file(const wchar_t *old_path, const wchar_t *new_path);
    bool move_file(const std::wstring &old_path, const std::wstring &new_path);

    //文件拷贝
    bool copy_file(const char *src_path, const char *dest_path);
    bool copy_file(const std::string& src_path, const std::string& dest_path);
    bool copy_file(const wchar_t *src_path, const wchar_t *dest_path);
    bool copy_file(const std::wstring &src_path, const std::wstring &dest_path);

    /////////////////////////////////////////////////////目录操作///////////////////////////////////////////////////////////////////////////////////////////
    //目录是否存在
    bool is_exists_directory(const char* path);
    bool is_exists_directory(const std::string& path);
    bool is_exists_directory(const wchar_t* path);
    bool is_exists_directory(const std::wstring& path);
    //创建目录
	bool create_directorys(const char* path);
    bool create_directorys(const std::string& path);
	bool create_directorys(const wchar_t* path);
	bool create_directorys(const std::wstring& path);
	//删除目录
	bool delete_directorys(const char* path);
    bool delete_directorys(const std::string& path);
	bool delete_directorys(const wchar_t* path);
	bool delete_directorys(const std::wstring& path);
    //目录不存在就创建
    bool verify_directory(const char* path);
    bool verify_directory(const std::string& path);
    bool verify_directory(const wchar_t* path);
    bool verify_directory(const std::wstring& path);
    //枚举目录
    bool enum_directory(const char* dir, std::vector<std::string> &dirs, std::vector<std::string> &files, const uint32_t level = - 1);
	bool enum_directory(const std::string& dir, std::vector<std::string>& dirs, std::vector<std::string>& files, const uint32_t level = -1);
    bool enum_directory(const wchar_t* dir, std::vector<std::wstring> &dirs, std::vector<std::wstring> &files, const uint32_t level = -1);
    bool enum_directory(const std::wstring& dir, std::vector<std::wstring> &dirs, std::vector<std::wstring> &files, const uint32_t level = -1);
    //目录改名
    bool rename_directory(const char *old_filename, const char *new_filename);
	bool rename_directory(const std::string& old_filename, const std::string& new_filename);
    bool rename_directory(const wchar_t *old_filename, const wchar_t *new_filename);
    bool rename_directory(const std::wstring &old_filename, const std::wstring &new_filename);

    //目录移动
    bool move_directory(const char *old_filename, const char *new_filename);
    bool move_directory(const std::string& old_filename, const std::string& new_filename);
    bool move_directory(const wchar_t *old_filename, const wchar_t *new_filename);
    bool move_directory(const std::wstring &old_filename, const std::wstring &new_filename);

    ////////////////////////////////////////////////////////////
}