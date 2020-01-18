#pragma once
#include "FishPanel.h"

namespace ft {

	class FishFrame : public wxFrame
	{
	public:
		FishFrame(const std::string& videoPath);
		~FishFrame();
		void OnClose(wxCloseEvent& evt);

		inline bool Opened() const { return m_VideoAvaliable; }
		void Run();

		void OnPlay(wxCommandEvent& evt);
		void OnPause(wxCommandEvent& evt);
		void OnFastFoward(wxCommandEvent& evt);
	public:
		FishPanel* m_Panel = nullptr;

		wxButton* m_PlayBtn = nullptr;
		wxButton* m_PauseBtn = nullptr;
		wxButton* m_Start = nullptr;
		wxButton* m_Stop = nullptr;
		wxButton* m_FastFowardBtn = nullptr;

		cv::Mat m_CapFrame;
	protected:
		std::thread* m_FishThread = nullptr;

		cv::VideoCapture m_Cap;
		cv::Size m_FrameSize;

		std::chrono::nanoseconds m_VideoFrameDuration;
		std::chrono::nanoseconds m_SleepDuration;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_FrameTimePoint;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_SleepTimePoint;

		bool m_VideoAvaliable = false;
		bool m_VideoPlaying = false;
		bool m_VideoFastFoward = false;

		wxDECLARE_EVENT_TABLE();
	};

} // namespace ft