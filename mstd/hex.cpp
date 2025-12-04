
#include "hex.h"
#include "platform.h"
namespace mstd {
    std::string to_hex(const char* buff, const int buff_len) {
		if (!buff || buff_len <= 0) {
			return "";
		}

		// 分配足够空间：每个字节转换为2个十六进制字符 + 空终止符
		const size_t str_size = static_cast<size_t>(buff_len) * 2 + 1;
		char* str = static_cast<char*>(malloc(str_size));
		if (!str) {
			return "";  // 内存分配失败
	}

		int n = 0;
		for (int i = 0; i < buff_len; i++) {
			// 使用无符号字符避免符号扩展问题
			unsigned char uc = static_cast<unsigned char>(buff[i]);
#ifdef MSTD_WINDOWS
			// sprintf_s: 第二个参数是包括空终止符的总缓冲区大小
			// str + n 指向当前位置，剩余空间是 str_size - n
			int written = sprintf_s(str + n, str_size - n, "%02x", uc);
#else
			int written = sprintf(str + n, "%02x", uc);
#endif
			if (written > 0) {
				n += written;
			} else {
				// 处理错误
				break;
			}
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
