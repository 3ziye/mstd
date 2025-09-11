#ifdef MSTD_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#endif // MSTD_WINDOWS

#include "md5util.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include "md5.h"
namespace mstd
{
    bool md5_string(const std::string& str, std::string& md5)
    {
        MD5_CTX context;
        unsigned char digest[16];
        char szbuf[4];

        unsigned char* p = new unsigned char[str.length()];
        memcpy(p, str.c_str(), str.length());
        MD5Init(&context);
        MD5Update(&context, p, str.length());
        MD5Final(&context, digest);

        for (unsigned int i = 0; i < 16; i++)
        {
            sprintf(szbuf, "%02x", digest[i]);
            md5.append(szbuf);
        }

        delete[]p;
        return true;
    }


    std::string md5_string(const std::string& str)
    {
        MD5_CTX context;
        unsigned char digest[16];
        char szbuf[4];
        std::string md5;

        unsigned char* p = new unsigned char[str.length()];
        memcpy(p, str.c_str(), str.length());

        MD5Init(&context);
        MD5Update(&context, p, str.length());
        MD5Final(&context, digest);

        for (unsigned int i = 0; i < 16; i++)
        {
            sprintf(szbuf, "%02x", digest[i]);
            md5.append(szbuf);
        }

        delete[]p;
        return md5;
    }

    bool md5_file(const char* filename, std::string& md5)
    {
        FILE* file;
        MD5_CTX context;
        int len;
        unsigned char digest[16];
        unsigned char filebuf[1024];
        char szbuf[4];

        if ((file = fopen(filename, "rb")) == NULL)
            return false;
        else {
            MD5Init(&context);
            while ((len = fread(filebuf, 1, 1024, file)))
                MD5Update(&context, filebuf, len);
            MD5Final(&context, digest);
            fclose(file);
        }

        for (unsigned int i = 0; i < 16; i++)
        {
            sprintf(szbuf, "%02x", digest[i]);
            md5.append(szbuf);
        }
        return true;
    }

    std::string md5_file(const char* filename)
    {
        FILE* file;
        MD5_CTX context;
        int len;
        unsigned char digest[16];
        unsigned char filebuf[1024];
        char szbuf[4];
        std::string md5 = "";

        if ((file = fopen(filename, "rb")) == NULL)
            return md5;
        else {
            MD5Init(&context);
            while ((len = fread(filebuf, 1, 1024, file)))
                MD5Update(&context, filebuf, len);
            MD5Final(&context, digest);
            fclose(file);
        }

        for (unsigned int i = 0; i < 16; i++)
        {
            sprintf(szbuf, "%02x", digest[i]);
            md5.append(szbuf);
        }

        return md5;
    }

}