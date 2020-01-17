#include "App.h"
#include "Core.h"
#include "FishFrame.h"
#include "Instrumentation/ScopeTimer.h"

wxBEGIN_EVENT_TABLE(ft::FishFrame, wxFrame)
EVT_CLOSE(ft::FishFrame::OnClose)
EVT_BUTTON(10002, ft::FishFrame::OnPlay)
EVT_BUTTON(10003, ft::FishFrame::OnPause)
EVT_BUTTON(10004, ft::FishFrame::OnFastFoward)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(ft::FishFramePanel, wxPanel)
EVT_PAINT(ft::FishFramePanel::PaintEvent)
wxEND_EVENT_TABLE()

namespace ft
{

	FishFrame::FishFrame(const std::string& videoPath) : wxFrame(nullptr, wxID_ANY, "Inspector", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER),
		m_Cap(videoPath)
	{
		if (m_Cap.isOpened())
		{
			m_VideoAvaliable = true;
			CreateStatusBar();

			m_Cap.read(m_CapFrame);

			m_FrameDuration = (unsigned long)(1000.0 / m_Cap.get(cv::CAP_PROP_FPS));
			m_Width = (int)m_Cap.get(cv::CAP_PROP_FRAME_WIDTH) / 2;
			m_Height = (int)m_Cap.get(cv::CAP_PROP_FRAME_HEIGHT) / 2;

			m_Panel = new FishFramePanel(this);
			m_Panel->SetSize(wxSize(m_Width, m_Height));

			m_PlayBtn = new wxButton(this, 10002, "Play");
			m_PauseBtn = new wxButton(this, 10003, "Pause");
			m_FastFowardBtn = new wxButton(this, 10004, "Fast Foward");

			wxBoxSizer* hSizer = new wxBoxSizer(wxHORIZONTAL);
			wxBoxSizer* vSizer = new wxBoxSizer(wxVERTICAL);

			hSizer->Add(m_PlayBtn, 1, wxEXPAND);
			hSizer->Add(m_PauseBtn, 1, wxEXPAND);
			hSizer->Add(m_FastFowardBtn, 1, wxEXPAND);
			vSizer->Add(m_Panel, m_Height + 30, wxEXPAND);
			vSizer->Add(hSizer, 30, wxEXPAND);

			SetSize(GetEffectiveMinSize() + wxSize(0, 30));
			this->SetSizer(vSizer);
			vSizer->Layout();
		}
	}
	FishFrame::~FishFrame()
	{
	}
	void FishFrame::OnClose(wxCloseEvent& evt)
	{
		wxGetApp().ActivateRenderLoop(false);
		evt.Skip(); // don't stop event, we still want window to close
	}
	void FishFrame::OnPlay(wxCommandEvent& evt)
	{
		if (!m_VideoAvaliable)
		{
			wxLogInfo("There is no video loaded");
			return;
		}
		wxGetApp().ActivateRenderLoop(true);
		m_VideoPlaying = true;
		m_VideoFastFoward = false;
	}
	void FishFrame::OnPause(wxCommandEvent& evt)
	{
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
		if (!m_VideoAvaliable)
		{
			wxLogInfo("There is no video loaded");
			return;
		}
		wxGetApp().ActivateRenderLoop(true);
		m_VideoPlaying = true;
		m_VideoFastFoward = true;
	}
	void FishFrame::Run()
	{
		if (m_VideoPlaying)
		{
			FT_FUNCTION_TIMER_STATUS(microseconds, this);
			m_Cap.read(m_CapFrame); // Main Bottleneck
			if (m_CapFrame.empty())
			{
				wxLogInfo("End of the video");
				m_VideoPlaying = false;
				m_VideoAvaliable = false;
				return;
			}

			if (m_VideoFastFoward)
				return;

			m_DeltaTime = getTickCount() - m_DeltaTime;
			unsigned long deltaTimeMilli = m_FrameDuration - m_DeltaTime;
			if (deltaTimeMilli < m_FrameDuration)
				wxMilliSleep(deltaTimeMilli);
			m_DeltaTime = getTickCount();
		}
	}

	FishFramePanel::FishFramePanel(FishFrame* parent) : wxPanel(parent), m_CapFrame(parent->m_CapFrame)
	{
	}
	void FishFramePanel::PaintEvent(wxPaintEvent& evt)
	{
		wxPaintDC dc(this);
		PaintFunction(dc);
	}
	void FishFramePanel::PaintNow()
	{
		wxClientDC dc(this);
		PaintFunction(dc);
	}
	void FishFramePanel::PaintFunction(wxDC& dc) {
		PrepareDC(dc);
		cv::cvtColor(m_CapFrame, m_ColorCorrected, cv::COLOR_BGR2RGB);
		cv::resize(m_ColorCorrected, m_SizeCorrected, m_SizeCorrected.size(), 0.5, 0.5);
		wxImage image(m_SizeCorrected.cols, m_SizeCorrected.rows, m_SizeCorrected.data, true);
		wxBitmap bitmap = wxBitmap(image);
		dc.DrawBitmap(bitmap, 0, 0, true);
	}
} // namespace ft
