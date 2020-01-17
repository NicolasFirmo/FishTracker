#pragma once

namespace ft {

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
		wxBitmap* m_OutFrame = nullptr;
		cv::Mat m_CapFrame;
	private:
		cv::VideoCapture m_Cap;
		cv::Size m_FrameSize;

		int m_Width;
		int m_Height;
		int m_FrameDuration;

		int m_DeltaTime = (int)cv::getTickCount();

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
		cv::Mat m_CapFrame;
		cv::Mat m_ColorCorrected;
	};
}