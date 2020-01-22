#include "FishPanel.h"
#include "FishFrame.h"
#include "Instrumentation/ScopeTimer.h"
#include "Instrumentation/Profile.h"

wxBEGIN_EVENT_TABLE(ft::FishPanel, wxPanel)
EVT_PAINT(ft::FishPanel::PaintEvent)
wxEND_EVENT_TABLE()

namespace ft {

	FishPanel::FishPanel(FishFrame* parent) : wxPanel(parent), m_FishFrame(parent)
	{
	}

	void FishPanel::PaintEvent(wxPaintEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		wxPaintDC dc(this);
		PrepareDC(dc);
		PaintFunction(dc);
	}
	void FishPanel::PaintNow()
	{
		FT_PROFILE_FUNCTION();
		wxClientDC dc(this);
		PrepareDC(dc);
		PaintFunction(dc);
	}
	void FishPanel::PaintFunction(wxDC& dc)
	{
		double newWidthFactor = ((double)m_FishFrame->GetClientSize().GetWidth()) / m_FishFrame->m_OriginalFrameSize.width;
		double newHeightFactor = ((double)m_FishFrame->GetClientSize().GetHeight() - m_FishFrame->m_PlayBtn->GetSize().GetHeight()) / m_FishFrame->m_OriginalFrameSize.height;
		double minorDimensionFactor = newWidthFactor < newHeightFactor ? newWidthFactor : newHeightFactor;
		{
			FT_PROFILE_SCOPE("PaintFunction: OpenCV Mat Transformations");
			if (!(m_FishFrame->m_CapFrame.empty()))
				cv::cvtColor(m_FishFrame->m_CapFrame, m_ColorCorrected, cv::COLOR_BGR2RGB);
			cv::resize(m_ColorCorrected, m_SizeCorrected, cv::Size(), minorDimensionFactor, minorDimensionFactor);
		}
		{
			FT_PROFILE_SCOPE("PaintFunction: wxWidgets draw");
			wxImage image(m_SizeCorrected.cols, m_SizeCorrected.rows, m_SizeCorrected.data, true);
			wxBitmap bitmap = wxBitmap(image);
			if (!(m_FishFrame->m_ResizeHandled)) {
				FT_PROFILE_SCOPE("PaintFunction: dc.Clear();");
				dc.Clear();
				m_FishFrame->m_ResizeHandled = true;
			}
			if (minorDimensionFactor == newWidthFactor)
			{
				FT_PROFILE_SCOPE("PaintFunction: dc.DrawBitmap() with m_FrameTopCoord");
				m_FrameTopCoord = (m_FishFrame->GetClientSize().GetHeight() - m_FishFrame->m_PlayBtn->GetSize().GetHeight() - m_SizeCorrected.rows) / 2;
				dc.DrawBitmap(bitmap, 0, m_FrameTopCoord, true);
			}
			else
			{
				FT_PROFILE_SCOPE("PaintFunction: dc.DrawBitmap() with m_FrameLeftCoord");
				m_FrameLeftCoord = (m_FishFrame->GetClientSize().GetWidth() - m_SizeCorrected.cols) / 2;
				dc.DrawBitmap(bitmap, m_FrameLeftCoord, 0, true);
			}
		}
	}

} // namespace ft