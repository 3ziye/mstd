#ifndef BYTE_BUFFER_H__
#define BYTE_BUFFER_H__

#include <vector>
#include <cstring>
#include <cassert>
#include "platform.h"
class ByteBuffer
{
public:
    static const size_t init_size_ = 4 * 1024;
    static const size_t send_size_ = 4 * 1024;
    static const size_t recv_size_ = 4 * 1024;

public:
    ByteBuffer() : wpos_(0), rpos_(0), inner_(true) {
        storage_ = (char*)malloc(init_size_);
        storage_size_ = init_size_;
    }

	explicit ByteBuffer(size_t size) : wpos_(0), rpos_(0), inner_(true){
        storage_ = (char*)malloc(size);
        storage_size_ = size;
    }

	ByteBuffer(const ByteBuffer& rhs) : wpos_(rhs.wpos_), rpos_(rhs.rpos_), inner_(true) {
        storage_ = (char*)malloc(rhs.storage_size_);
        memcpy(storage_, rhs.storage_, rhs.storage_size_);
        storage_size_ = rhs.storage_size_;
    }

	ByteBuffer(ByteBuffer&& rhs) 
        : wpos_(rhs.wpos_), rpos_(rhs.rpos_), inner_(rhs.inner_),
        storage_(rhs.storage_), storage_size_(rhs.storage_size_) {

        rhs.rpos_ = rhs.wpos_ = 0;
        rhs.storage_ = nullptr;
        rhs.storage_size_ = 0;
        rhs.inner_ = false;
    }

	ByteBuffer(const char *data, const size_t size, const bool alloc_by_inner = true)
        : wpos_(0), rpos_(0), inner_(alloc_by_inner) {
        if (inner_) {
            storage_ = (char*)malloc(size);
            memcpy(storage_, data, size);
        } else {
            storage_ = const_cast<char*>(data);
        }

        storage_size_ = size;
		writeBytes(size);
	}

    ~ByteBuffer() {
        if (inner_ && storage_ != nullptr) {
            free(storage_);
        }

        storage_ = nullptr;
        storage_size_ = 0;
    }

	ByteBuffer& operator=(const ByteBuffer& rhs) {
        if (inner_ && storage_ != nullptr) {
            free(storage_);
        }

        storage_ = (char*)malloc(rhs.storage_size_);
        memcpy(storage_, rhs.storage_, rhs.storage_size_);
        storage_size_ = rhs.storage_size_;
        inner_ = true;
        rpos_ = rhs.rpos_;
        wpos_ = rhs.wpos_;
		return *this;
	}

	ByteBuffer& operator=(ByteBuffer&& rhs) {
        if (inner_ && storage_ != nullptr) {
            free(storage_);
        }

        inner_ = rhs.inner_;
        storage_ = rhs.storage_;
        storage_size_ = rhs.storage_size_;
        rpos_ = rhs.rpos_;
        wpos_ = rhs.wpos_;

        rhs.storage_ = nullptr;
        rhs.storage_size_ = 0;
        rhs.inner_ = false;
        rhs.rpos_ = rhs.wpos_ = 0;
		return *this;
	}

	bool Read(void *buf, uint32_t size) {
		if (Size() < size) {
			return false;
		}

        memcpy(buf, readBegin(), size);
		readBytes(size);
		return true;
	}

    template<typename T>
    bool Read(T &t) {
        if (Size() < sizeof(T)) {
            return false;
        }

        t = *(T*)readBegin();
        readBytes(sizeof(T));
        t = mstd::ntoh(t);
        return true;
    }

	template<typename T>
	void Write(const T& t) {
        T tmp = mstd::hton(t);
        Write((char*)&tmp, sizeof(T));
	}

	void Write(const char* data, std::size_t size) {
		if (size > 0 && inner_) {
			estimateSize(size);
			memcpy(writeBegin(), data, size);
			writeBytes(size);
		}
	}

    void ResetRead() { rpos_ = 0; }
	void Reset() { wpos_ = 0; rpos_ = 0; }
    size_t Size() const { return wpos_ - rpos_; }
private:
    char* data() { return storage_; }
    char* readBegin() { return storage_ + rpos_; }
    char* writeBegin() { return data() + wpos_; }

	void readBytes(size_t bytes) { rpos_ += bytes; }
	void writeBytes(size_t bytes) { wpos_ += bytes; }

    
    size_t validSize() const { return storage_size_ - wpos_; }
    size_t capacity() const { return storage_size_; }
    void addCapacity(size_t bytes) { 
        if (inner_) {
            storage_ = (char*)realloc(storage_, storage_size_ + bytes);
            storage_size_ += bytes;
            assert(storage_);
        }
    }

	void Normalize() {
		if (rpos_ > 0 && inner_) {
			if (rpos_ != wpos_) {
				memmove(data(), readBegin(), Size());
			}
			wpos_ -= rpos_;
			rpos_ = 0;

            //如果可写容量超过初始化的5倍，进行缩容量
            if ((validSize() > init_size_ * 5) && (Size() < init_size_)) {
                char *tmp = (char*)malloc(init_size_);
                if (wpos_)
                    memcpy(tmp, storage_, wpos_);
                free(storage_);
                storage_ = tmp;
                storage_size_ = init_size_;
            }
		}
	}

	void estimateSize(size_t size) {
		if (validSize() < size && inner_) {
            storage_size_ = (storage_size_ + size) * 3 / 2;
            storage_ = (char*)realloc(storage_, storage_size_);
            assert(storage_);
		}
	}

	void swap(ByteBuffer& rhs) {
        if (inner_ && storage_ != nullptr) {
            free(storage_);
        }

        inner_ = rhs.inner_;
        storage_ = rhs.storage_;
        storage_size_ = rhs.storage_size_;
        rpos_ = rhs.rpos_;
        wpos_ = rhs.wpos_;

        rhs.storage_ = nullptr;
        rhs.storage_size_ = 0;
        rhs.inner_ = false;
        rhs.rpos_ = rhs.wpos_ = 0;
	}

	ByteBuffer Slice(size_t start, size_t end) {
		ByteBuffer temp(this->storage_ + start, end - start, false);
		return temp;
	}

private:
	size_t  wpos_;
    size_t  rpos_;
	char*   storage_;
    size_t  storage_size_;
    bool    inner_;
};
#endif // BYTE_BUFFER_H__

