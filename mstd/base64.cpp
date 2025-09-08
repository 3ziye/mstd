
#include "base64.h"

namespace mstd {

  using byte_t = std::uint8_t;
  static const char* base64_initilize(void);

  static char kDecode64[257] = {};
  static const char* kEncode64 = base64_initilize();

  static const char* base64_initilize(void) {
    const char* s =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    for (int i = 0; i < 64; ++i)
      kDecode64[static_cast<int>(s[i])] = i;
    return s;
  }

  std::string base64_encode(const char* s, std::size_t n) {
    std::string r;

    const byte_t* b = reinterpret_cast<const byte_t*>(s);
    for (std::size_t i = 0; i < n; i += 3) {
      r.push_back(kEncode64[b[i] >> 2]);

      if (i + 1 < n) {
        r.push_back(kEncode64[((b[i] & 0x03) << 4) | (b[i + 1] >> 4)]);
      }
      else {
        r.push_back(kEncode64[(b[i] & 0x03) << 4]);
        r.append("==");
        break;
      }

      if (i + 2 < n) {
        r.push_back(kEncode64[((b[i + 1] & 0x0f) << 2) | (b[i + 2] >> 6)]);
        r.push_back(kEncode64[b[i + 2] & 0x3f]);
      }
      else {
        r.push_back(kEncode64[(b[i + 1] & 0x0f) << 2]);
        r.push_back('=');
        break;
      }
    }

    return std::move(r);
  }

  std::string base64_decode(const char* s, std::size_t n) {
    std::string r;

    const byte_t* b = reinterpret_cast<const byte_t*>(s);
    for (std::size_t i = 0; i < n; i += 4) {
      r.push_back((kDecode64[b[i]] << 2) | ((kDecode64[b[i + 1]] >> 4) & 0x03));
      if (b[i + 2] != '=')
        r.push_back(
            (kDecode64[b[i + 1]] << 4) | ((kDecode64[b[i + 2]] >> 2) & 0x0f));
      if (b[i + 3] != '=')
        r.push_back((kDecode64[b[i + 2]] << 6) | (kDecode64[b[i + 3]] & 0x3f));
    }

    return std::move(r);
  }
}
