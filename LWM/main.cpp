#include "stdafx.h"
#include "global.h"
#include "group.h"
#include "member.h"
#include "work.h"
#ifndef _LWM_NO_EDIT
#include "form_editwork.h"
#endif
#include "main.h"

wxBEGIN_EVENT_TABLE(mainFrame, wxFrame)

EVT_CHECKLISTBOX(ID_LISTGROUP, mainFrame::listGroup_ItemCheck)
EVT_BUTTON(ID_BUTTONADDG, mainFrame::buttonAddG_Click)
EVT_BUTTON(ID_BUTTONDELG, mainFrame::buttonDelG_Click)
EVT_BUTTON(ID_BUTTONRENAMEG, mainFrame::buttonRenameG_Click)

EVT_TEXT(ID_TEXTSEARCHMEM, mainFrame::textSearchMem_TextChanged)
EVT_LISTBOX(ID_LISTMEMBER, mainFrame::listMember_SelectedIndexChanged)
EVT_BUTTON(ID_BUTTONADDM, mainFrame::buttonAddM_Click)
EVT_BUTTON(ID_BUTTONDELM, mainFrame::buttonDelM_Click)

EVT_CHECKLISTBOX(ID_LISTMEMGROUP, mainFrame::listMemGroup_ItemCheck)
EVT_LISTBOX(ID_LISTMEMWORK, mainFrame::listMemWork_SelectedIndexChanged)
EVT_BUTTON(ID_BUTTONADDMW, mainFrame::buttonAddMW_Click)
EVT_BUTTON(ID_BUTTONDELMW, mainFrame::buttonDelMW_Click)
EVT_BUTTON(ID_BUTTONEDITMW, mainFrame::buttonEditMW_Click)
EVT_BUTTON(ID_BUTTONAPPLY, mainFrame::buttonApply_Click)

wxEND_EVENT_TABLE()

#ifdef __WXMSW__
#define _GUI_GAP 20
#define _GUI_SIZE_X 960
#define _GUI_SIZE_Y 500
#else
#define _GUI_GAP 0
#define _GUI_SIZE_X 940
#define _GUI_SIZE_Y 460
#endif

#define checkErr if (err.err){ wxMessageBox(err.info, wxT("ERROR"), wxOK | wxICON_ERROR); return; }

mainFrame *form;
std::vector<size_t> grpListDisp;
std::vector<size_t> memListDisp;
std::vector<size_t> workListDisp;

void loadCfg()
{
	std::ifstream fin(".config");
	if (!fin)
	{
		std::ofstream fout(".config");
		fout.close();
	}
	std::string tmp;
	std::getline(fin, tmp);
	scriptURL = str2cstr(tmp);
	fin.close();
}

