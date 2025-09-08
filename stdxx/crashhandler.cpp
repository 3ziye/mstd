#include "crashhandler.h"
#include "platform_define.h"
#include "string_convert.h"
#if defined(STDXX_LINUX)
#include "client/linux/handler/exception_handler.h"
#elif defined(STDXX_WINDOWS)
#include "client/windows/handler/exception_handler.h"
#elif defined(STDXX_MACOS)
#include "client/mac/handler/exception_handler.h"
#endif

#ifndef WIN32
#include <unistd.h>
#endif
#include <wchar.h>

#define CRASHANDLER_MAX_PATH 2000

namespace stdxx {
class crash_handler_private
{
public:
    crash_handler_private()
    {
        hander_ = NULL;
    }
 
    ~crash_handler_private()
    {
        delete hander_;
    }
 
    void init_crash_handler(const std::string& dumpPath, const std::string &appPath, const std::string &appDir);
    static google_breakpad::ExceptionHandler* hander_;
    static bool breport_crash_to_system_;
    
};
 
google_breakpad::ExceptionHandler* crash_handler_private::hander_ = NULL;
bool crash_handler_private::breport_crash_to_system_ = false;
wchar_t crash_reporter_path[CRASHANDLER_MAX_PATH];
wchar_t application_path[CRASHANDLER_MAX_PATH];

/************************************************************************/
/* dump_callback                                                         */
/************************************************************************/
#if defined(STDXX_WINDOWS)
bool dump_callback(const wchar_t* _dump_dir, const wchar_t* _minidump_id, void* context, EXCEPTION_POINTERS* exinfo, MDRawAssertionInfo* assertion, bool success)
#elif defined(STDXX_LINUX)
bool dump_callback(const google_breakpad::MinidumpDescriptor &md, void *context, bool success)
#elif defined(STDXX_MACOS)
bool dump_callback(const char* _dump_dir, const char* _minidump_id, void *context, bool success)
#endif
{
	//UNUSED(context);
#if defined(STDXX_WINDOWS)
    //UNUSED(_dump_dir);
    //UNUSED(_minidump_id);
    //UNUSED(assertion);
    //UNUSED(exinfo);
#elif defined(STDXX_LINUX)
    //UNUSED(md);
#endif
    /*
    NO STACK USE, NO HEAP USE THERE !!!
    Creating QString's, using qDebug, etc. - everything is crash-unfriendly.
    */

#if defined(STDXX_WINDOWS)    
    wcscat_s( crash_reporter_path, L" \"");
    wcscat_s( crash_reporter_path, _dump_dir );
    wcscat_s( crash_reporter_path, L"\\" );
    wcscat_s( crash_reporter_path, _minidump_id );
    wcscat_s( crash_reporter_path, L".dmp\"" );
    wcscat_s( crash_reporter_path, L" \"");
    wcscat_s( crash_reporter_path, application_path );
    wcscat_s( crash_reporter_path, L"\"");

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof( si ) );
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWNORMAL;
    ZeroMemory( &pi, sizeof(pi) );

    if ( CreateProcess( NULL, crash_reporter_path, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ) )
    {
        CloseHandle( pi.hProcess );
        CloseHandle( pi.hThread );
        TerminateProcess( GetCurrentProcess(), 1 );
    }
#else // OSX & LINUX

   char reporterPath[CRASHANDLER_MAX_PATH];
   wcstombs(reporterPath, crash_reporter_path, CRASHANDLER_MAX_PATH);

   char aPath[CRASHANDLER_MAX_PATH];
   wcstombs(aPath, application_path, CRASHANDLER_MAX_PATH);

   char command[CRASHANDLER_MAX_PATH];

#if defined(STDXX_LINUX)
   strcpy(command, md.path());
#elif defined(STDXX_MACOS)
    strcpy(command, _dump_dir);
    strcat(command, "/");
    strcat(command, _minidump_id);
    strcat(command, ".dmp");
#endif

    pid_t pid = fork();
    if ( pid == -1 ) // fork failed
        return false;
    if ( pid == 0 )
    {
        // we are the fork
        execl( reporterPath,
            reporterPath,
            command,
            aPath,
            (char*) 0 );

        printf( "Error: Can't launch CrashReporter!\n" );
        return false;
    }
#endif

    return crash_handler_private::breport_crash_to_system_ ? success : true;
}
 
void crash_handler_private::init_crash_handler(const std::string& dump_path, const std::string& app_path, const std::string& crash_reporter_fullpath)
{
    if ( hander_ != NULL )
        return;

    wcscpy(application_path, stdxx::to_wstring(app_path).c_str());
    wcscpy(crash_reporter_path, stdxx::to_wstring(crash_reporter_fullpath).c_str());
 
#if defined(STDXX_WINDOWS)
    std::wstring pathAsStr = stdxx::to_wstring(dump_path);
    hander_ = new google_breakpad::ExceptionHandler(
        pathAsStr,
        /*FilterCallback*/ 0,
        dump_callback,
        /*context*/
        0,
        true
        );
#elif defined(STDXX_LINUX)
    std::string pathAsStr = dump_path;
    google_breakpad::MinidumpDescriptor md(pathAsStr);
    pHandler = new google_breakpad::ExceptionHandler(
        md,
        /*FilterCallback*/ 0,
		dump_callback,
        /*context*/ 0,
        true,
        -1
        );
#elif defined(STDXX_MACOS)
    std::string pathAsStr = dump_path;
    pHandler = new google_breakpad::ExceptionHandler(
        pathAsStr,
        /*FilterCallback*/ 0,
		dump_callback,
        /*context*/
        0,
        true,
        NULL
        );
#endif
}
 
/************************************************************************/
/* crash_handler                                                         */
/************************************************************************/
crash_handler* crash_handler::instance()
{
    static crash_handler globalHandler;
    return &globalHandler;
}
 
crash_handler::crash_handler()
{
    d = new crash_handler_private();
}
 
crash_handler::~crash_handler()
{
    delete d;
}
 
void crash_handler::set_report_crash_to_system(bool report)
{
    d->breport_crash_to_system_ = report;
}
 
bool crash_handler::write_minidump()
{
    bool res = d->hander_->WriteMinidump();
    return res;
}
 
void crash_handler::init( const std::string& report_path,  const std::string& app_path, const std::string& crash_reporter_fullpath )
{
    d->init_crash_handler(report_path, app_path, crash_reporter_fullpath);
}
}