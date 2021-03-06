#include "FishPanel.h"
#include "FishFrame.h"
#include "Instrumentation/ScopeTimer.h"
#include "Instrumentation/Profile.h"

wxBEGIN_EVENT_TABLE(ft::FishPanel, wxPanel)
EVT_PAINT(ft::FishPanel::PaintEvent)
wxEND_EVENT_TABLE()

namespace ft {

	std::mutex FishPanel::s_PaintingMutex;

	FishPanel::FishPanel(FishFrame* parent) : wxPanel(parent), m_FishFrame(parent) {}

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
		FT_PROFILE_FUNCTION();
		double newWidthFactor = ((double)m_FishFrame->GetClientSize().GetWidth() - m_FishFrame->m_RightPanelWidth) / m_FishFrame->m_OriginalFrameSize.width;
		double newHeightFactor = ((double)m_FishFrame->GetClientSize().GetHeight() - m_FishFrame->m_ButtonHeight) / m_FishFrame->m_OriginalFrameSize.height;
		m_MinorDimensionFactor = newWidthFactor < newHeightFactor ? newWidthFactor : newHeightFactor;
		{
			FT_PROFILE_SCOPE("PaintFunction: OpenCV Mat Transformations");
			if (!(m_FishFrame->m_ToRenderFrame.empty())) {
				FT_PROFILE_SCOPE("OpenCV Mat Transformations: cv::cvtColor");
				cv::cvtColor(m_FishFrame->m_ToRenderFrame, m_ColorCorrected, cv::COLOR_BGR2RGB);
			}
			{
				FT_PROFILE_SCOPE("OpenCV Mat Transformations: cv::resize");
				cv::resize(m_ColorCorrected, m_SizeCorrected, cv::Size(), m_MinorDimensionFactor, m_MinorDimensionFactor);
			}
		}
		{
			FT_PROFILE_SCOPE("PaintFunction: Drawing Tracking Graphics");
			for (auto&& ROI : m_FishFrame->m_ROIs)
				ROI->Render(m_MinorDimensionFactor);
			m_FishFrame->m_Fish->DrawPositionCross(m_MinorDimensionFactor);
			m_FishFrame->m_Fish->DrawScannigArea(m_MinorDimensionFactor);
			if (m_FishFrame->m_Scale)
				m_FishFrame->m_Scale->Render(m_SizeCorrected, m_MinorDimensionFactor);
		}
		{
			FT_PROFILE_SCOPE("PaintFunction: wxWidgets Draw");
			wxImage image(m_SizeCorrected.cols, m_SizeCorrected.rows, m_SizeCorrected.data, true);
			wxBitmap bitmap = wxBitmap(image);
			if (!(m_FishFrame->m_ResizeHandled)) {
				FT_PROFILE_SCOPE("PaintFunction: dc.Clear();");
				s_PaintingMutex.lock();
				dc.Clear();
				s_PaintingMutex.unlock();
				m_FishFrame->m_ResizeHandled = true;
			}
			if (m_MinorDimensionFactor == newWidthFactor)
			{
				FT_PROFILE_SCOPE("PaintFunction: dc.DrawBitmap() with m_FrameTopCoord");
				m_FrameLeftCoord = 0;
				m_FrameTopCoord = (m_FishFrame->GetClientSize().GetHeight() - m_FishFrame->m_ButtonHeight - m_SizeCorrected.rows) / 2;
				FT_ASSERT(m_FishFrame->m_ResizeHandled, "Resize not Handeled!");
				FT_ASSERT(bitmap.GetHeight() <= m_FishFrame->GetClientSize().GetHeight() && bitmap.GetWidth() <= m_FishFrame->GetClientSize().GetWidth(), "Bitmap bigger than ClientSize!");
				s_PaintingMutex.lock();
				dc.DrawBitmap(bitmap, m_FrameLeftCoord, m_FrameTopCoord, true);
				s_PaintingMutex.unlock();
			}
			else
			{
				FT_PROFILE_SCOPE("PaintFunction: dc.DrawBitmap() with m_FrameLeftCoord");
				m_FrameLeftCoord = (m_FishFrame->GetClientSize().GetWidth() - m_FishFrame->m_RightPanelWidth - m_SizeCorrected.cols) / 2;
				m_FrameTopCoord = 0;
				FT_ASSERT(m_FishFrame->m_ResizeHandled, "Resize not Handeled!");
				FT_ASSERT(bitmap.GetHeight() <= m_FishFrame->GetClientSize().GetHeight() && bitmap.GetWidth() <= m_FishFrame->GetClientSize().GetWidth(), "Bitmap bigger than ClientSize!");
				s_PaintingMutex.lock();
				dc.DrawBitmap(bitmap, m_FrameLeftCoord, m_FrameTopCoord, true);
				s_PaintingMutex.unlock();
			}
		}
	}

} // namespace ft