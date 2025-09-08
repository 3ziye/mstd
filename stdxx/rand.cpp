
#include "rand.h"
#ifndef _CRT_RAND_S
#define _CRT_RAND_S
#endif
#include <stdlib.h>
#include <tchar.h>
#include <time.h>

namespace stdxx {
	int rand_between(int min, int max) {
		if (max < 0 || min < 0 || max <= min)
			return 0;
		srand(time(0));
		unsigned int randnum = rand();
		int ret = (randnum % (unsigned)(max - min)) + min;
		return ret;
	}

	std::string rand_string(int count, const std::string& charset) {
		char str_temp[2] = { 0 };
		std::string str;
		int i, x;
		std::string tmpcharset = charset;
		unsigned int randnum = 0;
		if (tmpcharset.empty())
			tmpcharset = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
		if (count <= 0) {
			return "";
		}

		srand(time(0));
		for (i = 0; i < count; ++i) {
			randnum = rand();
			x = randnum % (unsigned)(tmpcharset.size() - 1);
			sprintf(str_temp, "%c", tmpcharset[x]);
			str.append(str_temp);
		}
		return str;
	}

	std::wstring rand_string(int count, const std::wstring& charset) {
		wchar_t str_temp[2] = { 0 };
		std::wstring str;
		int i, x;
		std::wstring tmpcharset = charset;
		unsigned int randnum = 0;
		if (tmpcharset.empty())
			tmpcharset = L"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
		if (count <= 0) {
			return L"";
		}

		srand(time(0));
		for (i = 0; i < count; ++i) {
			randnum = rand();
			x = randnum % (unsigned)(tmpcharset.size() - 1);
			swprintf_s(str_temp, L"%c", tmpcharset[x]);
			str.append(str_temp);
		}
		return str;
	}
};