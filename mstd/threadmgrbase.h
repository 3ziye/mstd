#pragma once
#include <string>
#include <mutex>

namespace mstd {
    class ThreadMgrBase
    {
    public:
        ThreadMgrBase(const char* name);
        virtual ~ThreadMgrBase();

    public:
        std::string Name() { return mgr_name_; }
        void RegisterAlive();
        void UnregisterAlive();

        virtual bool IsAlive(std::string& info) = 0;
    protected:
        std::string mgr_name_;

    private:
        static std::recursive_mutex mutex_;
        static bool alive_create_;
        static uint32_t mgr_count_;
    };

};
