#include "SetScaleLenFrame.h"
#include "FishFrame.h"
#include "IDs.h"

wxBEGIN_EVENT_TABLE(ft::SetScaleLenFrame, wxFrame)

EVT_BUTTON(FT_ID_SET_LEN_REF, ft::SetScaleLenFrame::OnSet)

wxEND_EVENT_TABLE()

namespace ft {

	SetScaleLenFrame::SetScaleLenFrame(FishFrame* parent) : wxFrame(parent, wxID_ANY, "Setting Reference Lenght", wxDefaultPosition, { 200,100 }, wxCAPTION | wxFRAME_TOOL_WINDOW | wxFRAME_FLOAT_ON_PARENT),
		m_FishFrame(parent)
	{
		m_LenCtrl = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1000, 10);
		m_UnityChc = new wxChoice(this, wxID_ANY);

		m_UnityChc->AppendString("mm");
		m_UnityChc->AppendString("cm");
		m_UnityChc->AppendString("dm");
		m_UnityChc->AppendString("m");

		m_UnityChc->SetSelection(0);

		m_SetLenBtn = new wxButton(this, FT_ID_SET_LEN_REF, "Set");

		wxBoxSizer* hSizer = new wxBoxSizer(wxHORIZONTAL);
		wxBoxSizer* vSizer = new wxBoxSizer(wxVERTICAL);

		hSizer->Add(m_LenCtrl, 1, wxEXPAND);
		hSizer->Add(m_UnityChc, 0, wxEXPAND);
		vSizer->Add(hSizer, 0, wxEXPAND);
		vSizer->Add(m_SetLenBtn, 1, wxEXPAND);

		SetSizer(vSizer);
		vSizer->Layout();
	}

	void SetScaleLenFrame::OnSet(wxCommandEvent& evt)
	{
		m_FishFrame->m_Scale->SetLenghtInMillimeters(m_LenCtrl->GetValue() * pow(10, m_UnityChc->GetSelection()));
		Close();
	}

} // namespace ft

