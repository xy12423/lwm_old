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
	std::unique_ptr<char[]> errBuf = std::make_unique<char[]>(2048);
	CURLcode success;
	curl_easy_setopt(handle, CURLOPT_URL, scriptURL);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf.get());
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, "field=group&operation=list");
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &buf);
	success = curl_easy_perform(handle);
	if (success != CURLcode::CURLE_OK)
	{
		curl_easy_cleanup(handle);
		std::string err(errBuf.get());
		return errInfo("E:network:" + err);
	}
	curl_easy_cleanup(handle);

	for (std::pair<size_t, group*> gPtr : grpList)
		delete gPtr.second;
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

					std::string name = std::move(data[LINE_MEMBERS]);
					if (!name.empty())
					{
						if (name.back() != ';')
							name.push_back(';');
						size_t pos1 = 0, pos2 = name.find(';');
						while (pos2 != std::string::npos)
						{
							if (str2num(name.substr(pos1, pos2 - pos1), uID) != 0)
								return errInfo("UID:Not a num");
							newGroup->members.insert(uID);
							pos1 = pos2 + 1;
							pos2 = name.find(';', pos1);
						}
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

errInfo newGrp(const std::wstring& name, group **ret)
{
	dataBuf buf;
	CURL *handle = curl_easy_init();
	std::string postField = "field=group&operation=add&name=" + encode(name);
	std::unique_ptr<char[]> errBuf = std::make_unique<char[]>(2048);
	CURLcode success;
	curl_easy_setopt(handle, CURLOPT_URL, scriptURL);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf.get());
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postField.c_str());
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &buf);
	success = curl_easy_perform(handle);
	if (success != CURLcode::CURLE_OK)
	{
		curl_easy_cleanup(handle);
		std::string err(errBuf.get());
		return errInfo("E:network:" + err);
	}
	curl_easy_cleanup(handle);

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
	std::string postField = "field=group&operation=del&id=" + std::to_string(gID);
	std::unique_ptr<char[]> errBuf = std::make_unique<char[]>(2048);
	CURLcode success;
	curl_easy_setopt(handle, CURLOPT_URL, scriptURL);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf.get());
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postField.c_str());
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &buf);
	success = curl_easy_perform(handle);
	if (success != CURLcode::CURLE_OK)
	{
		curl_easy_cleanup(handle);
		std::string err(errBuf.get());
		return errInfo("E:network:" + err);
	}
	curl_easy_cleanup(handle);

	if (!buf.empty())
		return errInfo(std::string("E:Server side error:") + buf);

	return errInfo();
}

errInfo group::editName(const std::wstring &newName)
{
	name = newName;

	dataBuf buf;
	CURL *handle = curl_easy_init();
	std::string postField = "field=group&operation=edit&id=" + std::to_string(gID) + "&item=name&value=" + encode(newName);
	std::unique_ptr<char[]> errBuf = std::make_unique<char[]>(2048);
	CURLcode success;
	curl_easy_setopt(handle, CURLOPT_URL, scriptURL);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf.get());
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postField.c_str());
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &buf);
	success = curl_easy_perform(handle);
	if (success != CURLcode::CURLE_OK)
	{
		curl_easy_cleanup(handle);
		std::string err(errBuf.get());
		return errInfo("E:network:" + err);
	}
	curl_easy_cleanup(handle);

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
	for (size_t uID : members)
	{
		newMemberStr.append(std::to_string(uID));
		newMemberStr.push_back(';');
	};

	dataBuf buf;
	CURL *handle = curl_easy_init();
	std::string postField = "field=group&operation=edit&id=" + std::to_string(gID) + "&item=member&value=" + encode(newMemberStr);
	std::unique_ptr<char[]> errBuf = std::make_unique<char[]>(2048);
	CURLcode success;
	curl_easy_setopt(handle, CURLOPT_URL, scriptURL);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf.get());
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postField.c_str());
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &buf);
	success = curl_easy_perform(handle);
	if (success != CURLcode::CURLE_OK)
	{
		curl_easy_cleanup(handle);
		std::string err(errBuf.get());
		return errInfo("E:network:" + err);
	}
	curl_easy_cleanup(handle);

	if (!buf.empty())
		return errInfo(std::string("E:Server side error:") + buf);

	return errInfo();
}
