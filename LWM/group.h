#pragma once

#ifndef _H_GROUP
#define _H_GROUP

#include "errInfo.h"

class group
{
public:
	group(size_t _gID, const std::string &_name){ gID = _gID; name = _name; };
	size_t getGID(){ return gID; };
	const std::string &getName(){ return name; };
	void getMember(std::list<size_t> &ret){ std::for_each(members.begin(), members.end(), [&ret](size_t uID){ ret.push_back(uID); }); };

#ifndef _LWM_NO_EDIT
	errInfo editName(const std::string &newName);
	errInfo addMember(size_t uID);
	errInfo delMember(size_t uID);
#endif

	friend errInfo readGrpList();
private:
	size_t gID;
	std::string name;
	std::set<size_t> members;
#ifndef _LWM_NO_EDIT
	errInfo writeMemList();
#endif
};

typedef std::map<size_t, group*> grpListTp;
extern grpListTp grpList;

errInfo readGrpList();
#ifndef _LWM_NO_EDIT
errInfo newGrp(const std::string& name, group **ret);
errInfo delGrp(size_t gID);
#endif

#endif
