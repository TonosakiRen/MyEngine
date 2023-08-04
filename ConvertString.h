#pragma once
#include <string>
#include <format>
void Log(const std::string& message);
void Log(const std::wstring& message);
std::wstring ConvertString(const std::string& str);
std::string ConvertString(const std::wstring& str);

//基本的な使い方
//wstring->string
//Log(ConvertString(std::format(L"WSTRING{}\n",wstringValue)))