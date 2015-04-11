#include "stdafx.h"
#include "global.h"

wxString guiStrData[guiStrCount] = {
	wxT("Live Work Manager"),

	wxT("组"), wxT("添加"), wxT("删除"), wxT("重命名"),
	wxT("人员"), wxT("显示无组人员"), wxT("搜索"), wxT("添加"), wxT("删除"),
	wxT("信息"),
	wxT("ID"), wxT("来源"), wxT("简介"), wxT("属于的组"),
	wxT("正在进行的工作"), wxT("添加"), wxT("删除"), wxT("编辑"),
	wxT("应用更改"),

	wxT("更改工作信息"),
	wxT("工作名"), wxT("详细信息"), wxT("应用更改")
};

const char* scriptURL = "http://xy12423.wicp.net:4825/edit.php";

int str2num(std::string str, size_t &ret, int type)
{
	static const int numLevel[] = {
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 5, 5, 5, 5, 5, 5,
		5, 5, 5, 5, 5, 5, 5, 5, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 5, 5, 5, 5, 5, 5, 5,
		5, 2, 2, 2, 2, 2, 2, 5, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 5, 5, 5, 5, 5, 5, 5,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5
	};
	size_t len = str.length();
	if (len < 1)
		return -1;
	std::stringstream ss;
	if (str.front() == '-')
	{
		return -1;
	}
	else if (str.front() == '+')
	{
		str.erase(0, 1);
		len--;
	}
	switch (type)
	{
		case 0:
			if (len == 0)
				return -1;
			if (str.back() == 'h' || str.back() == 'H')
			{
				str.erase(len - 1, 1);
				ss << std::hex << str;
				ss >> ret;
			}
			else if (str.front() == '0' && len > 1)
			{
				if (len > 2 && str[1] == 'x')
				{
					str.erase(0, 2);
					ss << std::hex << str;
					ss >> ret;
				}
				else
				{
					str.erase(0, 1);
					ss << std::oct << str;
					ss >> ret;
				}
			}
			else if (str.front() == '\'' && str.back() == '\'')
			{
				if (str.length() == 3)
					ret = (int)(str[1]);
				else
					return -1;
			}
			else
			{
				ss << std::dec << str;
				ss >> ret;
			}
			break;
		case 4:
			if (str.front() == '\'' && str.back() == '\'')
				str.erase(0, 1);
			ret = (int)(str[0]);
			break;
		case 3:
			ss << std::hex << str;
			ss >> ret;
			break;
		case 2:
			ss << std::dec << str;
			ss >> ret;
			break;
		case 1:
			ss << std::oct << str;
			ss >> ret;
			break;
	}
	return 0;
}

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
	dataBuf *myBuf = static_cast<dataBuf*>(userp);
	char *dataBuf = static_cast<char*>(buffer);
	size_t sizeAll = size * nmemb;
	for (size_t i = 0; i < sizeAll; i++)
	{
		myBuf->push_back(*dataBuf);
		dataBuf++;
	}
	return sizeAll;
}
