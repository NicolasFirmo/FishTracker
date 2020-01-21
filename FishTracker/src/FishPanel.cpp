#include "FishPanel.h"
#include "FishFrame.h"
#include "Instrumentation/ScopeTimer.h"
#include "Instrumentation/Profile.h"

wxBEGIN_EVENT_TABLE(ft::FishPanel, wxPanel)
EVT_PAINT(ft::FishPanel::PaintEvent)
wxEND_EVENT_TABLE()

namespace ft {

	FishPanel::FishPanel(FishFrame* parent) : wxPanel(parent), m_CapFrame(parent->m_CapFrame)
	{
	}

	void FishPanel::PaintEvent(wxPaintEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		wxPaintDC dc(this);
		PaintFunction(dc);
	}
	void FishPanel::PaintNow()
	{
		FT_PROFILE_FUNCTION();
		wxClientDC dc(this);
		PaintFunction(dc);
	}
	void FishPanel::PaintFunction(wxDC& dc)
	{
		{
			FT_PROFILE_SCOPE("PaintFunction: OpenCV Mat Transformations");
			cv::cvtColor(m_CapFrame, m_ColorCorrected, cv::COLOR_BGR2RGB);
			cv::resize(m_ColorCorrected, m_SizeCorrected, m_SizeCorrected.size(), 0.5, 0.5);
		}
		{
			FT_PROFILE_SCOPE("PaintFunction: wxWidgets draw");
			PrepareDC(dc);
			wxImage image(m_SizeCorrected.cols, m_SizeCorrected.rows, m_SizeCorrected.data, true);
			wxBitmap bitmap = wxBitmap(image);
			dc.DrawBitmap(bitmap, 0, 0, true);
		}
	}

} // namespace ft