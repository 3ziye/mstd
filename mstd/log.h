#ifndef __LOG_H_
#define __LOG_H_

#include <stdarg.h>
#include <string>
namespace mstd {
    namespace log {
    /// 日志级别
    enum Level
    {
        Panic = 60000,         // panic level, call panic(): [PANIC]
        Error = 50000,         // error level: [ERROR]
        Warn  = 40000,         // warning level: [WARN]
        Info  = 30000,         // info level: [INFO]
        Debug = 20000,         // debug level: [DEBUG]
        Trace = 10000,         // trace level: [TRACE]
    };

    enum Storage {
        Screen = 0x01,
        DiskFile = 0x02,
        StorageMask = 0xFF,
    };

    void print_v(
        Level        level,
        const char*  filename,
        unsigned int line,
        const char*  function,
        const char*  format,
        va_list      ap);

    void print(
        Level        level,
        const char*  filename,
        unsigned int line,
        const char*  function,
        const char*  format,
        ...);

    //日志目录
    void set_directory(const char* path);
    //日志前缀
    void set_prefix_name(const char* prefix_name);
    //日志级别
    void set_level(Level level);
    //
    void set_storage_type(Storage type);
    //保留天数
    void set_reserve_days(int days);
	//
	std::string log_file_path();
    };
};

#define LOGFMTT(...) \
        mstd::log::print(mstd::log::Trace, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define LOGFMTI(...) \
        mstd::log::print(mstd::log::Info, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define LOGFMTE(...) \
        mstd::log::print(mstd::log::Error, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define LOGFMTW(...) \
        mstd::log::print(mstd::log::Warn, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#endif  // __LOG_H_
