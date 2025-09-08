#pragma once
#include <atomic>
namespace stdxx {
    template<typename T>
    class control_block {
    public:
        control_block(T* ptr) : ptr_(ptr), ref_count_(1) {}

        void increment() { ++ref_count_; }
        void decrement() {
            if (--ref_count_ == 0) {
                delete ptr_;
                delete this;
            }
        }

        T *get() { return ptr_; }

    private:
        T* ptr_;
        std::atomic<int> ref_count_; // 使用std::atomic来处理多线程环境中的原子操作  
    };

    template <typename T>
    class shared_ptr {
    public:
        shared_ptr(T* ptr) : control_block_(new control_block<T>(ptr)) {
            control_block_->increment();
        }

        shared_ptr(const shared_ptr& other) : control_block_(other.control_block_) {
            if (control_block_) {
                control_block_->increment();
            }
        }

		shared_ptr(shared_ptr&& other){
			control_block_ = other.control_block_;
			other.control_block_ = nullptr;
		}

		shared_ptr() {
			control_block_ = nullptr;
		}

        ~shared_ptr() {
            if (control_block_) {
                control_block_->decrement();
            }
        }

        void reset(T *ptr = nullptr) {
            if (control_block_) {
                control_block_->decrement();
            }

            if (ptr) {
                control_block_ = new control_block<T>(ptr);
                control_block_->increment();
            }
        }

        T* get() {
			if (!control_block_) return nullptr;

            return control_block_->get();
        }

		T* get() const {
			if (!control_block_) return nullptr;

			return control_block_->get();
		}

        T* operator->() {
			if (!control_block_) return nullptr;

            return control_block_->get();
        }

		T* operator->() const {
			if (!control_block_) return nullptr;

			return control_block_->get();
		}

        T& operator*() {
            return *control_block_->get();
        }

		T& operator*() const {
			return *control_block_->get();
		}

		T* operator()() {
			if (!control_block_) return nullptr;

			return control_block_->get();
		}

		T* operator()() const {
			if (!control_block_) return nullptr;

			return control_block_->get();
		}

		shared_ptr<T>& operator=(const shared_ptr<T> &obj) {
			if (control_block_) {
				control_block_->decrement();
			}

			control_block_ = obj.control_block_;
			if (control_block_) control_block_->increment();

			return *this;
		}

		shared_ptr<T>& operator=(shared_ptr<T> &&obj) {
			if (control_block_) {
				control_block_->decrement();
			}

			control_block_ = obj.control_block_;
			obj.control_block_ = nullptr;
			return *this;
		}


		shared_ptr<T> shared_from_this() {
			return *this;
		}

		const shared_ptr<T>& shared_from_this() const {
			return *this;
		}
        // ... 其他成员函数，如operator*、operator->等 ...  

    private:
        control_block<T>* control_block_;
    };


	//template<typename T>
	//class enable_shared_from_this {
	//public:
	//	shared_ptr<T> shared_from_this() {
	//		return (shared_ptr<T>(this));
	//	}

	//	shared_ptr<const T> shared_from_this() const
	//	{	// return shared_ptr
	//		return (shared_ptr<const T>(this));
	//	}
	//};
};