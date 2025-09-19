#pragma once
#pragma warning(disable:4267)
#include "bytebuffer.h"
#include "stdxx/platform_define.h"
#include <istream>
#include <stdint.h>
#include <vector>
#include <list>
#include <map>
#include <assert.h>
#ifdef MSTD_WINDOWS
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

namespace mstd {
	template <typename T>
	inline uint32_t size(T &t) {
		return uint32_t(t.size());
	}

	template <>
    inline uint32_t size<bool>(bool &) {
		return 1;
	}

	template <>
    inline uint32_t size<uint8_t>(uint8_t &) {
		return 1;
	}

	template <>
    inline uint32_t size<uint16_t>(uint16_t &) {
		return 2;
	}

	template <>
    inline uint32_t size<uint32_t>(uint32_t &) {
		return 4;
	}

	template <>
    inline uint32_t size<uint64_t>(uint64_t &) {
		return 8;
	}

	template <>
    inline uint32_t size<std::string>(std::string &t) {
		return uint32_t(4 + t.size());
	}

	inline uint32_t append_size() {
		return 0;
	}

	template <typename T1>
	inline uint32_t append_size(T1 &t) {
		return size(t);
	}

	template <typename T1, typename T2>
	inline uint32_t append_size(T1 &t1, T2 &t2) {
		return size(t1) + size(t2);
	}

	template <typename T1, typename T2, typename T3>
	inline uint32_t append_size(T1 &t1, T2 &t2, T3 &t3) {
		return size(t1) + size(t2) + size(t3);
	}

