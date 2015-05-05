#include "stdafx.h"
#include "global.h"
#include "group.h"
#include "member.h"

grpListTp grpList;

enum group_lineN{
	LINE_GID,
	LINE_NAME,
	LINE_MEMBERS,
	LINE_END
};

errInfo readGrpList()
{
	dataBuf buf;
	CURL *handle = curl_easy_init();
	char *addCStr = str2cstr(scriptURL);
	char *errBuf = new char[2048];
	CURLcode success;
	curl_easy_setopt(handle, CURLOPT_URL, addCStr);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf);
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, "field=group&operation=list");
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

	std::for_each(grpList.begin(), grpList.end(), [](std::pair<size_t, group*> gPtr){
		delete gPtr.second;
	});
	grpList.clear();

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
					size_t gID, uID;
					if (str2num(data[LINE_GID], gID) != 0)
						return errInfo("GID:Not a num");
					group *newGroup = new group(gID, decode(data[LINE_NAME]));
					std::string name;
					for (p2 = data[LINE_MEMBERS].cbegin(), pEnd2 = data[LINE_MEMBERS].cend(); p2 != pEnd2; p2++)
					{
						if (*p2 == ';')
						{
							if (str2num(name, uID) != 0)
								return errInfo("UID:Not a num");
							newGroup->members.insert(uID);
							name.clear();
						}
						else
							name.push_back(*p2);
					}
					if (!name.empty())
					{
						if (str2num(name, uID) != 0)
							return errInfo("UID:Not a num");
						newGroup->members.insert(uID);
						name.clear();
					}

					grpList.emplace(gID, newGroup);

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
		return errInfo("Invalid group info from server");

	return errInfo();
}

#ifndef _LWM_NO_EDIT

errInfo newGrp(const std::wstring& name, group **ret)
{
	dataBuf buf;
	CURL *handle = curl_easy_init();
	char *addCStr = str2cstr(scriptURL);
	char *postField = str2cstr("field=group&operation=add&name=" + encode(name));
	char *errBuf = new char[2048];
	CURLcode success;
	curl_easy_setopt(handle, CURLOPT_URL, addCStr);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf);
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postField);
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
	*ret = new group(newID, name);
	grpList.emplace(newID, *ret);

	return errInfo();
}

errInfo delGrp(size_t gID)
{
	grpList.erase(gID);

	dataBuf buf;
	CURL *handle = curl_easy_init();
	char *addCStr = str2cstr(scriptURL);
	char *postField = str2cstr("field=group&operation=del&id=" + num2str(gID));
	char *errBuf = new char[2048];
	CURLcode success;
	curl_easy_setopt(handle, CURLOPT_URL, addCStr);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf);
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postField);
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

errInfo group::editName(const std::wstring &newName)
{
	name = newName;

	dataBuf buf;
	CURL *handle = curl_easy_init();
	char *addCStr = str2cstr(scriptURL);
	char *postField = str2cstr("field=group&operation=edit&id=" + num2str(gID) + "&item=name&value=" + encode(newName));
	char *errBuf = new char[2048];
	CURLcode success;
	curl_easy_setopt(handle, CURLOPT_URL, addCStr);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf);
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postField);
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

errInfo group::addMember(size_t uID)
{
	members.insert(uID);
	return writeMemList();
}

errInfo group::delMember(size_t uID)
{
	members.erase(uID);
	return writeMemList();
}

errInfo group::writeMemList()
{
	std::string newMemberStr;
	std::for_each(members.begin(), members.end(), [&newMemberStr](size_t uID){
		newMemberStr.append(num2str(uID));
		newMemberStr.push_back(';');
	});

	dataBuf buf;
	CURL *handle = curl_easy_init();
	char *addCStr = str2cstr(scriptURL);
	char *postField = str2cstr("field=group&operation=edit&id=" + num2str(gID) + "&item=member&value=" + encode(newMemberStr));
	char *errBuf = new char[2048];
	CURLcode success;
	curl_easy_setopt(handle, CURLOPT_URL, addCStr);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf);
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postField);
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

#endif
