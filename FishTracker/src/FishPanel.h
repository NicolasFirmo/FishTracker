#pragma once

namespace ft {

	class FishFrame;

	class FishPanel : public wxPanel
	{
	public:
		FishPanel(FishFrame* parent);

		void PaintEvent(wxPaintEvent& evt);
		void PaintNow();

		DECLARE_EVENT_TABLE()
	private:
		void PaintFunction(wxDC& dc);
	private:
		cv::Mat m_CapFrame;
		cv::Mat m_ColorCorrected;
		cv::Mat m_SizeCorrected;
	};

} // namespace ft