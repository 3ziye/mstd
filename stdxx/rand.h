
#ifndef __STDRAND_H_
#define __STDRAND_H_
#include <string>

namespace stdxx {
	int rand_between(int min, int max);

	std::string rand_string(int count, const std::string& charset);
	std::wstring rand_string(int count, const std::wstring& charset);
};

#endif