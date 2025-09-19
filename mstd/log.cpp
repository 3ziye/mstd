#include "log.h"
#include <mutex>
#include <vector>
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <string.h>
#include "path_string.h"
#include "system.h"
#include "fileutils.h"
#include <assert.h>
#ifdef MSTD_WINDOWS
#include <filesystem>
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <time.h> 
#endif

namespace mstd
{
namespace log
{

const char* get_level_string(Level level) {
    switch (level)
    {
    case Panic:
        return "Panic";
    case Error:
        return "Error";
    case Warn:
        return "Warn";
    case Info:
        return "Info";
    case Debug:
        return "Debug";
    case Trace:
        return "Trace";
    default:
        return "Unknown";
    }
    return "null";
}

struct SystemTime
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int millisecond;
};

long get_timezone(void) 
{
#ifdef MSTD_WINDOWS
	TIME_ZONE_INFORMATION tmp;

	GetTimeZoneInformation(&tmp);
	return tmp.Bias * 60;
#else
	return timezone;
#endif
}

int is_leap_year(time_t year) 
{
    if (year % 4) return 0;         /* A year not divisible by 4 is not leap. */
    else if (year % 100) return 1;  /* If div by 4 and not 100 is surely leap. */
    else if (year % 400) return 0;  /* If div by 100 *and* not by 400 is not leap. */
    else return 1;                  /* If div by 100 and 400 is leap. */
}

void nolocks_localtime(struct tm *tmp, time_t t, time_t tz, int dst) 
{
    const time_t secs_min = 60;
    const time_t secs_hour = 3600;
    const time_t secs_day = 3600*24;

    t -= tz;                            /* Adjust for timezone. */
    t += 3600*dst;                      /* Adjust for daylight time. */
    time_t days = t / secs_day;         /* Days passed since epoch. */
    time_t seconds = t % secs_day;      /* Remaining seconds. */

    tmp->tm_isdst = dst;
    tmp->tm_hour = seconds / secs_hour;
    tmp->tm_min = (seconds % secs_hour) / secs_min;
    tmp->tm_sec = (seconds % secs_hour) % secs_min;

    /* 1/1/1970 was a Thursday, that is, day 4 from the POV of the tm structure
     * where sunday = 0, so to calculate the day of the week we have to add 4
     * and take the modulo by 7. */
    tmp->tm_wday = (days+4)%7;

    /* Calculate the current year. */
    tmp->tm_year = 1970;
    while(1) {
        /* Leap years have one day more. */
        time_t days_this_year = 365 + is_leap_year(tmp->tm_year);
        if (days_this_year > days) break;
        days -= days_this_year;
        tmp->tm_year++;
    }
    tmp->tm_yday = days;  /* Number of day of the current year. */

    /* We need to calculate in which month and day of the month we are. To do
     * so we need to skip days according to how many days there are in each
     * month, and adjust for the leap year that has one more day in February. */
    int mdays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    mdays[1] += is_leap_year(tmp->tm_year);

    tmp->tm_mon = 0;
    while(days >= mdays[tmp->tm_mon]) {
        days -= mdays[tmp->tm_mon];
        tmp->tm_mon++;
    }

    tmp->tm_mday = days+1;  /* Add 1 since our 'days' is zero-based. */
    tmp->tm_year -= 1900;   /* Surprisingly tm_year is year-1900. */
}

void gettimeofday_ex(timeval* tv, void* /*tz*/)
{
#ifdef MSTD_WINDOWS
#define EPOCHFILETIME (116444736000000000ULL)
	FILETIME ft;
	ULARGE_INTEGER li;
	uint64_t tt;

	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	tt = (li.QuadPart - EPOCHFILETIME) / 10;
	tv->tv_sec = tt / 1000000;
	tv->tv_usec = tt % 1000000;
#else
    gettimeofday(tv, NULL);
#endif
}

tm to_tm(time_t t, time_t tz)
{
    struct tm _tm;
    // 不采用夏令时
	nolocks_localtime(&_tm, t, tz, 0);
    return _tm;
}

SystemTime to_system_time(time_t t, time_t tz)
{
    tm _tm = to_tm(t, tz);

    return SystemTime {
        _tm.tm_year + 1900,
        _tm.tm_mon + 1,
        _tm.tm_mday,
        _tm.tm_hour,
        _tm.tm_min,
        _tm.tm_sec,
        0
    };
}

SystemTime get_system_time(time_t time_zone)
{
    std::chrono::system_clock::time_point now =
        std::chrono::system_clock::now();
    int milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count() % 1000;
    struct timeval tv;
    
    gettimeofday_ex(&tv, NULL);
	SystemTime system_time = to_system_time(tv.tv_sec, time_zone);
    system_time.millisecond = milliseconds;
    return system_time;
}

int vsnprintf(char *buffer, size_t size, const char *format, va_list ap)
{
#ifdef MSTD_WINDOWS  
    int pos = ::vsnprintf_s(buffer, size, _TRUNCATE, format, ap);
    return pos == -1 ? ::_vscprintf(format, ap) : pos;
#else         
    return ::vsnprintf(buffer, size, format, ap);
#endif        
}

template<typename CharT>
int snprintf(CharT *buffer, size_t size, const CharT *format, ...)
{
    va_list ap;
    va_start(ap, format);
    int pos = vsnprintf(buffer, size, format, ap);
    va_end(ap);
    return pos;
}

int sprintf_filename_suffix(char* buffer, int size, const SystemTime& system_time) {
    return snprintf(buffer, size, "%04u%02u%02u.log",
        system_time.year, system_time.month, system_time.day);
}

class Log
{
public:
    Log()
    {
//#if defined(WIN32) && defined(UNICODE)
//        static std::locale locale("");
//        std::wcout.imbue(locale);
//#endif
#ifdef MSTD_WINDOWS  
        _tzset();
#else
        tzset();
#endif
        timezone_ = get_timezone();

        log_directory_ = mstd::compose_path(mstd::get_program_dir<char>(), "log");
        prefix_name_ = mstd::get_last_path(mstd::get_process_path<char>());

        auto pos = prefix_name_.find_last_of('.');
        if (-1 != pos) {
            prefix_name_ = prefix_name_.substr(0, pos);
        }

        prefix_name_ += "_";

        level_ = Debug;
        storage_type_ = Screen | DiskFile;
    }