mainFrame::mainFrame(const wxString& title)
	: wxFrame(NULL, ID_FRAME, title, wxDefaultPosition, wxSize(_GUI_SIZE_X, _GUI_SIZE_Y))
{
	Center();

	panel = new wxPanel(this);
	wxStaticText *label;

	staticGroup = new wxStaticBox(panel, ID_STATICGROUP,
		guiStrData[TEXT_STATICGROUP],
		wxPoint(12, 12),
		wxSize(195, 437)
		);

	wxArrayString groupList;
	std::for_each(grpList.begin(), grpList.end(), [&groupList](std::pair<size_t, group*> gPtr){
		groupList.push_back(gPtr.second->getName());
		grpListDisp.push_back(gPtr.first);
	});

	listGroup = new wxCheckListBox(staticGroup, ID_LISTGROUP,
		wxPoint(6, _GUI_GAP),
		wxSize(183, 372),
		groupList
		);
	for (size_t i = 0; i < grpList.size(); i++)
		listGroup->Check(i);
	buttonAddG = new wxButton(staticGroup, ID_BUTTONADDG,
		guiStrData[TEXT_BUTTONADDG],
		wxPoint(6, _GUI_GAP + 378),
		wxSize(57, 33)
		);
	buttonDelG = new wxButton(staticGroup, ID_BUTTONDELG,
		guiStrData[TEXT_BUTTONDELG],
		wxPoint(69, _GUI_GAP + 378),
		wxSize(57, 33)
		);
	buttonRenameG = new wxButton(staticGroup, ID_BUTTONRENAMEG,
		guiStrData[TEXT_BUTTONRENAMEG],
		wxPoint(132, _GUI_GAP + 378),
		wxSize(57, 33)
		);

	staticMember = new wxStaticBox(panel, ID_STATICMEMBER,
		guiStrData[TEXT_STATICMEMBER],
		wxPoint(213, 12),
		wxSize(276, 437)
		);

	label = new wxStaticText(staticMember, wxID_ANY,
		guiStrData[TEXT_LABELSEARCHMEM],
		wxPoint(6, _GUI_GAP),
		wxSize(29, 21)
		);
	textSearchMem = new wxTextCtrl(staticMember, ID_TEXTSEARCHMEM,
		wxEmptyString,
		wxPoint(41, _GUI_GAP - 3),
		wxSize(229, 21)
		);
	listMember = new wxListBox(staticMember, ID_LISTMEMBER,
		wxPoint(8, _GUI_GAP + 24),
		wxSize(262, 352),
		wxArrayString()
		);
	buttonAddM = new wxButton(staticMember, ID_BUTTONADDM,
		guiStrData[TEXT_BUTTONADDM],
		wxPoint(8, _GUI_GAP + 382),
		wxSize(128, 29)
		);
	buttonDelM = new wxButton(staticMember, ID_BUTTONDELM,
		guiStrData[TEXT_BUTTONDELM],
		wxPoint(142, _GUI_GAP + 382),
		wxSize(128, 29)
		);

	staticInfo = new wxStaticBox(panel, ID_STATICINFO,
		guiStrData[TEXT_STATICINFO],
		wxPoint(495, 12),
		wxSize(444, 437)
		);

	label = new wxStaticText(staticInfo, wxID_ANY,
		guiStrData[TEXT_LABELID],
		wxPoint(6, _GUI_GAP),
		wxSize(17, 21)
		);
	textID = new wxTextCtrl(staticInfo, ID_TEXTID,
		wxEmptyString,
		wxPoint(29, _GUI_GAP - 3),
		wxSize(184, 21)
		);
	label = new wxStaticText(staticInfo, wxID_ANY,
		guiStrData[TEXT_LABELSOURCE],
		wxPoint(6, _GUI_GAP + 27),
		wxSize(29, 21)
		);
	textSource = new wxTextCtrl(staticInfo, ID_TEXTSOURCE,
		wxEmptyString,
		wxPoint(41, _GUI_GAP + 24),
		wxSize(172, 21)
		);
	label = new wxStaticText(staticInfo, wxID_ANY,
		guiStrData[TEXT_LABELINFO],
		wxPoint(6, _GUI_GAP + 51),
		wxSize(29, 21)
		);
	textInfo = new wxTextCtrl(staticInfo, ID_TEXTINFO,
		wxEmptyString,
		wxPoint(6, _GUI_GAP + 70),
		wxSize(207, 132),
		wxTE_MULTILINE
		);
	label = new wxStaticText(staticInfo, wxID_ANY,
		guiStrData[TEXT_LABELMEMGROUP],
		wxPoint(6, _GUI_GAP + 208),
		wxSize(53, 21)
		);
	listMemGroup = new wxCheckListBox(staticInfo, ID_LISTMEMGROUP,
		wxPoint(6, _GUI_GAP + 228),
		wxSize(207, 148),
		groupList
		);

	label = new wxStaticText(staticInfo, wxID_ANY,
		guiStrData[TEXT_LABELMEMWORK],
		wxPoint(219, _GUI_GAP - 3),
		wxSize(89, 21)
		);
	listMemWork = new wxListBox(staticInfo, ID_LISTMEMWORK,
		wxPoint(219, _GUI_GAP + 15),
		wxSize(219, 232),
		wxArrayString()
		);
	buttonAddMW = new wxButton(staticInfo, ID_BUTTONADDMW,
		guiStrData[TEXT_BUTTONADDMW],
		wxPoint(219, _GUI_GAP + 253),
		wxSize(69, 26)
		);
	buttonDelMW = new wxButton(staticInfo, ID_BUTTONDELMW,
		guiStrData[TEXT_BUTTONDELMW],
		wxPoint(294, _GUI_GAP + 253),
		wxSize(69, 26)
		);
	buttonEditMW = new wxButton(staticInfo, ID_BUTTONEDITMW,
		guiStrData[TEXT_BUTTONEDITMW],
		wxPoint(369, _GUI_GAP + 253),
		wxSize(69, 26)
		);
	textWorkInfo = new wxTextCtrl(staticInfo, ID_TEXTWORKINFO,
		wxEmptyString,
		wxPoint(219, _GUI_GAP + 285),
		wxSize(219, 91),
		wxTE_MULTILINE | wxTE_READONLY
		);

	buttonApply = new wxButton(staticInfo, ID_BUTTONAPPLY,
		guiStrData[TEXT_BUTTONAPPLY],
		wxPoint(6, _GUI_GAP + 382),
		wxSize(432, 29)
		);

	refMemDispList();
}

