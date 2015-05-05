#include "stdafx.h"
#include "global.h"
#include "work.h"
#include "form_editwork.h"

#define _GUI_SIZE_X 300
#define _GUI_SIZE_Y 300

wxBEGIN_EVENT_TABLE(eworkFrame, wxDialog)

EVT_BUTTON(ID_BUTTONAPPLY, eworkFrame::buttonApply_Click)

wxEND_EVENT_TABLE()

eworkFrame::eworkFrame(const wxString &_name, const wxString &_info)
	: wxDialog(NULL, ID_FRAME, guiStrData[TITLE_EWORK], wxDefaultPosition, wxSize(_GUI_SIZE_X, _GUI_SIZE_Y))
{
	Center();
	panel = new wxPanel(this);
	wxStaticText *label;

	label = new wxStaticText(panel, wxID_ANY,
		guiStrData[TEXT_LABELNAMEW],
		wxPoint(12, 12),
		wxSize(41, 21)
		);
	textName = new wxTextCtrl(panel, ID_TEXTNAME,
		_name,
		wxPoint(59, 12),
		wxSize(213, 21)
		);
	label = new wxStaticText(panel, wxID_ANY,
		guiStrData[TEXT_LABELINFOW],
		wxPoint(12, 39),
		wxSize(53, 21)
		);
	textInfo = new wxTextCtrl(panel, ID_TEXTINFO,
		_info,
		wxPoint(14, 60),
		wxSize(258, 157),
		wxTE_MULTILINE
		);
	buttonApply = new wxButton(panel, ID_BUTTONAPPLY,
		guiStrData[TEXT_BUTTONAPPLYW],
		wxPoint(14, 223),
		wxSize(258, 30)
		);
	name = _name;
	info = _info;
}

void eworkFrame::buttonApply_Click(wxCommandEvent& event)
{
	name = textName->GetValue();
	info = textInfo->GetValue();
	Close();
}
