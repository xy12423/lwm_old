#include "stdafx.h"
#include "utils.h"

char* str2cstr(std::string arg)
{
	char* ret = new char[arg.size() + 1];
#ifdef _MSC_VER
	strcpy_s(ret, arg.size() + 1, arg.c_str());
#else
	strcpy(ret, arg.c_str());
#endif
	return ret;
}

std::string num2str(long long n)
{
	static std::stringstream sstr;
	std::string ret;
	sstr.clear();
	sstr << n;
	sstr >> ret;
	return ret;
}

const char encode64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
const char decode64[] = {
	-1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, 62, -1, -1,
	52, 53, 54, 55, 56, 57, 58, 59,
	60, 61, -1, -1, -1, -1, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,
	 7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22,
	23, 24, 25, -1, -1, -1, -1, 63,
	-1, 26, 27, 28, 29, 30, 31, 32,
	33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48,
	49, 50, 51, -1, -1, -1, -1, -1
};
const char space64 = '|';

void encode(std::string &ret, BYTE* buf, int size)
{
	BYTE *ptr_end = buf + size - 3;
	BYTE *ptr = buf;
	for (; ptr < ptr_end; ptr += 3)
	{
		ret.push_back(encode64[*ptr >> 2]);
		ret.push_back(encode64[((*ptr << 4) | ptr[1] >> 4) & 0x3F]);
		ret.push_back(encode64[((ptr[1] << 2) | ptr[2] >> 6) & 0x3F]);
		ret.push_back(encode64[ptr[2] & 0x3F]);
	}
	switch (ptr - ptr_end)
	{
		case 1:
			ret.push_back(encode64[*ptr >> 2]);
			ret.push_back(encode64[((*ptr << 4) | ptr[1] >> 4) & 0x3F]);
			ret.push_back(encode64[(ptr[1] << 2) & 0x3F]);
			ret.push_back(space64);
			break;
		case 2:
			ret.push_back(encode64[*ptr >> 2]);
			ret.push_back(encode64[(*ptr << 4) & 0x3F]);
			ret.push_back(space64);
			ret.push_back(space64);
			break;
	}
}

std::string encode(std::string str)
{
	std::string ret;
	encode(ret, (BYTE *)(str.c_str()), str.length());
	return ret;
}

std::string encode(const std::wstring &str)
{
	std::string ret;
	std::string buf(wxConvUTF8.cWC2MB(str.c_str()));
	encode(ret, (BYTE *)(buf.c_str()), buf.length());
	return ret;
}

std::wstring decode(const std::string &str)
{
	wxWCharBuffer buf = wxConvUTF8.cMB2WC(str.c_str());
	if (buf.length() == 0)
		return std::wstring();
	return std::wstring(buf);
}
