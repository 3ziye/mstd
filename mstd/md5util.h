#ifndef __MD5UTIL_H_
#define __MD5UTIL_H_

#include <string>

namespace mstd
{
	/**
	 * @brief md5_string
	 * @note 获取字符串md5
	 * @param str
	 * @param md5_hash
	 * @return
	 */
	bool md5_string(const std::string& str, std::string& md5);

	/**
	 * @brief md5_string
	 * @note 获取字符串md5
	 * @param str
	 * @return
	 */
	std::string md5_string(const std::string& str);

	/**
	 * @brief md5_file
	 * @note 获取文件md5
	 * @param filename
	 * @param md5_hash
	 * @return
	 */
	bool md5_file(const char* filename, std::string& md5);

	/**
	 * @brief md5_file
	 * @note 获取文件md5
	 * @param filename
	 * @return
	 */
	std::string md5_file(const char* filename);
};

#endif //__MD5UTIL_H_
