#include "App.h"
#include "Core.h"
#include "Instrumentation/ScopeTimer.h"

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_CLOSE(MainFrame::OnClose)
EVT_BUTTON(10001, MainFrame::OnLoadVideo)
EVT_BUTTON(10002, MainFrame::OnPlay)
EVT_BUTTON(10003, MainFrame::OnPause)
EVT_BUTTON(10004, MainFrame::OnFastFoward)
wxEND_EVENT_TABLE()

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "Fish Tracker")
{
	CreateStatusBar();

	m_TickFreq = (int)cv::getTickFrequency() / (1000 + 333);

	m_LoadBtn = new wxButton(this, 10001, "Load Video");
	m_PlayBtn = new wxButton(this, 10002, "Play");
	m_PauseBtn = new wxButton(this, 10003, "Pause");
	m_FastFowardBtn = new wxButton(this, 10004, "Fast Foward");

	wxBoxSizer* hSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* vSizer = new wxBoxSizer(wxVERTICAL);

	hSizer->Add(m_PlayBtn, 1, wxEXPAND);
	hSizer->Add(m_PauseBtn, 1, wxEXPAND);
	hSizer->Add(m_FastFowardBtn, 1, wxEXPAND);
	vSizer->Add(m_LoadBtn, 1, wxEXPAND);
	vSizer->Add(hSizer, 1, wxEXPAND);

	this->SetSizer(vSizer);
	vSizer->Layout();
}

void MainFrame::OnPlay(wxCommandEvent& evt)
{
	if (!m_VideoLoaded)
	{
		wxLogInfo("There is no video loaded");
		return;
	}
	wxGetApp().activateRenderLoop(true);
	m_VideoPlaying = true;
	m_VideoFastFoward = false;
}

void MainFrame::OnPause(wxCommandEvent& evt)
{
	if (!m_VideoLoaded)
	{
		wxLogInfo("There is no video loaded");
		return;
	}
	wxGetApp().activateRenderLoop(false);
	m_VideoPlaying = false;
}

void MainFrame::OnFastFoward(wxCommandEvent& evt)
{
	if (!m_VideoLoaded)
	{
		wxLogInfo("There is no video loaded");
		return;
	}
	wxGetApp().activateRenderLoop(true);
	m_VideoPlaying = true;
	m_VideoFastFoward = true;
}

void MainFrame::Render()
{
	if (m_VideoPlaying)
	{
		m_Cap.read(m_VideoMat);
		if (m_VideoMat.empty())
		{
			wxLogInfo("End of the video");
			m_VideoPlaying = false;
			m_VideoLoaded = false;
			return;
		}

		cv::namedWindow("Video", cv::WINDOW_NORMAL | cv::WINDOW_GUI_NORMAL);
		cv::imshow("Video", m_VideoMat);

		if (m_VideoFastFoward)
			return;

		m_DeltaTime = (int)cv::getTickCount() - m_DeltaTime;
		int deltaTimeMilli = m_FramePeriod - m_DeltaTime / m_TickFreq;
		if (deltaTimeMilli > 1)
			cv::waitKey(deltaTimeMilli);
		m_DeltaTime = (int)cv::getTickCount();
	}
}

void MainFrame::OnClose(wxCloseEvent& evt)
{
	cv::destroyAllWindows();
	wxGetApp().activateRenderLoop(false);
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
	m_Cap = cv::VideoCapture((std::string)openFileDialog.GetPath());

	if (!m_Cap.isOpened())
	{
		wxLogError("ERROR! Unable to open camera");
		return;
	}

	m_FramePeriod = 1000 / m_Cap.get(cv::CAP_PROP_FPS);

	m_VideoLoaded = true;
}