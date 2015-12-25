#include "stdafx.h"
#include "global.h"
#include "member.h"

memListTp memList;

enum member_lineN{
	LINE_UID,
	LINE_NAME,
	LINE_SRC,
	LINE_INFO,
	LINE_GROUPS,
	LINE_WORKS,
	LINE_END
};

errInfo readMemList()
{
	dataBuf buf;
	CURL *handle = curl_easy_init();
	std::unique_ptr<char[]> errBuf = std::make_unique<char[]>(2048);
	CURLcode success;
	curl_easy_setopt(handle, CURLOPT_URL, scriptURL);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf.get());
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, "field=member&operation=list");
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

	for (std::pair<size_t, member*> mPtr : memList)
		delete mPtr.second;
	memList.clear();

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
					size_t uID, gID, wID;
					if (str2num(data[LINE_UID], uID) != 0)
						return errInfo("UID:Not a num");
					processEscChar(data[LINE_INFO]);
					member *newMember = new member(uID, decode(data[LINE_NAME]), uExtInfo(decode(data[LINE_SRC]), decode(data[LINE_INFO])));

					std::string name = std::move(data[LINE_GROUPS]);
					if (!name.empty())
					{
						if (name.back() != ';')
							name.push_back(';');
						size_t pos1 = 0, pos2 = name.find(';');
						while (pos2 != std::string::npos)
						{
							if (str2num(name.substr(pos1, pos2 - pos1), gID) != 0)
								return errInfo("GID:Not a num");
							newMember->groups.insert(gID);
							pos1 = pos2 + 1;
							pos2 = name.find(';', pos1);
						}
					}

					name = std::move(data[LINE_WORKS]);
					if (!name.empty())
					{
						if (name.back() != ';')
							name.push_back(';');
						size_t pos1 = 0, pos2 = name.find(';');
						while (pos2 != std::string::npos)
						{
							if (str2num(name.substr(pos1, pos2 - pos1), gID) != 0)
								return errInfo("WID:Not a num");
							newMember->groups.insert(wID);
							pos1 = pos2 + 1;
							pos2 = name.find(';', pos1);
						}
					}

					memList.emplace(uID, newMember);

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
		return errInfo("Invalid member info from server");

	return errInfo();
}

errInfo newMem(const std::wstring &name, const uExtInfo &extInfo, member **ret)
{
	dataBuf buf;
	CURL *handle = curl_easy_init();
	std::string postField = "field=member&operation=add&name=" + encode(name) + "&src=" + encode(extInfo.src) + "&info=" + encode(toSingleLine(extInfo.info));
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
	*ret = new member(newID, name, extInfo);
	memList.emplace(newID, *ret);

	return errInfo();
}

errInfo delMem(size_t uID)
{
	memList.erase(uID);

	dataBuf buf;
	CURL *handle = curl_easy_init();
	std::string postField = "field=member&operation=del&id=" + std::to_string(uID);
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

#define doEDIT	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postField.c_str());	\
				success = curl_easy_perform(handle);	\
				if (success != CURLcode::CURLE_OK)	\
				{	\
					curl_easy_cleanup(handle);	\
					std::string err(errBuf.get());	\
					return errInfo("E:network:" + err);	\
				}

errInfo member::writeGrpList()
{
	std::string newGroupStr;
	for (size_t gID : groups)
	{
		newGroupStr.append(std::to_string(gID));
		newGroupStr.push_back(';');
	}

	dataBuf buf;
	CURL *handle = curl_easy_init();
	std::unique_ptr<char[]> errBuf = std::make_unique<char[]>(2048);
	curl_easy_setopt(handle, CURLOPT_URL, scriptURL);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf.get());
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &buf);

	std::string postField;
	CURLcode success;
	postField = "field=member&operation=edit&id=" + std::to_string(uID) + "&item=group&value=" + encode(newGroupStr);
	doEDIT;

	curl_easy_cleanup(handle);

	if (!buf.empty())
		return errInfo(std::string("E:Server side error:") + buf);

	return errInfo();
}

errInfo member::writeWorkList()
{
	std::string newWorkStr;
	for (size_t wID : works)
	{
		newWorkStr.append(std::to_string(wID));
		newWorkStr.push_back(';');
	}

	dataBuf buf;
	CURL *handle = curl_easy_init();
	std::unique_ptr<char[]> errBuf = std::make_unique<char[]>(2048);
	curl_easy_setopt(handle, CURLOPT_URL, scriptURL);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf.get());
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &buf);

	std::string postField;
	CURLcode success;
	postField = "field=member&operation=edit&id=" + std::to_string(uID) + "&item=work&value=" + encode(newWorkStr);
	doEDIT;

	curl_easy_cleanup(handle);

	if (!buf.empty())
		return errInfo(std::string("E:Server side error:") + buf);

	return errInfo();
}

errInfo member::applyEdit()
{
	std::string newWorkStr;
	for (size_t wID : works)
	{
		newWorkStr.append(std::to_string(wID));
		newWorkStr.push_back(';');
	}

	dataBuf buf;
	CURL *handle = curl_easy_init();
	std::unique_ptr<char[]> errBuf = std::make_unique<char[]>(2048);
	std::string postFieldPre = "field=member&operation=edit&id=" + std::to_string(uID);
	curl_easy_setopt(handle, CURLOPT_URL, scriptURL);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf.get());
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &buf);

	std::string postField;
	CURLcode success;

	postField = postFieldPre + "&item=name&value=" + encode(name);
	doEDIT;
	postField = postFieldPre + "&item=src&value=" + encode(extInfo.src);
	doEDIT;
	postField = postFieldPre + "&item=info&value=" + encode(toSingleLine(extInfo.info));
	doEDIT;

	curl_easy_cleanup(handle);

	if (!buf.empty())
		return errInfo(std::string("E:Server side error:") + buf);

	return errInfo();
}
