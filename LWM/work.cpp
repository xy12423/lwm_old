#include "stdafx.h"
#include "global.h"
#include "work.h"
#include "member.h"

workListTp workList;

enum group_lineN{
	LINE_WID,
	LINE_NAME,
	LINE_INFO,
	LINE_MEMBERS,
	LINE_END
};

errInfo readWorkList()
{
	dataBuf buf;
	CURL *handle = curl_easy_init();
	char *addCStr = str2cstr(scriptURL);
	char *errBuf = new char[2048];
	CURLcode success;
	curl_easy_setopt(handle, CURLOPT_URL, addCStr);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf);
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, "field=work&operation=list");
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &buf);
	success = curl_easy_perform(handle);
	if (success != CURLcode::CURLE_OK)
	{
		curl_easy_cleanup(handle);
		std::string err(errBuf);
		delete[] errBuf;
		return errInfo("E:network:" + err);
	}
	curl_easy_cleanup(handle);
	delete[] errBuf;

	std::for_each(workList.begin(), workList.end(), [](std::pair<size_t, work*> wPtr){
		delete wPtr.second;
	});
	workList.clear();

	dataBuf::const_iterator p, pEnd = buf.cend();
	std::string::const_iterator p2, pEnd2;
	std::string data[LINE_END];
	int state = 0;
	for (p = buf.cbegin(); p != pEnd;)
	{
		switch (*p)
		{
			case '\r':
				p++;
			case '\n':
			{
				p++;

				state++;
				if (state == LINE_END)
				{
					size_t wID, uID;
					if (str2num(data[LINE_WID], wID) != 0)
						return errInfo("WID:Not a num");
					work *newWork = new work(wID, decode(data[LINE_NAME]), decode(data[LINE_INFO]));
					std::string name;
					for (p2 = data[LINE_MEMBERS].cbegin(), pEnd2 = data[LINE_MEMBERS].cend(); p2 != pEnd2; p2++)
					{
						if (*p2 == ';')
						{
							if (str2num(name, uID) != 0)
								return errInfo("UID:Not a num");
							newWork->members.insert(uID);
							name.clear();
						}
						else
							name.push_back(*p2);
					}
					if (!name.empty())
					{
						if (str2num(name, uID) != 0)
							return errInfo("UID:Not a num");
						newWork->members.insert(uID);
						name.clear();
					}

					workList.emplace(wID, newWork);

					for (int i = 0; i < LINE_END; i++)
						data[i].clear();
					state = 0;
				}
				break;
			}
			default:
				data[state].push_back(*p);
				p++;
		}
	}

	if (state != 0)
		return errInfo("Invalid work info from server");

	return errInfo();
}

errInfo newWork(const std::wstring &name, const std::wstring &info, work **ret)
{
	dataBuf buf;
	CURL *handle = curl_easy_init();
	char *addCStr = str2cstr(scriptURL);
	std::string postField = "field=work&operation=add&name=" + encode(name) + "&info=" + encode(info);
	char *errBuf = new char[2048];
	CURLcode success;
	curl_easy_setopt(handle, CURLOPT_URL, addCStr);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf);
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postField.c_str());
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &buf);
	success = curl_easy_perform(handle);
	if (success != CURLcode::CURLE_OK)
	{
		curl_easy_cleanup(handle);
		std::string err(errBuf);
		delete[] errBuf;
		return errInfo("E:network:" + err);
	}
	curl_easy_cleanup(handle);
	delete[] errBuf;

	size_t newID;
	if (str2num(buf, newID) != 0)
		return errInfo(std::string("E:Server side error:") + buf);
	*ret = new work(newID, name, info);
	workList.emplace(newID, *ret);

	return errInfo();
}

errInfo delWork(size_t wID)
{
	workList.erase(wID);

	dataBuf buf;
	CURL *handle = curl_easy_init();
	char *addCStr = str2cstr(scriptURL);
	std::string postField = "field=work&operation=del&id=" + num2str(wID);
	char *errBuf = new char[2048];
	CURLcode success;
	curl_easy_setopt(handle, CURLOPT_URL, addCStr);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf);
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postField.c_str());
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &buf);
	success = curl_easy_perform(handle);
	if (success != CURLcode::CURLE_OK)
	{
		curl_easy_cleanup(handle);
		std::string err(errBuf);
		delete[] errBuf;
		return errInfo("E:network:" + err);
	}
	curl_easy_cleanup(handle);
	delete[] errBuf;

	if (!buf.empty())
		return errInfo(std::string("E:Server side error:") + buf);

	return errInfo();
}

errInfo work::editName(const std::wstring &newName)
{
	name = newName;

	dataBuf buf;
	CURL *handle = curl_easy_init();
	char *addCStr = str2cstr(scriptURL);
	std::string postField = "field=work&operation=edit&id=" + num2str(wID) + "&item=name&value=" + encode(newName);
	char *errBuf = new char[2048];
	CURLcode success;
	curl_easy_setopt(handle, CURLOPT_URL, addCStr);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf);
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postField.c_str());
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &buf);
	success = curl_easy_perform(handle);
	if (success != CURLcode::CURLE_OK)
	{
		curl_easy_cleanup(handle);
		std::string err(errBuf);
		delete[] errBuf;
		return errInfo("E:network:" + err);
	}
	curl_easy_cleanup(handle);
	delete[] errBuf;

	if (!buf.empty())
		return errInfo(std::string("E:Server side error:") + buf);

	return errInfo();
}

errInfo work::editInfo(const std::wstring &newInfo)
{
	info = newInfo;

	dataBuf buf;
	CURL *handle = curl_easy_init();
	char *addCStr = str2cstr(scriptURL);
	std::string postField = "field=work&operation=edit&id=" + num2str(wID) + "&item=info&value=" + encode(newInfo);
	char *errBuf = new char[2048];
	CURLcode success;
	curl_easy_setopt(handle, CURLOPT_URL, addCStr);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf);
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postField.c_str());
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &buf);
	success = curl_easy_perform(handle);
	if (success != CURLcode::CURLE_OK)
	{
		curl_easy_cleanup(handle);
		std::string err(errBuf);
		delete[] errBuf;
		return errInfo("E:network:" + err);
	}
	curl_easy_cleanup(handle);
	delete[] errBuf;

	if (!buf.empty())
		return errInfo(std::string("E:Server side error:") + buf);

	return errInfo();
}

errInfo work::addMember(size_t uID)
{
	members.insert(uID);
	return writeMemList();
}

errInfo work::delMember(size_t uID)
{
	members.erase(uID);
	return writeMemList();
}

errInfo work::writeMemList()
{
	std::string newMemberStr;
	std::for_each(members.begin(), members.end(), [&newMemberStr](size_t uID){
		newMemberStr.append(num2str(uID));
		newMemberStr.push_back(';');
	});

	dataBuf buf;
	CURL *handle = curl_easy_init();
	char *addCStr = str2cstr(scriptURL);
	std::string postField = "field=work&operation=edit&id=" + num2str(wID) + "&item=member&value=" + encode(newMemberStr);
	char *errBuf = new char[2048];
	CURLcode success;
	curl_easy_setopt(handle, CURLOPT_URL, addCStr);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf);
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postField.c_str());
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &buf);
	success = curl_easy_perform(handle);
	if (success != CURLcode::CURLE_OK)
	{
		curl_easy_cleanup(handle);
		std::string err(errBuf);
		delete[] errBuf;
		return errInfo("E:network:" + err);
	}
	curl_easy_cleanup(handle);
	delete[] errBuf;

	if (!buf.empty())
		return errInfo(std::string("E:Server side error:") + buf);

	return errInfo();
}
