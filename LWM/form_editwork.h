#pragma once

#ifndef _H_EWORK
#define _H_EWORK

class eworkFrame : public wxDialog
{
public:
	eworkFrame();
	enum itemID{
		ID_FRAME,
		ID_TEXTNAME, ID_TEXTINFO, ID_BUTTONAPPLY
	};

	wxString name, info;

	wxPanel *panel;

	wxTextCtrl *textName, *textInfo;
	wxButton *buttonApply;
	void buttonApply_Click(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();
};

#endif
