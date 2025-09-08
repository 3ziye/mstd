#ifndef __CRC32C_H_
#define __CRC32C_H_

#include <cstddef>
#include <cstdint>

namespace mstd {

    namespace crc32 {
        std::uint32_t extend(
            std::uint32_t init_crc, const char* buf, std::size_t len);

        inline std::uint32_t value(const char* s, std::size_t n) {
            return extend(0, s, n);
        }

        static const std::uint32_t kMaskDelta = 0xa282ead8ul;

        inline std::uint32_t mask(std::uint32_t crc) {
            return ((crc >> 15) | (crc << 17)) + kMaskDelta;
        }

        inline std::uint32_t unmask(std::uint32_t masked_crc) {
            std::uint32_t rot = masked_crc - kMaskDelta;
            return ((rot >> 17) | (rot << 15));
        }
    };

    namespace ccbl {
        uint32_t crc32_sse(const uint8_t* buf, size_t len, uint32_t crc0 = 0);
    };

}

#endif // __CRC32C_H_
