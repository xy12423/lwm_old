#include "stdafx.h"
#include "global.h"

wxString guiStrData[guiStrCount] = {
	wxT("Live Work Manager"),

	wxT("��"), wxT("���"), wxT("ɾ��"), wxT("������"),
	wxT("��Ա"), wxT("��ʾ������Ա"), wxT("����"), wxT("���"), wxT("ɾ��"),
	wxT("��Ϣ"),
	wxT("ID"), wxT("��Դ"), wxT("���"), wxT("���ڵ���"),
	wxT("���ڽ��еĹ���"), wxT("���"), wxT("ɾ��"), wxT("�༭"),
	wxT("Ӧ�ø���"),

	wxT("���Ĺ�����Ϣ"),
	wxT("������"), wxT("��ϸ��Ϣ"), wxT("Ӧ�ø���")
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