    ~Log() {
    }
public:
    void set_directory(const char* path) {
        log_directory_ = path;
        mstd::create_directorys(log_directory_.c_str());
    }

    void set_prefix_name(const char* prefix_name) {
        prefix_name_ = prefix_name;
        prefix_name_ += "_";
    }

    void set_level(unsigned int level) {
        level_ = level;
    }

    void set_storage_type(unsigned int storage_type) {
        storage_type_ = storage_type;
    }

    void set_reserve_days(int days) {
        reserve_days_ = days;
    }

    void print_v(Level level, const char* filename, unsigned int line,
        const char* function, const char* format, va_list ap) {

        if (level_ > level) return;

        // 日志时间
        SystemTime t = get_system_time(timezone_);
        // 初始化行头
        const int size = 2048;
        char buffer[size];

        int pos = snprintf(buffer, size, "[%02u:%02u:%02u.%03u]", t.hour,
            t.minute, t.second, t.millisecond);
        if (pos >= size) return;

        pos += snprintf(buffer + pos, size - pos, "[%04x]", mstd::get_current_thread_id());
        if (pos >= size) return;

        pos += snprintf(buffer + pos, size - pos, "[%s]", get_level_string(level));
        if (pos >= size) return;

        pos += snprintf(buffer + pos, size - pos, "[%s:%d]", mstd::get_last_path(filename).c_str(), line);
        if (pos >= size) return;

        //pos += snprintf(buffer + pos, size - pos, "[%s]", function);
        //if (pos >= size) return;

        pos += vsnprintf(buffer + pos, size - pos, format, ap);
        if (pos >= size) return;

        if (size - pos >= 1) {
            //buffer[pos++] = '\r';
            buffer[pos++] = '\n';
        }

        buffer[pos] = 0;
        write(buffer, pos, t);
    }

	std::string log_file_path() {
		return file_path_;
	}

private:
    void write(
        const char* message,
        int size,
        const SystemTime& system_time)
    {
        std::lock_guard<std::mutex> guard(lock_);
        if (storage_type_ & Screen)
            print_to_screen(message);
        if (storage_type_ & DiskFile)
            wirte_to_file(message, size, system_time);
    }

    void print_to_screen(const char* message) {
        std::cout << message;
    }

    void wirte_to_file(const char* message, int size, const SystemTime& system_time) {
        sprintf_filename_suffix(new_suffix_, sizeof(new_suffix_), system_time);
        if (strcmp(old_suffix_, new_suffix_) != 0) {
            clear_files();
            std::string path = get_log_filename(new_suffix_);
            file_.close();
            file_.open(path, std::ios::out | std::ios::app);
#ifdef MSTD_WINDOWS
            strcpy_s(old_suffix_, sizeof(old_suffix_), new_suffix_);
#else
            strcpy(old_suffix_, new_suffix_);
#endif
			file_path_ = path;
        }

        if (file_.good()) {
            file_.write((const char*)message, size);
            file_.flush();
        }
    }

    void clear_files() {
        time_t seconds = time(NULL) - reserve_days_ * 86400;
        for (seconds -= 24 * 3600;; seconds -= 24 * 3600) {
            std::string filename = get_log_filename(seconds);
            if (!mstd::delete_file(filename.c_str())) break;
        }
    }

private:
    std::string get_log_filename(const char* suffix) {
        return mstd::compose_path(log_directory_, prefix_name_ + suffix);
    }

    std::string get_log_filename(time_t t) {
		SystemTime system_time = to_system_time(t, timezone_);
        char suffix[20] = { 0 }; 
        sprintf_filename_suffix(suffix, sizeof(suffix), system_time);
        return get_log_filename(suffix);
    }

private:
    std::mutex lock_;
    unsigned int storage_type_ = StorageMask;
    unsigned int level_ = Trace;
    unsigned int reserve_days_ = 7;  // 日志保留天数，默认30天
    int filename_depth_ = 1;
    std::string log_directory_;
    std::string prefix_name_;
    char old_suffix_[20];
    char new_suffix_[20];
	std::string file_path_;
    std::ofstream file_;
    time_t timezone_;
};

Log _logger;

void print_v(
    Level level,
    const char*  filename,
    unsigned int line,
    const char*  function,
    const char*  format,
    va_list      ap)
{
    _logger.print_v(
        level,
        filename,
        line,
        function,
        format,
        ap);
}

void print(
    Level        level,
    const char*  filename,
    unsigned int line,
    const char*  function,
    const char*  format,
    ...)
{
    va_list ap;
    va_start(ap, format);
    print_v(level, filename, line, function, format, ap);
    va_end(ap);
}

void set_directory(const char* path){
    _logger.set_directory(path);
}

void set_prefix_name(const char* prefix_name){
    _logger.set_prefix_name(prefix_name);
}

void set_level(Level level){
    _logger.set_level(level);
}

//
void set_storage_type(Storage type) {
    _logger.set_storage_type(type);
}

void set_reserve_days(int days){
    _logger.set_reserve_days(days);
}

std::string log_file_path() {
	return _logger.log_file_path();
}

}  // namespace log
}  // namespace mstd
