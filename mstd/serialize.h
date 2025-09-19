#pragma once
#pragma warning(disable:4267)
#include "bytebuffer.h"
#include "platform.h"
#include <istream>
#include <stdint.h>
#include <vector>
#include <list>
#include <map>
#include <assert.h>

namespace mstd {
	// 对于有size()方法的类型
	template <typename T>
	inline auto t_size(const T& t) -> decltype(t.size(), uint32_t()) {
		return static_cast<uint32_t>(t.size());
	}

	// 对于内置类型 - 使用重载而不是特化
	inline uint32_t t_size(bool) { return 1; }
	inline uint32_t t_size(uint8_t) { return 1; }
	inline uint32_t t_size(uint16_t) { return 2; }
	inline uint32_t t_size(uint32_t) { return 4; }
	inline uint32_t t_size(uint64_t) { return 8; }
	// 对于std::string - 如果需要特殊处理
	inline uint32_t t_size(const std::string& t) {
		return static_cast<uint32_t>(t.size()); // 或者根据实际协议调整
	}

	template <typename... Args>
	inline uint32_t ts_size(const Args&... args) {
		return (t_size(args) + ...);
	}

	/////////////////////////////////////////////////////////////////////////////
    template <typename T>
    inline void write(ByteBuffer &buff, const T &t) {
        t.serialize(buff);
    }

    template <typename T>
    inline void write(ByteBuffer &buff, const std::vector<T> &t) {
        uint32_t n = uint32_t(t.size());
        write(buff, n);
        for (auto &it : t) {
            write(buff, it);
        }
    }

	template <typename T>
	inline void write(ByteBuffer &buff, const std::list<T> &t) {
		uint32_t n = t.size();
		write(buff, n);
		for (auto &it : t) {
			write(buff, it);
		}
	}

    template <>
    inline void write<bool>(ByteBuffer &buff, const bool &t) {
        uint8_t n = 0;
        if (t) n = 1;
        buff.Write(n);
    }

	template <>
	inline void write<uint8_t>(ByteBuffer &buff, const uint8_t &t) {
		buff.Write(t);
	}

	template <>
	inline void write<uint16_t>(ByteBuffer &buff, const uint16_t &t) {
		buff.Write(t);
	}

	template <>
	inline void write<uint32_t>(ByteBuffer &buff, const uint32_t &t) {
		buff.Write(t);
	}

    template <>
    inline void write<uint64_t>(ByteBuffer &buff, const uint64_t &t) {
        buff.Write(t);
    }

	template <>
	inline void write<std::string>(ByteBuffer &buff, const std::string &t) {
		uint32_t n = t.size();
		buff.Write(n);
        if (!t.empty())
		    buff.Write((char*)t.c_str(), t.size());
	}

	template <typename... Args>
	inline void serialize(ByteBuffer& buff, Args&&... args) {
		(write(buff, std::forward<Args>(args)), ...);
	}

	////////////////////////////////////////////////////////////////////////////////////

	template <typename T>
	inline bool read(ByteBuffer &buff, T &t) {
		return t.deserialize(buff);
	}

	template <typename T>
	inline bool read(ByteBuffer &buff, std::list<T> &t) {
		uint32_t n = 0;
		if (!read<uint32_t>(buff, n)) return false;

		while (n--) {
			T m;
			if (!read(buff, m)) return false;

			t.push_back(m);
		}

		return true;
	}

    template <typename T>
    inline bool read(ByteBuffer &buff, std::vector<T> &t) {
        uint32_t n = 0;
		if (!read<uint32_t>(buff, n)) return false;

        while (n--) {
            T m;
			if (!read(buff, m)) return false;

            t.push_back(m);
        }

		return true;
    }

    template <>
    inline bool read<bool>(ByteBuffer &buff, bool &t) {
		uint8_t v = 0;
		if (auto ret = buff.Read(t); ret) {
			if (v)
				t = true;
			else
				t = false;
			return true;
		} 

		return false;
    }

	template <>
	inline bool read<uint8_t>(ByteBuffer &buff, uint8_t &t) {
		return buff.Read(t);
	}

	template <>
	inline bool read<uint16_t>(ByteBuffer &buff, uint16_t &t) {
		return buff.Read(t);
	}

	template <>
	inline bool read<uint32_t>(ByteBuffer &buff, uint32_t &t) {
		return buff.Read(t);
	}

    template <>
    inline bool read<uint64_t>(ByteBuffer &buff, uint64_t &t) {
		return buff.Read(t);
    }

	template <>
	inline bool read<std::string>(ByteBuffer &buff, std::string &t) {
		uint32_t size = 0;
		if (!buff.Read(size)) {
			return false;
		}

		t.resize(size);
		if (!buff.Read((char*)t.data(), size)) {
			return false;
		}

		return true;
	}

	template <typename... Args>
	inline bool deserialize(ByteBuffer& buff, Args&... args) {
		return (read(buff, args) && ...);
	}

	template<typename T, int N>
	class array {
	public:
		void serialize(ByteBuffer& buff) {
			for (int n = 0; n < N; n++) {
				write(buff, m[n]);
			}
		}

		bool deserialize(ByteBuffer& buff) {
			for (int n = 0; n < N; n++) {
				if (!read(buff, m[n])) return false;
			}

			return true;
		}

		T &operator[](int index) {
			if (index < 0 || index >= N) {
				throw std::out_of_range("Index out of range");
			}

			return m[index];
		}

		T* val() {
			return m;
		}

		uint32_t size() {
			return N;
		}
	private:
		T m[N];
	};
};

#define mstd_serialize(...) \
    virtual void serialize(ByteBuffer& buff){\
		mstd::serialize(buff, ##__VA_ARGS__);\
	}\
    virtual bool deserialize(ByteBuffer& buff){\
	    return mstd::deserialize(buff, ##__VA_ARGS__); \
	}\
    uint32_t size() {\
        return mstd::ts_size(__VA_ARGS__);\
    }

