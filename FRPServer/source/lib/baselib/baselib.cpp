#include "baselib.h"

void baselib::error(const wchar_t* msg, const wchar_t* tit)
{
	MessageBox(nullptr, msg, tit, MB_OK);
	exit(-1);
}

bool baselib::split_char(const char* buf, int len, char sep, const char*& left, int& left_len, const char*& right, int& right_len)
{
    for (int i = 0; i < len; i++)
    {
        if (buf[i] == sep)
        {
            left = buf;
            left_len = i;
            right = buf + i + 1;
            right_len = len - i - 1;
            return true;
        }
    }
    return false;
}

std::vector<std::string> baselib::split_string(const std::string& str, const char spl)
{
    std::vector<std::string> value;
    std::string left_str = str;
    while (true)
    {
        size_t pos = left_str.find(spl);
        if (pos == std::string::npos)
        {
            value.emplace_back(left_str);
            break;
        }
        const std::string sub = left_str.substr(0, pos);
        value.emplace_back(sub);
        left_str = left_str.substr(pos + 1);
    }

    return value;
}

std::string baselib::utf8_to_gbk(const std::string& utf8)
{
    int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), (int)utf8.size(), nullptr, 0);
    std::wstring wbuf(wlen, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), (int)utf8.size(), &wbuf[0], wlen);

    int glen = WideCharToMultiByte(CP_ACP, 0, wbuf.c_str(), wlen, nullptr, 0, nullptr, nullptr);
    std::string gbk(glen, 0);
    WideCharToMultiByte(CP_ACP, 0, wbuf.c_str(), wlen, &gbk[0], glen, nullptr, nullptr);

    return gbk;
}
