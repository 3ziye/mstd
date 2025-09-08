
#include "hex.h"

namespace mstd {
    std::string to_hex(const char* buff, const int buff_len) {
        char *str = (char*)malloc(buff_len * sizeof(char) * 2 + 1);
        int n = 0;
        for (int i = 0; i < buff_len; i++) {
            n += sprintf(str + n, "%02x", buff[i]);
        }

        std::string ret(str, n);
        free(str);
        return ret;
    }

    std::string from_hex(const char* buff, const int buff_len) {
        if (buff_len % 2 != 0) return "";

        char *str = (char*)malloc(buff_len / 2);

        int m = 0;
        for (int n = 0; n < buff_len; n = n + 2) {
            *(str + m) = (*(buff + n) - '0') * 16; 
            *(str + m) += *(buff + n + 1) - '0';
            m++;
        }

        std::string ret(str, buff_len / 2);
        free(str);
        return ret;
    }
}
