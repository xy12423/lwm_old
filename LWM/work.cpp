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
	std::unique_ptr<char[]> errBuf = std::make_unique<char[]>(2048);
	CURLcode success;
	curl_easy_setopt(handle, CURLOPT_URL, scriptURL);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf.get());
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, "field=work&operation=list");
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

	for (std::pair<size_t, work*> wPtr : workList)
		delete wPtr.second;
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
					processEscChar(data[LINE_INFO]);
					work *newWork = new work(wID, decode(data[LINE_NAME]), decode(data[LINE_INFO]));

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
							newWork->members.insert(uID);
							pos1 = pos2 + 1;
							pos2 = name.find(';', pos1);
						}
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
	std::string postField = "field=work&operation=add&name=" + encode(name) + "&info=" + encode(toSingleLine(info));
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
	*ret = new work(newID, name, info);
	workList.emplace(newID, *ret);

	return errInfo();
}

errInfo delWork(size_t wID)
{
	workList.erase(wID);

	dataBuf buf;
	CURL *handle = curl_easy_init();
	std::string postField = "field=work&operation=del&id=" + std::to_string(wID);
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

errInfo work::editName(const std::wstring &newName)
{
	name = newName;

	dataBuf buf;
	CURL *handle = curl_easy_init();
	std::string postField = "field=work&operation=edit&id=" + std::to_string(wID) + "&item=name&value=" + encode(newName);
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

errInfo work::editInfo(const std::wstring &newInfo)
{
	info = newInfo;

	dataBuf buf;
	CURL *handle = curl_easy_init();
	std::string postField = "field=work&operation=edit&id=" + std::to_string(wID) + "&item=info&value=" + encode(toSingleLine(newInfo));
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
	for (size_t uID : members)
	{
		newMemberStr.append(std::to_string(uID));
		newMemberStr.push_back(';');
	}

	dataBuf buf;
	CURL *handle = curl_easy_init();
	std::string postField = "field=work&operation=edit&id=" + std::to_string(wID) + "&item=member&value=" + encode(newMemberStr);
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
