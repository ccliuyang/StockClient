#ifndef CONV_UTILS_H
#define CONV_UTILS_H

#include <codecvt>
#include <locale>
#include <string>

using namespace std;


wstring utf8_to_wstring(const string &str);
string wstring_to_utf8(const wstring &str);
wstring  gbk_to_wstring(const string &str);
string  wstring_to_gbk(const wstring &str);
string utf8_to_gbk(const string &str);
string gbk_to_utf8(const string &str);



#endif

