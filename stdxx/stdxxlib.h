#ifndef __STDXX_STRING_H_
#define __STDXX_STRING_H_

#include "platform_define.h"
#include <stdlib.h>
#include <string>
#include <sstream>
#include <cstdlib>

#if defined(STDXX_POSIX) || defined(STDXX_MINGW)

// please note that extending namespace is actually undefined behaviour
// (because the implementation can extend this namespace)
// but in that case we are doing it intentionally.
namespace stdxx {

#if ((__GNUC__ < 5))
template <typename T>
std::string to_string(const T &value)
{
        std::ostringstream os;
        os << value;
        return os.str();
}
#endif

/* GCC < 4.8.4 */
#if (__GNUC__ == 4 && __GNUC_MINOR__ <= 9 )
inline int stoi(const std::string &s, size_t *idx = 0, int base = 10)
{
        char *endptr = 0;
        int v = strtol(s.c_str(), &endptr, base);
        if (idx) {
                *idx = endptr - s.c_str();
        }
        return v;
}

// fixes for lame android ndks and some mingws
inline float strtof(const char* str, char** endptr)
{
        return ::strtof(str, endptr);
}

inline float stof(const std::string& s)
{
    std::stringstream stream(s);

    float f;
    if ((stream >> f).fail() || !(stream >> std::ws).eof()) {
        throw std::bad_cast();
    }

    return f;
}

inline long double strtold(const char* str, char** endptr)
{
#if defined(__GNUC__) && ((__GNUC__ < 5))
    return stdxx::strtold(str, endptr);
#else
    return std::strtold(str, endptr);
#endif
}

inline long stol(const std::string& str, std::size_t* pos = 0, int base = 10)
{
	return strtol(str.c_str(), nullptr, base);
	
}

inline long long stoll(const std::string& str, std::size_t* pos = 0, int base = 10)
{
	return strtoll(str.c_str(), nullptr, base);
}

inline unsigned long stoul(const std::string& str, std::size_t* pos = 0, int base = 10)
{
	return strtoul(str.c_str(), nullptr, base);
}

inline unsigned long long stoull(const std::string& str, std::size_t* pos = 0, int base = 10)
{
	return strtoull(str.c_str(), nullptr, base);
}

inline double stod(const std::string& str, std::size_t* pos = 0)
{
	return strtod(str.c_str(), nullptr);
}

inline long double stold(const std::string& str, std::size_t* pos = 0)
{
	return strtold(str.c_str(), nullptr);
}

#endif

}

#endif

#endif // __STDXX_STRING_H_
