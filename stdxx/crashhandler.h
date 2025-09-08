#pragma once
#include <string>

namespace stdxx {

class crash_handler_private;
class crash_handler
{
public:
    static crash_handler* instance();
    void init(const std::string&  report_path, const std::string &app_path, const std::string &crash_reporter_fullpath);
    void set_report_crash_to_system(bool report);
    bool write_minidump();

private:
    crash_handler();
    ~crash_handler();
    crash_handler_private* d;
};

}

