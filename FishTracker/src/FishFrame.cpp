#include "App.h"
#include "Core.h"
#include "FishFrame.h"
#include "Instrumentation/ScopeTimer.h"

namespace ft
{

wxBEGIN_EVENT_TABLE(FishFrame, wxFrame)
		EVT_CLOSE(FishFrame::OnClose)
				EVT_BUTTON(10002, FishFrame::OnPlay)
						EVT_BUTTON(10003, FishFrame::OnPause)
								EVT_BUTTON(10004, FishFrame::OnFastFoward)
										wxEND_EVENT_TABLE()

												FishFrame::FishFrame(const std::string &videoPath) : wxFrame(nullptr, wxID_ANY, "Inspector", wxDefaultPosition, wxSize(320, 240), wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER),
																																						 m_Cap(videoPath)
{
	if (m_Cap.isOpened())
	{
		m_VideoAvaliable = true;
		CreateStatusBar();

		m_Cap.read(m_CapFrame);

		m_FrameDuration = (unsigned)(1000.0 / m_Cap.get(cv::CAP_PROP_FPS));
		m_Width = (int)m_Cap.get(cv::CAP_PROP_FRAME_WIDTH);
		m_Height = (int)m_Cap.get(cv::CAP_PROP_FRAME_HEIGHT);
		SetSize(wxSize(m_Width, m_Height));

		m_Panel = new FishFramePanel(this);
		m_PlayBtn = new wxButton(this, 10002, "Play");
		m_PauseBtn = new wxButton(this, 10003, "Pause");
		m_FastFowardBtn = new wxButton(this, 10004, "Fast Foward");

		wxBoxSizer *hSizer = new wxBoxSizer(wxHORIZONTAL);
		wxBoxSizer *vSizer = new wxBoxSizer(wxVERTICAL);

		hSizer->Add(m_PlayBtn, 1, wxEXPAND);
		hSizer->Add(m_PauseBtn, 1, wxEXPAND);
		hSizer->Add(m_FastFowardBtn, 1, wxEXPAND);
		vSizer->Add(m_Panel, 30, wxEXPAND);
		vSizer->Add(hSizer, 1, wxEXPAND);

		this->SetSizer(vSizer);
		vSizer->Layout();
	}
}
FishFrame::~FishFrame()
{
}
void FishFrame::OnClose(wxCloseEvent &evt)
{
	wxGetApp().ActivateRenderLoop(false);
	evt.Skip(); // don't stop event, we still want window to close
}
void FishFrame::OnPlay(wxCommandEvent &evt)
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
void FishFrame::OnPause(wxCommandEvent &evt)
{
	if (!m_VideoAvaliable)
	{
		wxLogInfo("There is no video loaded");
		return;
	}
	wxGetApp().ActivateRenderLoop(false);
	m_VideoPlaying = false;
}
void FishFrame::OnFastFoward(wxCommandEvent &evt)
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
		unsigned deltaTimeMilli = m_FrameDuration - m_DeltaTime;
		if (deltaTimeMilli < m_FrameDuration)
			wxMilliSleep(deltaTimeMilli);
		m_DeltaTime = getTickCount();
	}
}

wxBEGIN_EVENT_TABLE(FishFramePanel, wxPanel)
		EVT_PAINT(FishFramePanel::PaintEvent)
				wxEND_EVENT_TABLE()

						FishFramePanel::FishFramePanel(FishFrame *parent) : wxPanel(parent), m_CapFrame(parent->m_CapFrame)
{
}
void FishFramePanel::PaintEvent(wxPaintEvent &evt)
{
	wxPaintDC dc(this);
	PrepareDC(dc);
	cv::cvtColor(m_CapFrame, m_ColorCorrected, cv::COLOR_BGR2RGB);
	wxImage image(m_ColorCorrected.cols, m_ColorCorrected.rows, m_ColorCorrected.data, true);
	wxBitmap bitmap = wxBitmap(image);
	dc.DrawBitmap(bitmap, 0, 0, true);
}
void FishFramePanel::PaintNow()
{
	wxClientDC dc(this);
	PrepareDC(dc);
	cv::cvtColor(m_CapFrame, m_ColorCorrected, cv::COLOR_BGR2RGB);
	wxImage image(m_ColorCorrected.cols, m_ColorCorrected.rows, m_ColorCorrected.data, true);
	wxBitmap bitmap = wxBitmap(image);
	dc.DrawBitmap(bitmap, 0, 0, true);
}

} // namespace ft