	template <typename T1, typename T2, typename T3, typename T4>
	inline uint32_t append_size(T1 &t1, T2 &t2, T3 &t3, T4 &t4) {
		return size(t1) + size(t2) + size(t3) + size(t4);
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5>
	inline uint32_t append_size(T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5) {
		return size(t1) + size(t2) + size(t3) + size(t4) + size(t5);
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	inline uint32_t append_size(T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6) {
		return size(t1) + size(t2) + size(t3) + size(t4) + size(t5) + size(t6);
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	inline uint32_t append_size(T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7) {
		return size(t1) + size(t2) + size(t3) + size(t4) + size(t5) + size(t6) + size(t7);
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	inline uint32_t append_size(T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8) {
		return size(t1) + size(t2) + size(t3) + size(t4) + size(t5) + size(t6) + size(t7) + size(t8);
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
	inline uint32_t append_size(T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9) {
		return size(t1) + size(t2) + size(t3) + size(t4) + size(t5) + size(t6) + size(t7) + size(t8) + size(t9);
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
	inline uint32_t append_size(T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10) {
		return size(t1) + size(t2) + size(t3) + size(t4) + size(t5) + size(t6) + size(t7) + size(t8) + size(t9) + size(t10);
	}

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
    inline uint32_t append_size(T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10, T11 &t11) {
        return size(t1) + size(t2) + size(t3) + size(t4) + size(t5) + size(t6) + size(t7) + size(t8) + size(t9) + size(t10) + size(t11);
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12>
    inline uint32_t append_size(T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10, T11 &t11, T12 &t12) {
        return size(t1) + size(t2) + size(t3) + size(t4) + size(t5) + size(t6) + size(t7) + size(t8) + size(t9) + size(t10) + size(t11) + size(t12);
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13>
    inline uint32_t append_size(T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10, T11 &t11, T12 &t12, T13 &t13) {
        return size(t1) + size(t2) + size(t3) + size(t4) + size(t5) + size(t6) + size(t7) + size(t8) + size(t9) + size(t10) + size(t11) + size(t12) + size(t13);
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14>
    inline uint32_t append_size(T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10, T11 &t11, T12 &t12, T13 &t13, T14 &t14) {
        return size(t1) + size(t2) + size(t3) + size(t4) + size(t5) + size(t6) + size(t7) + size(t8) + size(t9) + size(t10) + size(t11) + size(t12) + size(t13) + size(t14);
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15>
    inline uint32_t append_size(T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10, T11 &t11, T12 &t12, T13 &t13, T14 &t14, T15 &t15) {
        return size(t1) + size(t2) + size(t3) + size(t4) + size(t5) + size(t6) + size(t7) + size(t8) + size(t9) + size(t10) + size(t11) + size(t12) + size(t13) + size(t14) + size(t15);
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15, typename T16>
    inline uint32_t append_size(T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10, T11 &t11, T12 &t12, T13 &t13, T14 &t14, T15 &t15, T16 &t16) {
        return size(t1) + size(t2) + size(t3) + size(t4) + size(t5) + size(t6) + size(t7) + size(t8) + size(t9) + size(t10) + size(t11) + size(t12) + size(t13) + size(t14) + size(t15) + size(t16);
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17>
    inline uint32_t append_size(T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10, T11 &t11, T12 &t12, T13 &t13, T14 &t14, T15 &t15, T16 &t16, T17 &t17) {
        return size(t1) + size(t2) + size(t3) + size(t4) + size(t5) + size(t6) + size(t7) + size(t8) + size(t9) + size(t10) + size(t11) + size(t12) + size(t13) + size(t14) + size(t15) + size(t16) + size(t17);
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18>
    inline uint32_t append_size(T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10, T11 &t11, T12 &t12, T13 &t13, T14 &t14, T15 &t15, T16 &t16, T17 &t17, T17 &t18) {
        return size(t1) + size(t2) + size(t3) + size(t4) + size(t5) + size(t6) + size(t7) + size(t8) + size(t9) + size(t10) + size(t11) + size(t12) + size(t13) + size(t14) + size(t15) + size(t16) + size(t17) + size(t18);
    }


	/////////////////////////////////////////////////////////////////////////////
    template <typename T>
    inline void write(ByteBuffer &buff, T &t) {
        t.serialize(buff);
    }

    template <typename T>
    inline void write(ByteBuffer &buff, std::vector<T> &t) {
        uint32_t n = uint32_t(t.size());
        write(buff, n);
        for (auto &it : t) {
            write(buff, it);
        }
    }

	template <typename T>
	inline void write(ByteBuffer &buff, std::list<T> &t) {
		uint32_t n = t.size();
		write(buff, n);
		for (auto &it : t) {
			write(buff, it);
		}
	}

    template <>
    inline void write<bool>(ByteBuffer &buff, bool &t) {
        char n = 0;
        if (t) n = 1;
        buff.Write(&n, 1);
    }

	template <>
	inline void write<uint8_t>(ByteBuffer &buff, uint8_t &t) {
		buff.Write((char*)&t, 1);
	}

	template <>
	inline void write<uint16_t>(ByteBuffer &buff, uint16_t &t) {
		buff.Write((char*)&t, 2);
	}

	template <>
	inline void write<uint32_t>(ByteBuffer &buff, uint32_t &t) {
		buff.Write((char*)&t, 4);
	}

    template <>
    inline void write<uint64_t>(ByteBuffer &buff, uint64_t &t) {
        buff.Write((char*)&t, 8);
    }

	template <>
	inline void write<std::string>(ByteBuffer &buff, std::string &t) {
		uint32_t v = t.size();
		buff.Write((char*)&v, 4);
        if (!t.empty())
		    buff.Write((char*)t.c_str(), t.size());
	}

    inline void serialize(ByteBuffer &) {
	}

	template <typename T1>
	inline void serialize(ByteBuffer &buff, T1 &t) {
		write(buff, t);
	}

	template <typename T1, typename T2>
	inline void serialize(ByteBuffer &buff, T1 &t1, T2 &t2) {
		write(buff, t1);
		write(buff, t2);
	}

	template <typename T1, typename T2, typename T3>
	inline void serialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3) {
		write(buff, t1);
		write(buff, t2);
		write(buff, t3);
	}

	template <typename T1, typename T2, typename T3, typename T4>
	inline void serialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4) {
		write(buff, t1);
		write(buff, t2);
		write(buff, t3);
		write(buff, t4);
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5>
	inline void serialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5) {
		write(buff, t1);
		write(buff, t2);
		write(buff, t3);
		write(buff, t4);
		write(buff, t5);
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	inline void serialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6) {
		write(buff, t1);
		write(buff, t2);
		write(buff, t3);
		write(buff, t4);
		write(buff, t5);
		write(buff, t6);
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	inline void serialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7) {
		write(buff, t1);
		write(buff, t2);
		write(buff, t3);
		write(buff, t4);
		write(buff, t5);
		write(buff, t6);
		write(buff, t7);
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	inline void serialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8) {
		write(buff, t1);
		write(buff, t2);
		write(buff, t3);
		write(buff, t4);
		write(buff, t5);
		write(buff, t6);
		write(buff, t7);
		write(buff, t8);
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
	inline void serialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9) {
		write(buff, t1);
		write(buff, t2);
		write(buff, t3);
		write(buff, t4);
		write(buff, t5);
		write(buff, t6);
		write(buff, t7);
		write(buff, t8);
		write(buff, t9);
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
	inline void serialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10) {
		write(buff, t1);
		write(buff, t2);
		write(buff, t3);
		write(buff, t4);
		write(buff, t5);
		write(buff, t6);
		write(buff, t7);
		write(buff, t8);
		write(buff, t9);
		write(buff, t10);
	}

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
    inline void serialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10, T11 &t11) {
        write(buff, t1);
        write(buff, t2);
        write(buff, t3);
        write(buff, t4);
        write(buff, t5);
        write(buff, t6);
        write(buff, t7);
        write(buff, t8);
        write(buff, t9);
        write(buff, t10);
        write(buff, t11);
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12>
    inline void serialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10, T11 &t11, T12 &t12) {
        write(buff, t1);
        write(buff, t2);
        write(buff, t3);
        write(buff, t4);
        write(buff, t5);
        write(buff, t6);
        write(buff, t7);
        write(buff, t8);
        write(buff, t9);
        write(buff, t10);
        write(buff, t11);
        write(buff, t12);
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13>
    inline void serialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10, T11 &t11, T12 &t12, T13 &t13) {
        write(buff, t1);
        write(buff, t2);
        write(buff, t3);
        write(buff, t4);
        write(buff, t5);
        write(buff, t6);
        write(buff, t7);
        write(buff, t8);
        write(buff, t9);
        write(buff, t10);
        write(buff, t11);
        write(buff, t12);
        write(buff, t13);
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14>
    inline void serialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10, T11 &t11, T12 &t12, T13 &t13, T14 &t14) {
        write(buff, t1);
        write(buff, t2);
        write(buff, t3);
        write(buff, t4);
        write(buff, t5);
        write(buff, t6);
        write(buff, t7);
        write(buff, t8);
        write(buff, t9);
        write(buff, t10);
        write(buff, t11);
        write(buff, t12);
        write(buff, t13);
        write(buff, t14);
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15>
    inline void serialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10, T11 &t11, T12 &t12, T13 &t13, T14 &t14, T15 &t15) {
        write(buff, t1);
        write(buff, t2);
        write(buff, t3);
        write(buff, t4);
        write(buff, t5);
        write(buff, t6);
        write(buff, t7);
        write(buff, t8);
        write(buff, t9);
        write(buff, t10);
        write(buff, t11);
        write(buff, t12);
        write(buff, t13);
        write(buff, t14);
        write(buff, t15);
    }


    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15, typename T16>
    inline void serialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10, T11 &t11, T12 &t12, T13 &t13, T14 &t14, T15 &t15, T16 &t16) {
        write(buff, t1);
        write(buff, t2);
        write(buff, t3);
        write(buff, t4);
        write(buff, t5);
        write(buff, t6);
        write(buff, t7);
        write(buff, t8);
        write(buff, t9);
        write(buff, t10);
        write(buff, t11);
        write(buff, t12);
        write(buff, t13);
        write(buff, t14);
        write(buff, t15);
        write(buff, t16);
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17>
    inline void serialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10, T11 &t11, T12 &t12, T13 &t13, T14 &t14, T15 &t15, T16 &t16, T17 &t17) {
        write(buff, t1);
        write(buff, t2);
        write(buff, t3);
        write(buff, t4);
        write(buff, t5);
        write(buff, t6);
        write(buff, t7);
        write(buff, t8);
        write(buff, t9);
        write(buff, t10);
        write(buff, t11);
        write(buff, t12);
        write(buff, t13);
        write(buff, t14);
        write(buff, t15);
        write(buff, t16);
        write(buff, t17);
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18>
    inline void serialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10, T11 &t11, T12 &t12, T13 &t13, T14 &t14, T15 &t15, T16 &t16, T17 &t17, T18 &t18) {
        write(buff, t1);
        write(buff, t2);
        write(buff, t3);
        write(buff, t4);
        write(buff, t5);
        write(buff, t6);
        write(buff, t7);
        write(buff, t8);
        write(buff, t9);
        write(buff, t10);
        write(buff, t11);
        write(buff, t12);
        write(buff, t13);
        write(buff, t14);
        write(buff, t15);
        write(buff, t16);
        write(buff, t17);
        write(buff, t18);
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
        if (buff.Size() < 1)
            return false;

        if (*buff.ReadBegin())
            t = true;
        else
            t = false;

        buff.ReadBytes(1);
		return true;
    }

	template <>
	inline bool read<uint8_t>(ByteBuffer &buff, uint8_t &t) {
		if (buff.Size() < 1)
			return false;

		t = *buff.ReadBegin();
		buff.ReadBytes(1);
		return true;
	}

	template <>
	inline bool read<uint16_t>(ByteBuffer &buff, uint16_t &t) {
		if (buff.Size() < 2)
			return false;

		t = *(uint16_t*)buff.ReadBegin();
		buff.ReadBytes(2);
		return true;
	}

	template <>
	inline bool read<uint32_t>(ByteBuffer &buff, uint32_t &t) {
		if (buff.Size() < 4)
			return false;

        t = *(uint32_t*)buff.ReadBegin();
		buff.ReadBytes(4);
		return true;
	}

    template <>
    inline bool read<uint64_t>(ByteBuffer &buff, uint64_t &t) {
        if (buff.Size() < 8)
            return false;

        t = *(uint64_t*)buff.ReadBegin();
        buff.ReadBytes(8);
		return true;
    }


	template <>
	inline bool read<std::string>(ByteBuffer &buff, std::string &t) {
		if (buff.Size() < 4)
			return false;

		uint32_t len = *(uint32_t*)buff.ReadBegin();
		buff.ReadBytes(4);

        if (!len) {
            t = "";
            return true;
        }

		if (buff.Size() < len)
			return false;

		t.assign(buff.ReadBegin(), len);
		buff.ReadBytes(len);
		return true;
	}

    inline bool deserialize(ByteBuffer &) {
		return true;
	}

	template <typename T1>
	inline bool deserialize(ByteBuffer &buff, T1 &t) {
		return read(buff, t);
	}

	template <typename T1, typename T2>
	inline bool deserialize(ByteBuffer &buff, T1 &t1, T2 &t2) {
		return read(buff, t1) && read(buff, t2);
	}

	template <typename T1, typename T2, typename T3>
	inline bool deserialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3) {
		return read(buff, t1) && read(buff, t2) && read(buff, t3);
	}

	template <typename T1, typename T2, typename T3, typename T4>
	inline bool deserialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4) {
		return read(buff, t1) && read(buff, t2) && read(buff, t3) && read(buff, t4);
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5>
	inline bool deserialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5) {
		return read(buff, t1) && read(buff, t2) && read(buff, t3) && read(buff, t4) && read(buff, t5);
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	inline bool deserialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6) {
		return read(buff, t1) && read(buff, t2) && read(buff, t3) && read(buff, t4) && read(buff, t5) &&
				read(buff, t6);
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	inline bool deserialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7) {
		return read(buff, t1) && read(buff, t2) && read(buff, t3) && read(buff, t4) && read(buff, t5) &&
			read(buff, t6) &&read(buff, t7);
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	inline bool deserialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8) {
		return read(buff, t1) && read(buff, t2) && read(buff, t3) && read(buff, t4) && read(buff, t5) &&
			read(buff, t6) && read(buff, t7) && read(buff, t8);
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
	inline bool deserialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9) {
		return read(buff, t1) && read(buff, t2) && read(buff, t3) && read(buff, t4) && read(buff, t5) &&
			read(buff, t6) && read(buff, t7) && read(buff, t8) && read(buff, t9);
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
	inline bool deserialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10) {
		return read(buff, t1) && read(buff, t2) && read(buff, t3) && read(buff, t4) && read(buff, t5) &&
			read(buff, t6) && read(buff, t7) && read(buff, t8) && read(buff, t9) && read(buff, t10);
	}

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
    inline bool deserialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10, T11 &t11) {
		return read(buff, t1) && read(buff, t2) && read(buff, t3) && read(buff, t4) && read(buff, t5) &&
			read(buff, t6) && read(buff, t7) && read(buff, t8) && read(buff, t9) && read(buff, t10) &&
			read(buff, t11);
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12>
    inline bool deserialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10, T11 &t11, T12 &t12) {
		return read(buff, t1) && read(buff, t2) && read(buff, t3) && read(buff, t4) && read(buff, t5) &&
			read(buff, t6) && read(buff, t7) && read(buff, t8) && read(buff, t9) && read(buff, t10) &&
			read(buff, t11) && read(buff, t12);
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13>
    inline bool deserialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10, T11 &t11, T12 &t12, T13 &t13) {
		return read(buff, t1) && read(buff, t2) && read(buff, t3) && read(buff, t4) && read(buff, t5) &&
			read(buff, t6) && read(buff, t7) && read(buff, t8) && read(buff, t9) && read(buff, t10) &&
			read(buff, t11) && read(buff, t12) && read(buff, t13);
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14>
    inline bool deserialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10, T11 &t11, T12 &t12, T13 &t13, T14 &t14) {
		return read(buff, t1) && read(buff, t2) && read(buff, t3) && read(buff, t4) && read(buff, t5) &&
			read(buff, t6) && read(buff, t7) && read(buff, t8) && read(buff, t9) && read(buff, t10) &&
			read(buff, t11) && read(buff, t12) && read(buff, t13) && read(buff, t14);
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15>
    inline bool deserialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10, T11 &t11, T12 &t12, T13 &t13, T14 &t14, T15 &t15) {
		return read(buff, t1) && read(buff, t2) && read(buff, t3) && read(buff, t4) && read(buff, t5) &&
			read(buff, t6) && read(buff, t7) && read(buff, t8) && read(buff, t9) && read(buff, t10) &&
			read(buff, t11) && read(buff, t12) && read(buff, t13) && read(buff, t14) && read(buff, t15);
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15, typename T16>
    inline bool deserialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10, T11 &t11, T12 &t12, T13 &t13, T14 &t14, T15 &t15, T16 &t16) {
		return read(buff, t1) && read(buff, t2) && read(buff, t3) && read(buff, t4) && read(buff, t5) &&
			read(buff, t6) && read(buff, t7) && read(buff, t8) && read(buff, t9) && read(buff, t10) &&
			read(buff, t11) && read(buff, t12) && read(buff, t13) && read(buff, t14) && read(buff, t15) &&
			read(buff, t16);
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17>
    inline bool deserialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10, T11 &t11, T12 &t12, T13 &t13, T14 &t14, T15 &t15, T16 &t16, T17 &t17) {
		return read(buff, t1) && read(buff, t2) && read(buff, t3) && read(buff, t4) && read(buff, t5) &&
			read(buff, t6) && read(buff, t7) && read(buff, t8) && read(buff, t9) && read(buff, t10) &&
			read(buff, t11) && read(buff, t12) && read(buff, t13) && read(buff, t14) && read(buff, t15) &&
			read(buff, t16) && read(buff, t17);
    }

    template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18>
    inline bool deserialize(ByteBuffer &buff, T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5, T6 &t6, T7 &t7, T8 &t8, T9 &t9, T10 &t10, T11 &t11, T12 &t12, T13 &t13, T14 &t14, T15 &t15, T16 &t16, T17 &t17, T18 &t18) {
		return read(buff, t1) && read(buff, t2) && read(buff, t3) && read(buff, t4) && read(buff, t5) &&
			read(buff, t6) && read(buff, t7) && read(buff, t8) && read(buff, t9) && read(buff, t10) &&
			read(buff, t11) && read(buff, t12) && read(buff, t13) && read(buff, t14) && read(buff, t15) &&
			read(buff, t16) && read(buff, t17) && read(buff, t18);
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

#define mypack_serialize(...) \
    virtual void serialize(ByteBuffer& buff){\
		mstd::serialize(buff, ##__VA_ARGS__);\
	}\
    virtual bool deserialize(ByteBuffer& buff){\
	    return mstd::deserialize(buff, ##__VA_ARGS__); \
	}\
    uint32_t size() {\
        return mstd::append_size(__VA_ARGS__);\
    }

