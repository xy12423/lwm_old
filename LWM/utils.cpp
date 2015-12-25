#include "stdafx.h"
#include "utils.h"

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
		case 0:
			ret.push_back(encode64[*ptr >> 2]);
			ret.push_back(encode64[((*ptr << 4) | ptr[1] >> 4) & 0x3F]);
			ret.push_back(encode64[((ptr[1] << 2) | ptr[2] >> 6) & 0x3F]);
			ret.push_back(encode64[ptr[2] & 0x3F]);
			break;
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

int str2num(std::string str, size_t &ret)
{
	try
	{
		ret = std::stoul(str, 0);
		return 0;
	}
	catch (...) {}
	return -1;
}

void processEscChar(std::string &str)
{
	std::string::iterator itr = str.begin();
	for (; itr != str.end(); itr++)
	{
		if (*itr == '\\')
		{
			char replace = '\0';
			itr = str.erase(itr);
			if (itr == str.end())
				break;
			switch (*itr)
			{
				case 'a':
					replace = '\a';
					break;
				case 'b':
					replace = '\b';
					break;
				case 'f':
					replace = '\f';
					break;
				case 'n':
					replace = '\n';
					break;
				case 'r':
					replace = '\r';
					break;
				case 't':
					replace = '\t';
					break;
				case 'v':
					replace = '\v';
					break;
				case '\\':
					replace = '\\';
					break;
				case '\'':
					replace = '\'';
					break;
				case '\"':
					replace = '\"';
					break;
				case 'x':
				{
					std::stringstream tmp;
					tmp << std::hex;
					for (int i = 0; i < 2 && itr != str.end(); i++)
					{
						itr = str.erase(itr);
						if (!isxdigit(*itr))
							break;
						tmp << *itr;
					}
					itr = str.insert(itr, '\0');
					int tmpn = 0;
					tmp >> tmpn;
					replace = tmpn;
					break;
				}
				default:
				{
					if (*itr > '7' || *itr < '0')
						return;
					std::stringstream tmp;
					tmp << std::oct;
					for (int i = 0; i < 3 && itr != str.end(); i++)
					{
						if (*itr > '7' || *itr < '0')
							break;
						tmp << *itr;
						itr = str.erase(itr);
					}
					itr = str.insert(itr, '\0');
					int tmpn = 0;
					tmp >> tmpn;
					replace = tmpn;
				}
			}

			*itr = replace;
		}
	}
}

std::string toSingleLine(const std::string &str)
{
	std::string ret = str;
	size_t pos = ret.find('\n');
	while (pos != std::string::npos)
	{
		ret.replace(pos, 1, "\\n");
		pos = ret.find('\n', pos + 2);
	}
	return ret;
}

std::wstring toSingleLine(const std::wstring &str)
{
	std::wstring ret = str;
	size_t pos = ret.find('\n');
	while (pos != std::wstring::npos)
	{
		ret.replace(pos, 1, L"\\n");
		pos = ret.find('\n', pos + 2);
	}
	return ret;
}

