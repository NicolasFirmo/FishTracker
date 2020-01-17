#include "App.h"
#include "Instrumentation/ScopeTimer.h"

wxBEGIN_EVENT_TABLE(ft::MainFrame, wxFrame)
EVT_CLOSE(ft::MainFrame::OnClose)
EVT_BUTTON(10001, ft::MainFrame::OnLoadVideo)
wxEND_EVENT_TABLE()

namespace ft {

	MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "Fish Tracker", wxDefaultPosition, wxSize(320, 240), wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER)
	{
		m_LoadBtn = new wxButton(this, 10001, "Load Video");

		wxBoxSizer* Sizer = new wxBoxSizer(wxHORIZONTAL);

		Sizer->Add(m_LoadBtn, 1, wxEXPAND);

		this->SetSizer(Sizer);
		Sizer->Layout();
	}

	void MainFrame::OnClose(wxCloseEvent& evt)
	{
		evt.Skip(); // don't stop event, we still want window to close
	}

	void MainFrame::OnLoadVideo(wxCommandEvent& evt)
	{
		//if (...current content has not been saved...)
		//{
		//	if (wxMessageBox(_("Current content has not been saved! Proceed?"), _("Please confirm"),
		//		wxICON_QUESTION | wxYES_NO, this) == wxNO)
		//		return;
		//	//else: proceed asking to the user the new file to open
		//}

		wxFileDialog openFileDialog(this, _("Load video"), "", "",
			"mp4 files (*.mp4)|*.mp4|All files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

		if (openFileDialog.ShowModal() == wxID_CANCEL)
			return; // the user changed idea...

		// proceed loading the file chosen by the user;
		// this can be done with e.g. wxWidgets input streams:
		m_FishFrame = new FishFrame((std::string)openFileDialog.GetPath());

		if (!m_FishFrame->Opened())
		{
			delete m_FishFrame;
			wxLogError("Unable to open video!");
			return;
		}

		m_FishFrame->Show();
	}

} // namespace ft