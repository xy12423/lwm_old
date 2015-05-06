#pragma once

#ifndef _H_GLOBAL
#define _H_GLOBAL

enum guiStrID{
	TITLE_WM,

	TEXT_STATICGROUP, TEXT_BUTTONADDG, TEXT_BUTTONDELG, TEXT_BUTTONRENAMEG,
	TEXT_STATICMEMBER, TEXT_CHECKDISPALL, TEXT_LABELSEARCHMEM, TEXT_BUTTONADDM, TEXT_BUTTONDELM,
	TEXT_STATICINFO,
	TEXT_LABELID, TEXT_LABELSOURCE, TEXT_LABELINFO, TEXT_LABELMEMGROUP,
	TEXT_LABELMEMWORK, TEXT_BUTTONADDMW, TEXT_BUTTONDELMW, TEXT_BUTTONEDITMW,
	TEXT_BUTTONAPPLY,

	TITLE_EWORK,
	TEXT_LABELNAMEW, TEXT_LABELINFOW, TEXT_BUTTONAPPLYW,

	TEXT_CONFIRM_DEL, TITLE_CONFIRM,

	guiStrCount
};
extern wxString guiStrData[guiStrCount];

extern const char* scriptURL;

typedef unsigned char BYTE;
typedef std::string dataBuf;
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);

int str2num(std::string str, size_t &ret, int type = 0);

#endif