void mainFrame::refMemDispList()
{
	wxArrayInt sel;
	listGroup->GetCheckedItems(sel);
	wxArrayInt::iterator itrG = sel.begin(), itrGEnd = sel.end();
	std::list<size_t> members;
	std::list<size_t>::iterator itrM, itrMEnd;
	std::set<size_t> memDispTmp;
	std::set<size_t>::iterator itrD, itrDEnd;
	std::string maskName = textSearchMem->GetValue();
	bool enableSearch = !maskName.empty();
	for (; itrG != itrGEnd; itrG++)
	{
		members.clear();
		grpList[grpListDisp[*itrG]]->getMember(members);
		for (itrM = members.begin(), itrMEnd = members.end(); itrM != itrMEnd; itrM++)
			memDispTmp.insert(*itrM);
	}
	itrD = memDispTmp.begin();
	itrDEnd = memDispTmp.end();
	memListDisp.clear();
	listMember->Clear();
	std::string name;
	for (; itrD != itrDEnd; itrD++)
	{
		name = memList[*itrD]->getName();
		if (!enableSearch || name.find(maskName) != std::string::npos)
		{
			memListDisp.push_back(*itrD);
			listMember->Append(name);
		}
	}
}

void mainFrame::listGroup_ItemCheck(wxCommandEvent& event)
{
	refMemDispList();
}

void mainFrame::buttonAddG_Click(wxCommandEvent& event)
{
#ifdef _LWM_NO_EDIT
	wxMessageBox(wxT("Read-Only version"), wxT("ERROR"), wxOK | wxICON_ERROR);
#else
	wxTextEntryDialog inputDlg(this, wxT("请输入组名"), wxT("输入组名"));
	inputDlg.ShowModal();
	wxString name = inputDlg.GetValue();
	if (name != wxEmptyString)
	{
		group *newG;
		errInfo err = newGrp(name.ToStdString(), &newG);
		checkErr;
		int newIndex = listGroup->Append(name);
		listGroup->Check(newIndex);
		listMemGroup->Append(name);
		grpListDisp.clear();
		std::for_each(grpList.begin(), grpList.end(), [](std::pair<size_t, group*> gPtr){
			grpListDisp.push_back(gPtr.first);
		});
		refMemDispList();
	}
#endif
}

void mainFrame::buttonDelG_Click(wxCommandEvent& event)
{
#ifdef _LWM_NO_EDIT
	wxMessageBox(wxT("Read-Only version"), wxT("ERROR"), wxOK | wxICON_ERROR);
#else
	int grpIndex = listGroup->GetSelection();
	if (grpIndex > 0)
	{
		std::list<size_t> depMemList;
		grpList[grpListDisp[grpIndex]]->getMember(depMemList);
		std::for_each(depMemList.begin(), depMemList.end(), [grpIndex](size_t uID){
			memList[uID]->delGroup(grpIndex);
		});
		errInfo err = delGrp(grpIndex);
		checkErr;
		listGroup->Delete(grpIndex);
		listMemGroup->Delete(grpIndex);
		grpListDisp.clear();
		std::for_each(grpList.begin(), grpList.end(), [](std::pair<size_t, group*> gPtr){
			grpListDisp.push_back(gPtr.first);
		});
		refMemDispList();
	}
	else if (grpIndex == 0)
		wxMessageBox(wxT("Can't delete default group"), wxT("ERROR"), wxOK | wxICON_ERROR);
#endif
}

void mainFrame::buttonRenameG_Click(wxCommandEvent& event)
{
#ifdef _LWM_NO_EDIT
	wxMessageBox(wxT("Read-Only version"), wxT("ERROR"), wxOK | wxICON_ERROR);
#else
	wxTextEntryDialog inputDlg(this, wxT("请输入组名"), wxT("输入组名"));
	inputDlg.ShowModal();
	wxString name = inputDlg.GetValue();
	if (name != wxEmptyString)
	{
		int grpIndex = listGroup->GetSelection();
		if (grpIndex >= 0)
		{
			errInfo err = grpList[grpListDisp[grpIndex]]->editName(name.ToStdString());
			checkErr;
			listGroup->SetString(grpIndex, name);
			listMemGroup->SetString(grpIndex, name);
		}
	}
#endif
}

void mainFrame::textSearchMem_TextChanged(wxCommandEvent& event)
{
	refMemDispList();
}

