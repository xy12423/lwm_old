#pragma once

#ifndef _H_WORK
#define _H_WORK

#include "errInfo.h"

class work
{
public:
	work(size_t _wID, const std::string &_name, const std::string &_info){ wID = _wID; name = _name; info = _info; };
	size_t getWID(){ return wID; };
	const std::string &getName(){ return name; };
	const std::string &getInfo(){ return info; };
	void getMember(std::list<size_t> &ret){ std::for_each(members.begin(), members.end(), [&ret](size_t uID){ ret.push_back(uID); }); };

#ifndef _LWM_NO_EDIT
	errInfo editName(const std::string &newName);
	errInfo editInfo(const std::string &newInfo);
	errInfo addMember(size_t uID);
	errInfo delMember(size_t uID);
#endif

	friend errInfo readWorkList();
private:
	size_t wID;
	std::string name;
	std::string info;
	std::set<size_t> members;
#ifndef _LWM_NO_EDIT
	errInfo writeMemList();
#endif
};

typedef std::map<size_t, work*> workListTp;
extern workListTp workList;

errInfo readWorkList();
#ifndef _LWM_NO_EDIT
errInfo newWork(const std::string &name, const std::string &info, work **ret);
errInfo delWork(size_t wID);
#endif

#endif
