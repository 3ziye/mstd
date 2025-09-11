#ifndef __SINGLETION_H_
#define __SINGLETION_H_

#include <mutex>

namespace mstd
{
    template <typename T>
    class Singleton
    {
    private:
        Singleton();
        ~Singleton();

    public:
        static T& instance();
        static void release();

    private:
        static std::mutex mutex_;
        static T* instance_;
    };

    template <class T>
    std::mutex Singleton<T>::mutex_;

    template <class T>
    T* Singleton<T>::instance_ = NULL;

    template <class T>
    T& Singleton<T>::instance()
    {
        if (instance_ == NULL)
        {
            mutex_.lock();
            if (instance_ == NULL)
                instance_ = new T;
            mutex_.unlock();
        }
        return *instance_;
    }

    template <class T>
    void Singleton<T>::release()
    {
        if (instance_ != NULL)
        {
            mutex_.lock();
            if (instance_ != NULL)
            {
                delete instance_;
                instance_ = NULL;
            }
            mutex_.unlock();
        }
    }

} // namespace mstd

#endif // __SINGLETION_H_
