#pragma once

namespace ft
{

	class FishFramePanel;

	class FishFrame : public wxFrame
	{
	public:
		FishFrame(const std::string& videoPath);
		inline bool Opened() const { return m_VideoAvaliable; }
		~FishFrame();
		void OnClose(wxCloseEvent& evt);

		void OnPlay(wxCommandEvent& evt);
		void OnPause(wxCommandEvent& evt);
		void OnFastFoward(wxCommandEvent& evt);

		void Run();

		wxDECLARE_EVENT_TABLE();
	public:
		FishFramePanel* m_Panel;

		wxButton* m_PlayBtn = nullptr;
		wxButton* m_PauseBtn = nullptr;
		wxButton* m_FastFowardBtn = nullptr;

		cv::Mat m_CapFrame;
	private:
		cv::VideoCapture m_Cap;
		cv::Size m_FrameSize;

		int m_Width;
		int m_Height;

		inline unsigned long getTickCount() { return std::chrono::duration_cast<std::chrono::duration<unsigned long, std::milli>>(std::chrono::steady_clock::now().time_since_epoch()).count(); }
		unsigned long m_FrameDuration;
		unsigned long m_DeltaTime = getTickCount();

		bool m_VideoAvaliable = false;
		bool m_VideoPlaying = false;
		bool m_VideoFastFoward = false;
	};

	class FishFramePanel : public wxPanel
	{
	public:
		FishFramePanel(FishFrame* parent);

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