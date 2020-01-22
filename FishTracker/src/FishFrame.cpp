#include "App.h"
#include "IDs.h"
#include "FishFrame.h"
#include "Instrumentation/ScopeTimer.h"
#include "Instrumentation/Profile.h"

wxBEGIN_EVENT_TABLE(ft::FishFrame, wxFrame)
EVT_CLOSE(ft::FishFrame::OnClose)
EVT_SIZE(ft::FishFrame::OnSize)
EVT_BUTTON(FT_ID_PLAY, ft::FishFrame::OnPlay)
EVT_BUTTON(FT_ID_PAUSE, ft::FishFrame::OnPause)
EVT_BUTTON(FT_ID_FASTFOWARD, ft::FishFrame::OnFastFoward)
wxEND_EVENT_TABLE()

namespace ft
{

	FishFrame::FishFrame(const std::string& videoPath) : wxFrame(nullptr, wxID_ANY, "Inspector", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE),
		m_Cap(videoPath), m_SleepDuration(std::chrono::microseconds(0)), m_FrameTimePoint(std::chrono::high_resolution_clock::now()), m_SleepTimePoint(std::chrono::high_resolution_clock::now())
	{
		FT_PROFILE_FUNCTION();
		if (m_Cap.isOpened())
		{
			CreateStatusBar();

			m_Cap.read(m_CapFrame);

			m_VideoFrameDuration = std::chrono::nanoseconds((long long)(1000000000.0 / m_Cap.get(cv::CAP_PROP_FPS)));
			m_OriginalFrameSize.width = (int)m_Cap.get(cv::CAP_PROP_FRAME_WIDTH);
			m_OriginalFrameSize.height = (int)m_Cap.get(cv::CAP_PROP_FRAME_HEIGHT);
			int displayWidth, displayHeight;
			wxDisplaySize(&displayWidth, &displayHeight);
			if (m_OriginalFrameSize.width < (displayWidth) && m_OriginalFrameSize.height < (displayHeight - 50))
			{
				m_CurrentFrameSize.SetWidth(m_OriginalFrameSize.width);
				m_CurrentFrameSize.SetHeight(m_OriginalFrameSize.height);
			}
			else if (m_OriginalFrameSize.width < (displayWidth * 2) && m_OriginalFrameSize.height < (displayHeight * 2 - 50))
			{
				m_CurrentFrameSize.SetWidth(m_OriginalFrameSize.width / 2);
				m_CurrentFrameSize.SetHeight(m_OriginalFrameSize.height / 2);
			}
			else
			{
				m_CurrentFrameSize.SetWidth(m_OriginalFrameSize.width / 4);
				m_CurrentFrameSize.SetHeight(m_OriginalFrameSize.height / 4);
			}

			m_FishPanel = new FishPanel(this);
			m_PlayBtn = new wxButton(this, FT_ID_PLAY, "PLAY", wxDefaultPosition, wxSize(30, 30));
			m_PauseBtn = new wxButton(this, FT_ID_PAUSE, "PAUSE", wxDefaultPosition, wxSize(30, 30));
			m_FastFowardBtn = new wxButton(this, FT_ID_FASTFOWARD, "FF", wxDefaultPosition, wxSize(30, 30));

			wxBoxSizer* hSizer = new wxBoxSizer(wxHORIZONTAL);
			wxBoxSizer* vSizer = new wxBoxSizer(wxVERTICAL);

			hSizer->Add(m_PlayBtn, 1, wxEXPAND);
			hSizer->Add(m_PauseBtn, 1, wxEXPAND);
			hSizer->Add(m_FastFowardBtn, 1, wxEXPAND);
			vSizer->Add(m_FishPanel, 1, wxEXPAND);
			vSizer->Add(hSizer, 0, wxEXPAND);

			SetSizer(vSizer);
			vSizer->Layout();

			wxSize frameSize = m_CurrentFrameSize + wxSize(0, 30);
			SetClientSize(frameSize);
			SetMinClientSize(GetMinClientSize() + wxSize(0, 40));
			m_FishThread = new std::thread(&FishFrame::Run, this);

			m_VideoAvaliable = true;
		}
	}
	FishFrame::~FishFrame()
	{
		// it's better to do any thread cleanup in the OnClose()
		// event handler, rather than in the destructor.
		// This is because the event loop for a top-level window is not
		// active anymore when its destructor is called and if the thread
		// sends events when ending, they won't be processed unless
		// you ended the thread from OnClose.
		// See @ref overview_windowdeletion for more info.
	}

	void FishFrame::Run()
	{
		FT_PROFILE_FUNCTION();
		while (m_VideoAvaliable)
			if (m_VideoPlaying)
			{
				FT_PROFILE_SCOPE("Run: Frame");
				FT_FUNCTION_TIMER_STATUS(microseconds, this);
				{
					FT_PROFILE_SCOPE("Run: m_Cap.read(m_CapFrame);");
					m_Cap.read(m_CapFrame); // Main Bottleneck
				}
				if (m_CapFrame.empty())
				{
					wxLogInfo("End of the video");
					wxGetApp().ActivateRenderLoop(false);
					m_VideoPlaying = false;
					m_VideoAvaliable = false;
					return;
				}

				if (m_VideoFastFoward)
					continue;
				{
					FT_PROFILE_SCOPE("Run: Sleeping");
					m_SleepDuration = m_VideoFrameDuration - (std::chrono::high_resolution_clock::now() - m_FrameTimePoint) - ((m_FrameTimePoint - m_SleepTimePoint) - m_SleepDuration);
					m_SleepTimePoint = std::chrono::high_resolution_clock::now();
					std::this_thread::sleep_for(m_SleepDuration);
					m_FrameTimePoint = std::chrono::high_resolution_clock::now();
				}
			}
			else
				std::this_thread::sleep_for(std::chrono::microseconds(100));
	}

	void FishFrame::OnClose(wxCloseEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		m_VideoAvaliable = false;

		m_FishThread->join();
		delete m_FishThread;

		wxGetApp().ActivateRenderLoop(false);

		Destroy();
		evt.Skip(); // don't stop event, we still want window to close
	}

	void ft::FishFrame::OnSize(wxSizeEvent& evt)
	{
		FT_PROFILE_FUNCTION();

		m_ResizeHandled = false;
		m_FishPanel->PaintNow();

		evt.Skip();
	}

	void FishFrame::OnPlay(wxCommandEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		if (!m_VideoAvaliable)
		{
			wxLogInfo("There is no video loaded");
			return;
		}
		wxGetApp().ActivateRenderLoop(true);
		m_VideoFastFoward = false;
		m_VideoPlaying = true;
		m_SleepTimePoint = std::chrono::high_resolution_clock::now() - m_VideoFrameDuration;
		m_FrameTimePoint = std::chrono::high_resolution_clock::now();
	}
	void FishFrame::OnPause(wxCommandEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		if (!m_VideoAvaliable)
		{
			wxLogInfo("There is no video loaded");
			return;
		}
		wxGetApp().ActivateRenderLoop(false);
		m_VideoPlaying = false;
	}
	void FishFrame::OnFastFoward(wxCommandEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		if (!m_VideoAvaliable)
		{
			wxLogInfo("There is no video loaded");
			return;
		}
		wxGetApp().ActivateRenderLoop(true);
		m_VideoPlaying = true;
		m_VideoFastFoward = true;
	}

} // namespace ft
