#pragma once
#include "lib/header/std.h"
#include <Windows.h>

class baselib
{
public:
    //错误弹窗并退出
    //error popup and exit
    static void error(const wchar_t* msg, const wchar_t* tit = TEXT("error"));

    //分割char（支持data）
    //split char (support data)
    static bool split_char(const char* buf, int len, char sep, const char*& left, int& left_len, const char*& right, int& right_len);

    //分割string（不支持data）
    //split string (no data support)
    static std::vector<std::string> split_string(const std::string& str, const char spl);

    //utf8转gbk
    //utf8 to gbk
    static std::string utf8_to_gbk(const std::string& utf8);
};
