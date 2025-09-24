#ifndef __CRC32C_H_
#define __CRC32C_H_

#include <cstddef>
#include <cstdint>

namespace mstd {
    uint32_t crc32(const uint8_t* data, size_t size);
    uint32_t crc32_sse(const uint8_t* buf, size_t len, uint32_t crc0 = 0);
}

#endif // __CRC32C_H_
