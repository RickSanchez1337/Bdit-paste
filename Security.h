#pragma once
#define WIN32_LEAN_AND_MEAN

#define SECURITY_WIN32 

#include <Windows.h>
#include <stdint.h>
#include <mutex>
#include <iostream>
#include <vector>
#include <exception>
#include <memory>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <Psapi.h>
#include <cstdint>
#include "utils/xor.h"
#include <aes.h>
#include <base64.h>
#include <modes.h>
#include <intrin.h>
#include <iphlpapi.h>
#include <rpcndr.h>
#include <WinInet.h>
#pragma comment(lib, "wininet.lib")

class Security {
private:
    inline void get_file(const char* dllName, const char* fileName, char* buffer,
        int bfSize)
    {
        if (strlen(fileName) + strlen(buffer) < MAX_PATH) {
            char* pathEnd = strrchr(buffer, '\\');
            strcpy(pathEnd + 1, fileName);
        }
        else {
            *buffer = 0;
        }
    }
    inline std::string replace_all(std::string subject, const std::string& search,
        const std::string& replace)
    {
        size_t pos = 0;
        while ((pos = subject.find(search, pos)) != std::string::npos) {
            subject.replace(pos, search.length(), replace);
            pos += replace.length();
        }
        return subject;
    }

public:
    std::string version = "1.4.0";
    bool authenticated = true;

public:
    inline std::string geturl(const char* URL)
    {
		return "";
    }

    inline std::string encrypt(std::string str_in, std::string key,
        std::string iv)
    {
        return "";
    }
    inline std::string decrypt(char* str_in, char* key, char* iv)
    {
		return "";
    }

    inline std::string random_string(size_t length)
    {
        static std::string charset = _xor_("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890")
            .str();
        std::string result;
        result.resize(length);

        for (int i = 0; i < length; i++)
            result[i] = charset[rand() % charset.length()];

        return result;
    }
    inline char* random_char_arr(size_t length)
    {
        static std::string charset = _xor_("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890")
            .str();
        std::string result;
        result.resize(length);

        for (int i = 0; i < length; i++)
            result[i] = charset[rand() % charset.length()];

        return (char*)result.c_str();
    }

    inline bool check_version()
    {
        return true;
    }

    inline std::string GenerateResponse();

    inline bool authenticate(std::string username, std::string password)
    {
        return true;
    }
};

extern Security security;