void mainFrame::listMember_SelectedIndexChanged(wxCommandEvent& event)
{
	member *sMem = memList[memListDisp[listMember->GetSelection()]];
	textID->SetValue(sMem->getName());
	const uExtInfo &extInfo = sMem->getExtInfo();
	textSource->SetValue(extInfo.src);
	textInfo->SetValue(extInfo.info);

	wxArrayInt checkedI;
	listMemGroup->GetCheckedItems(checkedI);
	std::for_each(checkedI.begin(), checkedI.end(), [this](size_t gID){
		listMemGroup->Check(gID, false);
	});
	std::list<size_t> groupID;
	sMem->getGroup(groupID);
	std::for_each(groupID.begin(), groupID.end(), [this](size_t gID){
		listMemGroup->Check(gID);
	});

	std::list<size_t> workID;
	sMem->getWork(workID);
	workListDisp.clear();
	listMemWork->Clear();
	std::for_each(workID.begin(), workID.end(), [this](size_t wID){
		listMemWork->Append(workList[wID]->getName());
		workListDisp.push_back(wID);
	});
}

void mainFrame::buttonAddM_Click(wxCommandEvent& event)
{
#ifdef _LWM_NO_EDIT
	wxMessageBox(wxT("Read-Only version"), wxT("ERROR"), wxOK | wxICON_ERROR);
#else
	wxTextEntryDialog inputDlg(this, wxT("请输入成员名"), wxT("输入成员名"));
	inputDlg.ShowModal();
	wxString name = inputDlg.GetValue();

	if (name != wxEmptyString)
	{
		member *newM;
		errInfo err = newMem(name.ToStdString(), uExtInfo(), &newM);
		if (err.err)
		{
			wxMessageBox(err.info, wxT("ERROR"), wxOK | wxICON_ERROR);
			return;
		}
		err = grpList[grpListDisp[0]]->addMember(newM->getUID());
		checkErr;
		err = newM->addGroup(0);
		checkErr;
		if (listGroup->IsChecked(0))
			refMemDispList();
	}
#endif
}

void mainFrame::buttonDelM_Click(wxCommandEvent& event)
{
#ifdef _LWM_NO_EDIT
	wxMessageBox(wxT("Read-Only version"), wxT("ERROR"), wxOK | wxICON_ERROR);
#else
	int memIndex = memListDisp[listMember->GetSelection()];
	if (memIndex >= 0)
	{
		std::list<size_t> depList;
		memList[memIndex]->getGroup(depList);
		std::for_each(depList.begin(), depList.end(), [memIndex](size_t gID){
			grpList[grpListDisp[gID]]->delMember(memIndex);
		});
		depList.clear();
		memList[memIndex]->getWork(depList);
		std::for_each(depList.begin(), depList.end(), [memIndex](size_t wID){
			workList[wID]->delMember(memIndex);
		});
		errInfo err = delMem(memIndex);
		if (err.err)
		{
			wxMessageBox(err.info, wxT("ERROR"), wxOK | wxICON_ERROR);
			return;
		}
		refMemDispList();
	}
#endif
}

void mainFrame::listMemGroup_ItemCheck(wxCommandEvent& event)
{
#ifdef _LWM_NO_EDIT
	wxMessageBox(wxT("Read-Only version"), wxT("ERROR"), wxOK | wxICON_ERROR);
#else
	int index = event.GetInt();
	if (listMember->GetSelection() < 0)
	{
		listMemGroup->Check(index, false);
		return;
	}
	size_t uID = memListDisp[listMember->GetSelection()];
	if (listMemGroup->IsChecked(index))
	{
		errInfo err = memList[uID]->addGroup(index);
		checkErr;
		err = grpList[grpListDisp[index]]->addMember(uID);
		checkErr;
	}
	else
	{
		member *sMem = memList[uID];
		if (sMem->getGroupCount() == 1)
		{
			size_t defaultGrpID = grpListDisp[0];
			if (!sMem->isInGroup(defaultGrpID))
			{
				errInfo err = sMem->delGroup(index);
				checkErr;
				err = grpList[grpListDisp[index]]->delMember(uID);
				checkErr;
				err = memList[uID]->addGroup(defaultGrpID);
				checkErr;
				err = grpList[defaultGrpID]->addMember(uID);
				checkErr;
				listMemGroup->Check(0);
			}
		}
		else
		{
			errInfo err = sMem->delGroup(index);
			checkErr;
			err = grpList[grpListDisp[index]]->delMember(uID);
			checkErr;
		}
	}
#endif
}

void mainFrame::listMemWork_SelectedIndexChanged(wxCommandEvent& event)
{
	work *sel = workList[workListDisp[listMemWork->GetSelection()]];
	textWorkInfo->SetValue(sel->getInfo());
}

