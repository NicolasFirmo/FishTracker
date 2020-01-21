#include "App.h"
#include "IDs.h"
#include "FishFrame.h"
#include "Instrumentation/ScopeTimer.h"
#include "Instrumentation/Profile.h"

wxBEGIN_EVENT_TABLE(ft::FishFrame, wxFrame)
EVT_CLOSE(ft::FishFrame::OnClose)
EVT_BUTTON(FT_ID_PLAY, ft::FishFrame::OnPlay)
EVT_BUTTON(FT_ID_PAUSE, ft::FishFrame::OnPause)
EVT_BUTTON(FT_ID_FASTFOWARD, ft::FishFrame::OnFastFoward)
wxEND_EVENT_TABLE()

namespace ft {

	FishFrame::FishFrame(const std::string& videoPath) : wxFrame(nullptr, wxID_ANY, "Inspector", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER),
		m_Cap(videoPath), m_SleepDuration(std::chrono::microseconds(0)), m_FrameTimePoint(std::chrono::high_resolution_clock::now()), m_SleepTimePoint(std::chrono::high_resolution_clock::now())
	{
		FT_PROFILE_FUNCTION();
		if (m_Cap.isOpened())
		{
			m_VideoAvaliable = true;
			CreateStatusBar();

			m_Cap.read(m_CapFrame);

			m_VideoFrameDuration = std::chrono::nanoseconds((long long)(1000000000.0 / m_Cap.get(cv::CAP_PROP_FPS)));
			m_FrameSize.width = (int)m_Cap.get(cv::CAP_PROP_FRAME_WIDTH) / 2;
			m_FrameSize.height = (int)m_Cap.get(cv::CAP_PROP_FRAME_HEIGHT) / 2;

			m_Panel = new FishPanel(this);
			m_Panel->SetSize(wxSize(m_FrameSize.width, m_FrameSize.height));

			m_PlayBtn = new wxButton(this, FT_ID_PLAY, "Play");
			m_PauseBtn = new wxButton(this, FT_ID_PAUSE, "Pause");
			m_FastFowardBtn = new wxButton(this, FT_ID_FASTFOWARD, "Fast Foward");

			wxBoxSizer* hSizer = new wxBoxSizer(wxHORIZONTAL);
			wxBoxSizer* vSizer = new wxBoxSizer(wxVERTICAL);

			hSizer->Add(m_PlayBtn, 1, wxEXPAND);
			hSizer->Add(m_PauseBtn, 1, wxEXPAND);
			hSizer->Add(m_FastFowardBtn, 1, wxEXPAND);
			vSizer->Add(m_Panel, m_FrameSize.height + 30, wxEXPAND);
			vSizer->Add(hSizer, 30, wxEXPAND);

			SetSize(GetEffectiveMinSize() + wxSize(0, 30));
			this->SetSizer(vSizer);
			vSizer->Layout();

			m_FishThread = new std::thread(&FishFrame::Run, this);
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
