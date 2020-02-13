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

		static std::mutex s_PaintingMutex;
	public:
		FishFrame* m_FishFrame;
		cv::Mat m_ColorCorrected;
		cv::Mat m_SizeCorrected;

		int m_FrameTopCoord;
		int m_FrameLeftCoord;
		double m_MinorDimensionFactor;
	};

} // namespace ft