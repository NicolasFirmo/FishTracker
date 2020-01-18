#include "FishPanel.h"
#include "FishFrame.h"

wxBEGIN_EVENT_TABLE(ft::FishPanel, wxPanel)
EVT_PAINT(ft::FishPanel::PaintEvent)
wxEND_EVENT_TABLE()

namespace ft {

	FishPanel::FishPanel(FishFrame* parent) : wxPanel(parent), m_CapFrame(parent->m_CapFrame)
	{
	}

	void FishPanel::PaintEvent(wxPaintEvent& evt)
	{
		wxPaintDC dc(this);
		PaintFunction(dc);
	}
	void FishPanel::PaintNow()
	{
		wxClientDC dc(this);
		PaintFunction(dc);
	}
	void FishPanel::PaintFunction(wxDC& dc)
	{
		PrepareDC(dc);
		cv::cvtColor(m_CapFrame, m_ColorCorrected, cv::COLOR_BGR2RGB);
		cv::resize(m_ColorCorrected, m_SizeCorrected, m_SizeCorrected.size(), 0.5, 0.5);
		wxImage image(m_SizeCorrected.cols, m_SizeCorrected.rows, m_SizeCorrected.data, true);
		wxBitmap bitmap = wxBitmap(image);
		dc.DrawBitmap(bitmap, 0, 0, true);
	}

} // namespace ft