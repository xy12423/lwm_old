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
	char *addCStr = str2cstr(scriptURL);
	char *errBuf = new char[2048];
	CURLcode success;
	curl_easy_setopt(handle, CURLOPT_URL, addCStr);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf);
	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, "field=member&operation=list");
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

	std::for_each(memList.begin(), memList.end(), [](std::pair<size_t, member*> mPtr){
		delete mPtr.second;
	});
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
					member *newMember = new member(uID, decode(data[LINE_NAME]), uExtInfo(decode(data[LINE_SRC]), decode(data[LINE_INFO])));
					std::string name;

					for (p2 = data[LINE_GROUPS].cbegin(), pEnd2 = data[LINE_GROUPS].cend(); p2 != pEnd2; p2++)
					{
						if (*p2 == ';')
						{
							if (str2num(name, gID) != 0)
								return errInfo("GID:Not a num");
							newMember->groups.insert(gID);
							name.clear();
						}
						else
							name.push_back(*p2);
					}
					if (!name.empty())
					{
						if (str2num(name, gID) != 0)
							return errInfo("GID:Not a num");
						newMember->groups.insert(gID);
						name.clear();
					}

					for (p2 = data[LINE_WORKS].cbegin(), pEnd2 = data[LINE_WORKS].cend(); p2 != pEnd2; p2++)
					{
						if (*p2 == ';')
						{
							if (str2num(name, wID) != 0)
								return errInfo("WID:Not a num");
							newMember->works.insert(wID);
							name.clear();
						}
						else
							name.push_back(*p2);
					}
					if (!name.empty())
					{
						if (str2num(name, wID) != 0)
							return errInfo("WID:Not a num");
						newMember->works.insert(wID);
						name.clear();
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

#ifndef _LWM_NO_EDIT

errInfo newMem(const std::wstring &name, const uExtInfo &extInfo, member **ret)
{
	dataBuf buf;
	CURL *handle = curl_easy_init();
	char *addCStr = str2cstr(scriptURL);
	char *postField = str2cstr("field=member&operation=add&name=" + encode(name) + "&src=" + encode(extInfo.src) + "&info=" + encode(extInfo.info));
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
	*ret = new member(newID, name, extInfo);
	memList.emplace(newID, *ret);

	return errInfo();
}

errInfo delMem(size_t uID)
{
	memList.erase(uID);

	dataBuf buf;
	CURL *handle = curl_easy_init();
	char *addCStr = str2cstr(scriptURL);
	char *postField = str2cstr("field=member&operation=del&id=" + num2str(uID));
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

#define doEDIT	curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postField.c_str());	\
				success = curl_easy_perform(handle);	\
				if (success != CURLcode::CURLE_OK)	\
				{	\
					curl_easy_cleanup(handle);	\
					std::string err(errBuf);	\
					delete[] errBuf;	\
					return errInfo("E:network:" + err);	\
				}

errInfo member::writeGrpList()
{
	std::string newGroupStr;
	std::for_each(groups.begin(), groups.end(), [&newGroupStr](size_t gID){
		newGroupStr.append(num2str(gID));
		newGroupStr.push_back(';');
	});

	dataBuf buf;
	CURL *handle = curl_easy_init();
	char *addCStr = str2cstr(scriptURL);
	char *errBuf = new char[2048];
	curl_easy_setopt(handle, CURLOPT_URL, addCStr);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf);
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &buf);

	std::string postField;
	CURLcode success;
	postField = "field=member&operation=edit&id=" + num2str(uID) + "&item=group&value=" + encode(newGroupStr);
	doEDIT;

	curl_easy_cleanup(handle);
	delete[] errBuf;

	if (!buf.empty())
		return errInfo(std::string("E:Server side error:") + buf);

	return errInfo();
}

errInfo member::writeWorkList()
{
	std::string newWorkStr;
	std::for_each(works.begin(), works.end(), [&newWorkStr](size_t wID){
		newWorkStr.append(num2str(wID));
		newWorkStr.push_back(';');
	});

	dataBuf buf;
	CURL *handle = curl_easy_init();
	char *addCStr = str2cstr(scriptURL);
	char *errBuf = new char[2048];
	curl_easy_setopt(handle, CURLOPT_URL, addCStr);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf);
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &buf);

	std::string postField;
	CURLcode success;
	postField = "field=member&operation=edit&id=" + num2str(uID) + "&item=work&value=" + encode(newWorkStr);
	doEDIT;

	curl_easy_cleanup(handle);
	delete[] errBuf;

	if (!buf.empty())
		return errInfo(std::string("E:Server side error:") + buf);

	return errInfo();
}

errInfo member::applyEdit()
{
	std::string newWorkStr;
	std::for_each(works.begin(), works.end(), [&newWorkStr](size_t wID){
		newWorkStr.append(num2str(wID));
		newWorkStr.push_back(';');
	});

	dataBuf buf;
	CURL *handle = curl_easy_init();
	char *addCStr = str2cstr(scriptURL);
	char *errBuf = new char[2048];
	std::string postFieldPre = "field=member&operation=edit&id=" + num2str(uID);
	curl_easy_setopt(handle, CURLOPT_URL, addCStr);
	curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, errBuf);
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &buf);

	std::string postField;
	CURLcode success;

	postField = postFieldPre + "&item=name&value=" + encode(name);
	doEDIT;
	postField = postFieldPre + "&item=src&value=" + encode(extInfo.src);
	doEDIT;
	postField = postFieldPre + "&item=info&value=" + encode(extInfo.info);
	doEDIT;

	curl_easy_cleanup(handle);
	delete[] errBuf;

	if (!buf.empty())
		return errInfo(std::string("E:Server side error:") + buf);

	return errInfo();
}

#endif
