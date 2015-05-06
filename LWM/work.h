#pragma once

#ifndef _H_WORK
#define _H_WORK

#include "utils.h"

class work
{
public:
	work(size_t _wID, const std::wstring &_name, const std::wstring &_info){ wID = _wID; name = _name; info = _info; };
	size_t getWID(){ return wID; };
	const std::wstring &getName(){ return name; };
	const std::wstring &getInfo(){ return info; };
	void getMember(std::list<size_t> &ret){ std::for_each(members.begin(), members.end(), [&ret](size_t uID){ ret.push_back(uID); }); };

	errInfo editName(const std::wstring &newName);
	errInfo editInfo(const std::wstring &newInfo);
	errInfo addMember(size_t uID);
	errInfo delMember(size_t uID);


	friend errInfo readWorkList();
private:
	size_t wID;
	std::wstring name;
	std::wstring info;
	std::set<size_t> members;
	errInfo writeMemList();
};

typedef std::map<size_t, work*> workListTp;
extern workListTp workList;

errInfo readWorkList();
errInfo newWork(const std::wstring &name, const std::wstring &info, work **ret);
errInfo delWork(size_t wID);

#endif