void mainFrame::buttonAddMW_Click(wxCommandEvent& event)
{
#ifdef _LWM_NO_EDIT
	wxMessageBox(wxT("Read-Only version"), wxT("ERROR"), wxOK | wxICON_ERROR);
#else
	wxArrayString choices;
	choices.push_back(wxT("[新工作]"));
	std::for_each(workList.begin(), workList.end(), [&choices](std::pair<size_t, work*> wPtr){
		choices.push_back(wPtr.second->getName());
	});
	wxSingleChoiceDialog addDlg(this, wxT("请选择工作"), wxT("选择工作"), choices);
	int result = addDlg.ShowModal();
	if (result == wxID_CANCEL)
		return;
	member *sMem = memList[memListDisp[listMember->GetSelection()]];
	int sel = addDlg.GetSelection();
	if (sel == 0)
	{
		eworkFrame nwDlg;
		nwDlg.ShowModal();
		if (nwDlg.name == wxEmptyString)
			return;
		work *newW;
		errInfo err = newWork(nwDlg.name.ToStdString(), nwDlg.info.ToStdString(), &newW);
		checkErr;
		err = newW->addMember(sel);
		checkErr;
		err = sMem->addWork(newW->getWID());
		checkErr;
	}
	else if (sel > 0)
	{
		sel--;
		errInfo err = workList[sel]->addMember(sel);
		checkErr;
		err = sMem->addWork(sel);
		checkErr;
	}

	std::list<size_t> workID;
	sMem->getWork(workID);
	workListDisp.clear();
	listMemWork->Clear();
	std::for_each(workID.begin(), workID.end(), [this](size_t wID){
		listMemWork->Append(workList[wID]->getName());
		workListDisp.push_back(wID);
	});
#endif
}

void mainFrame::buttonDelMW_Click(wxCommandEvent& event)
{
#ifdef _LWM_NO_EDIT
	wxMessageBox(wxT("Read-Only version"), wxT("ERROR"), wxOK | wxICON_ERROR);
#else
	size_t wID = workListDisp[listMemWork->GetSelection()];
	size_t uID = memListDisp[listMember->GetSelection()];
	errInfo err = memList[uID]->delWork(wID);
	checkErr;
	err = workList[wID]->delMember(uID);
	checkErr;
	textWorkInfo->SetValue(wxEmptyString);

	std::list<size_t> workID;
	memList[uID]->getWork(workID);
	workListDisp.clear();
	listMemWork->Clear();
	std::for_each(workID.begin(), workID.end(), [this](size_t wID){
		listMemWork->Append(workList[wID]->getName());
		workListDisp.push_back(wID);
	});
#endif
}

void mainFrame::buttonEditMW_Click(wxCommandEvent& event)
{
#ifdef _LWM_NO_EDIT
	wxMessageBox(wxT("Read-Only version"), wxT("ERROR"), wxOK | wxICON_ERROR);
#else
	eworkFrame nwDlg;
	nwDlg.ShowModal();
	if (nwDlg.name != wxEmptyString)
	{
		textWorkInfo->SetValue(nwDlg.info);
		work *sel = workList[workListDisp[listMemWork->GetSelection()]];
		errInfo err = sel->editName(nwDlg.name.ToStdString());
		checkErr;
		err = sel->editInfo(nwDlg.info.ToStdString());
		checkErr;
	}
#endif
}

void mainFrame::buttonApply_Click(wxCommandEvent& event)
{
#ifdef _LWM_NO_EDIT
	wxMessageBox(wxT("Read-Only version"), wxT("ERROR"), wxOK | wxICON_ERROR);
#else
	int sel = listMember->GetSelection();
	member *sMem = memList[memListDisp[sel]];
	listMember->SetString(sel, textID->GetValue());
	sMem->editName(textID->GetValue().ToStdString());
	sMem->editInfo(uExtInfo(textSource->GetValue().ToStdString(), textInfo->GetValue().ToStdString()));
	errInfo err = sMem->applyEdit();
	checkErr;
#endif
}

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	loadCfg();
	errInfo err;

	err = readGrpList();
	if (err.err)
	{
		wxMessageBox(wxT("Failed to load group data"), wxT("ERROR"), wxOK | wxICON_ERROR);
		return false;
	}
	err = readMemList();
	if (err.err)
	{
		wxMessageBox(wxT("Failed to load member data"), wxT("ERROR"), wxOK | wxICON_ERROR);
		return false;
	}
	err = readWorkList();
	if (err.err)
	{
		wxMessageBox(wxT("Failed to load work data"), wxT("ERROR"), wxOK | wxICON_ERROR);
		return false;
	}

	form = new mainFrame(guiStrData[TITLE_WM]);
	form->Show();

	return true;
}
