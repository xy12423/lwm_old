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
	wxT("工作名"), wxT("详细信息"), wxT("应用更改"),

	wxT("请确认删除操作"), wxT("确认")
};

const char* scriptURL = "http://localhost/edit.php";

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
