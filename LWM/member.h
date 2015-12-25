#pragma once

#ifndef _H_MEMBER
#define _H_MEMBER

#include "utils.h"

struct uExtInfo
{
	uExtInfo(){}
	uExtInfo(const std::wstring &_src, const std::wstring &_info)
		:src(_src), info(_info) {}
	uExtInfo(std::wstring &&_src, std::wstring &&_info)
		:src(_src), info(_info) {}
	uExtInfo(const uExtInfo &_ext)
		:src(_ext.src), info(_ext.info) {}
	uExtInfo(uExtInfo &&_ext)
		:src(std::move(_ext.src)), info(std::move(_ext.info)) {}

	const uExtInfo &operator=(const uExtInfo &_ext) { src = _ext.src; info = _ext.info; return *this; }
	const uExtInfo &operator=(uExtInfo &&_ext) { src = std::move(_ext.src); info = std::move(_ext.info); return *this; }
	std::wstring src;
	std::wstring info;
};

class member
{
public:
	member(size_t _uID, const std::wstring &_name, const uExtInfo &_extInfo)
		:uID(_uID), name(_name), extInfo(_extInfo) {}
	member(size_t _uID, std::wstring &&_name, uExtInfo &&_extInfo)
		:uID(_uID), name(_name), extInfo(_extInfo) {}
	size_t getUID(){ return uID; };
	const std::wstring &getName(){ return name; };
	const uExtInfo &getExtInfo(){ return extInfo; };
	void getGroup(std::list<size_t> &ret) { for (size_t gID : groups) { ret.push_back(gID); }; };
	size_t getGroupCount(){ return groups.size(); };
	bool isInGroup(size_t gID){ return groups.find(gID) != groups.end(); }
	void getWork(std::list<size_t> &ret) { for(size_t wID : works) { ret.push_back(wID); }; };

	void editName(const std::wstring &newName){ name = newName; };
	void editName(std::wstring &&newName) { name = std::move(newName); };
	void editInfo(const uExtInfo &newInfo){ extInfo = newInfo; };
	void editInfo(uExtInfo &&newInfo) { extInfo = std::move(newInfo); };
	errInfo addGroup(size_t gID){ groups.insert(gID); return writeGrpList(); };
	errInfo delGroup(size_t gID){ groups.erase(gID); return writeGrpList(); };
	errInfo addWork(size_t wID){ works.insert(wID); return writeWorkList(); };
	errInfo delWork(size_t wID){ works.erase(wID); return writeWorkList(); };
	errInfo applyEdit();

	friend errInfo readMemList();
private:
	size_t uID;
	std::wstring name;
	uExtInfo extInfo;
	std::set<size_t> groups, works;

	errInfo writeGrpList();
	errInfo writeWorkList();
};

typedef std::map<size_t, member*> memListTp;
extern memListTp memList;

errInfo readMemList();
errInfo newMem(const std::wstring &name, const uExtInfo &extInfo, member **ret);
errInfo delMem(size_t uID);

#endif
