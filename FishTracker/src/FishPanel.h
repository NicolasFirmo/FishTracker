#pragma once

namespace ft {

	class FishFrame;

	class FishPanel : public wxPanel
	{
	public:
		FishPanel(FishFrame* parent);

		void Start();
		void Shutdown();

		void PaintEvent(wxPaintEvent& evt);
		void PaintNow();

		DECLARE_EVENT_TABLE()
	private:
		void PaintFunction(wxDC& dc);
		bool m_Painting = false;

		bool m_RunningPainting = false;
		std::unique_ptr<std::thread> m_PaintingThread = nullptr;
	public:
		FishFrame* m_FishFrame;
		cv::Mat m_ColorCorrected;
		cv::Mat m_SizeCorrected;

		int m_FrameTopCoord;
		int m_FrameLeftCoord;
		double m_MinorDimensionFactor;
	};

} // namespace ft