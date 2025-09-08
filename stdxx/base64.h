#ifndef __BASE64_H_
#define __BASE64_H_

#include <cstring>
#include <string>

namespace stdxx {

  std::string base64_encode(const char* s, std::size_t n);
  std::string base64_decode(const char* s, std::size_t n);

  inline std::string base64_encode(const char* s) {
    return base64_encode(s, std::strlen(s));
  }

  inline std::string base64_decode(const char* s) {
    return base64_decode(s, std::strlen(s));
  }

  inline std::string base64_encode(const std::string& s) {
    return base64_encode(s.data(), s.size());
  }

  inline std::string base64_decode(const std::string& s) {
    return base64_decode(s.data(), s.size());
  }
}

#endif // __BASE64_H_
