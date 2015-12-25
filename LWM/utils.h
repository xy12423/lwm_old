#pragma once

#ifndef _H_ERRINFO
#define _H_ERRINFO

struct errInfo
{
	errInfo(){ err = false; }
	errInfo(std::string _info){ err = true; info = _info; }
	errInfo(const char *_info){ err = true; info = _info; }
	bool err;
	std::string info;
};

typedef unsigned char BYTE;

std::string encode(std::string str);
std::string encode(const std::wstring &str);
std::wstring decode(const std::string &str);

int str2num(std::string str, size_t &ret);
void processEscChar(std::string &str);
void toSingleLine(std::string &str);

#endif
