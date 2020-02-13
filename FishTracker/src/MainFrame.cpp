#include "App.h"
#include "IDs.h"
#include "Instrumentation/ScopeTimer.h"
#include "Instrumentation/Profile.h"

wxBEGIN_EVENT_TABLE(ft::MainFrame, wxFrame)
EVT_CLOSE(ft::MainFrame::OnClose)
EVT_BUTTON(FT_ID_LOAD, ft::MainFrame::OnLoadVideo)
wxEND_EVENT_TABLE()

namespace ft {

	MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "Fish Tracker", wxDefaultPosition, wxSize(320, 240), wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER)
	{
		FT_PROFILE_FUNCTION();
		m_FishFrames.reserve(FT_MAX_NUMBER_OF_INSPECTIONS);

		m_LoadBtn = new wxButton(this, FT_ID_LOAD, "Load Video");

		wxBoxSizer* Sizer = new wxBoxSizer(wxHORIZONTAL);
		Sizer->Add(m_LoadBtn, 1, wxEXPAND);

		SetSizer(Sizer);
		Sizer->Layout();

		m_Timer = new PaintTimer(this);
		m_Timer->Start();
	}

	void MainFrame::OnClose(wxCloseEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		m_Timer->Stop();
		evt.Skip(); // don't stop event, we still want window to close
		FT_PROFILE_END_SESSION();
	}

	void MainFrame::OnLoadVideo(wxCommandEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		//if (...current content has not been saved...)
		//{
		//	if (wxMessageBox(_("Current content has not been saved! Proceed?"), _("Please confirm"),
		//		wxICON_QUESTION | wxYES_NO, this) == wxNO)
		//		return;
		//	//else: proceed asking to the user the new file to open
		//}

		if (m_FishFrames.size() >= FT_MAX_NUMBER_OF_INSPECTIONS)
		{
			wxLogInfo("FishTracker doesn't suport more than %d inspections simultaneously!", FT_MAX_NUMBER_OF_INSPECTIONS);
			return;
		}

		wxFileDialog openFileDialog(this, _("Load video"), "", "",
			"wmv files (*.wmv)|*.wmv|mp4 files (*.mp4)|*.mp4|All files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

		if (openFileDialog.ShowModal() == wxID_CANCEL)
			return; // the user changed idea...

		{
			FT_PROFILE_SCOPE("OnLoadVideo: After file selection");
			// proceed loading the file chosen by the user;
			// this can be done with e.g. wxWidgets input streams:
			m_FishFrames.push_back(new FishFrame(this, (std::string)openFileDialog.GetPath()));
			m_FishFrames.back()->SetIterator(m_FishFrames.end() - 1);

			if (!m_FishFrames.back()->Opened())
			{
				delete m_FishFrames.back();
				m_FishFrames.pop_back();
				wxLogError("Unable to open video!");
				return;
			}

			m_FishFrames.back()->Show();
		}
	}

} // namespace ft