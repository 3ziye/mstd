#ifndef _BASE_FSTREAM_H__
#define _BASE_FSTREAM_H__

#include <fstream>
#ifdef WIN32
#include "string_convert.h"
#endif

namespace stdxx {

template < class charT, class traits = std::char_traits<charT> >
class basic_ifstream : public std::basic_ifstream<charT,traits> {
private: // disallow copying
    basic_ifstream(const basic_ifstream &);
    const basic_ifstream & operator=(const basic_ifstream &); 
public:
    basic_ifstream() {}

    basic_ifstream(const std::string &path, std::ios_base::openmode mode = std::ios::in) {
#if defined(WIN32) && !defined(__MINGW32__)
      // call basic_ifstream(const wchar_t *
      std::basic_ifstream<charT,traits>::open(stdxx::to_wstring(path).c_str(), mode);
#else
      std::basic_ifstream<charT,traits>::open(path.c_str(), mode);
#endif
    }

#if defined(WIN32) && !defined(__MINGW32__)
    basic_ifstream(const std::wstring &path, std::ios_base::openmode mode = std::ios::in) {
        std::basic_ifstream<charT,traits>::open(path.c_str(), mode);
    }
#endif

    void open(const std::string &path) {
        open(path, std::ios::in);
    }
    void open(const std::string &path, std::ios_base::openmode mode) {
#ifdef WIN32
      // basic_ifstream(const wchar_t *
      std::basic_ifstream<charT,traits>::open(stdxx::to_wstring(path).c_str(), mode);
#else
      std::basic_ifstream<charT,traits>::open(path.c_str(), mode);
#endif
    }

    virtual ~basic_ifstream() {}
};

typedef basic_ifstream<char> ifstream;

template < class charT, class traits = std::char_traits<charT> >
class basic_ofstream : public std::basic_ofstream<charT,traits> {
private: // disallow copying
  basic_ofstream(const basic_ofstream &);
  const basic_ofstream & operator=(const basic_ofstream &); 
public:
  basic_ofstream() {}

  basic_ofstream(const std::string &path, std::ios_base::openmode mode = std::ios::out) {
#if defined(WIN32) && !defined(__MINGW32__)
    // basic_ofstream(const wchar_t *
    std::basic_ofstream<charT,traits>::open(stdxx::to_wstring(path).c_str(), mode);
#else
    std::basic_ofstream<charT,traits>::open(path.c_str(), mode);
#endif
  }

  void open(const std::string &path) {
    open(path, std::ios::in);
  }
  void open(const std::string &path, std::ios_base::openmode mode = std::ios::out) {
#ifdef WIN32
    // basic_ofstream(const wchar_t *
    std::basic_ofstream<charT,traits>::open(stdxx::to_wstring(path).c_str(), mode);
#else
    std::basic_ofstream<charT,traits>::open(path.c_str(), mode);
#endif
  }

  virtual ~basic_ofstream() {}
};

typedef basic_ofstream<char> ofstream;

} // stdxx
#endif // _BASE_FSTREAM_H__
