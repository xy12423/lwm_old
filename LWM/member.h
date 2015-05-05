#pragma once

#ifndef _H_MEMBER
#define _H_MEMBER

#include "utils.h"

struct uExtInfo
{
	uExtInfo(){}
	uExtInfo(const std::wstring &_src, const std::wstring &_info){ src = _src; info = _info; }
	std::wstring src;
	std::wstring info;
};

class member
{
public:
	member(size_t _uID, const std::wstring &_name, const uExtInfo &_extInfo){ uID = _uID; name = _name; extInfo = _extInfo; };
	size_t getUID(){ return uID; };
	const std::wstring &getName(){ return name; };
	const uExtInfo &getExtInfo(){ return extInfo; };
	void getGroup(std::list<size_t> &ret){ std::for_each(groups.begin(), groups.end(), [&ret](size_t gID){ ret.push_back(gID); }); };
	size_t getGroupCount(){ return groups.size(); };
	bool isInGroup(size_t gID){ return groups.find(gID) != groups.end(); }
	void getWork(std::list<size_t> &ret){ std::for_each(works.begin(), works.end(), [&ret](size_t wID){ ret.push_back(wID); }); };

#ifndef _LWM_NO_EDIT
	void editName(const std::wstring &newName){ name = newName; };
	void editInfo(const uExtInfo &newInfo){ extInfo = newInfo; };
	errInfo addGroup(size_t gID){ groups.insert(gID); return writeGrpList(); };
	errInfo delGroup(size_t gID){ groups.erase(gID); return writeGrpList(); };
	errInfo addWork(size_t wID){ works.insert(wID); return writeWorkList(); };
	errInfo delWork(size_t wID){ works.erase(wID); return writeWorkList(); };
	errInfo applyEdit();
#endif

	friend errInfo readMemList();
private:
	size_t uID;
	std::wstring name;
	uExtInfo extInfo;
	std::set<size_t> groups, works;

#ifndef _LWM_NO_EDIT
	errInfo writeGrpList();
	errInfo writeWorkList();
#endif
};

typedef std::map<size_t, member*> memListTp;
extern memListTp memList;

errInfo readMemList();
#ifndef _LWM_NO_EDIT
errInfo newMem(const std::wstring &name, const uExtInfo &extInfo, member **ret);
errInfo delMem(size_t uID);
#endif

#endif
