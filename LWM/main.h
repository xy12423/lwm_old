#pragma once

#ifndef _H_MAIN
#define _H_MAIN

class mainFrame : public wxFrame
{
public:
	mainFrame(const wxString& title);
	enum itemID{
		ID_FRAME,
		ID_STATICGROUP, ID_LISTGROUP, ID_BUTTONADDG, ID_BUTTONDELG, ID_BUTTONRENAMEG,
		ID_STATICMEMBER, ID_CHECKDISPALL, ID_TEXTSEARCHMEM, ID_LISTMEMBER, ID_BUTTONADDM, ID_BUTTONDELM,
		ID_STATICINFO,
		ID_TEXTID, ID_TEXTSOURCE, ID_TEXTINFO, ID_LISTMEMGROUP,
		ID_LISTMEMWORK, ID_BUTTONADDMW, ID_BUTTONDELMW, ID_BUTTONEDITMW, ID_TEXTWORKINFO,
		ID_BUTTONAPPLY
	};

	void refMemDispList();

	wxPanel *panel;

	wxStaticBox *staticGroup;
	wxCheckListBox *listGroup;
	wxButton *buttonAddG;
	wxButton *buttonDelG;
	wxButton *buttonRenameG;
	void listGroup_ItemCheck(wxCommandEvent& event);
	void buttonAddG_Click(wxCommandEvent& event);
	void buttonDelG_Click(wxCommandEvent& event);
	void buttonRenameG_Click(wxCommandEvent& event);

	wxStaticBox *staticMember;
	wxCheckBox *checkDispAll;
	wxTextCtrl *textSearchMem;
	wxListBox *listMember;
	wxButton *buttonAddM;
	wxButton *buttonDelM;
	void textSearchMem_TextChanged(wxCommandEvent& event);
	void listMember_SelectedIndexChanged(wxCommandEvent& event);
	void buttonAddM_Click(wxCommandEvent& event);
	void buttonDelM_Click(wxCommandEvent& event);

	wxStaticBox *staticInfo;

	wxTextCtrl *textID;
	wxTextCtrl *textSource;
	wxTextCtrl *textInfo;
	wxCheckListBox *listMemGroup;
	void listMemGroup_ItemCheck(wxCommandEvent& event);

	wxListBox *listMemWork;
	wxButton *buttonAddMW;
	wxButton *buttonDelMW;
	wxButton *buttonEditMW;
	wxTextCtrl *textWorkInfo;
	void listMemWork_SelectedIndexChanged(wxCommandEvent& event);
	void buttonAddMW_Click(wxCommandEvent& event);
	void buttonDelMW_Click(wxCommandEvent& event);
	void buttonEditMW_Click(wxCommandEvent& event);

	wxButton *buttonApply;
	void buttonApply_Click(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();
};

class MyApp : public wxApp
{
public:
	virtual bool OnInit();
};

#endif
