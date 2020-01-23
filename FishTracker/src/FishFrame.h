#pragma once
#include "FishPanel.h"
#include "ROI.h"

namespace ft {

	class FishFrame : public wxFrame
	{
	public:
		FishFrame(const std::string& videoPath);
		~FishFrame();
		void OnClose(wxCloseEvent& evt);
		void OnSize(wxSizeEvent& evt);

		inline bool Opened() const { return m_VideoAvaliable; }
		void Run();

		void OnPlay(wxCommandEvent& evt);
		void OnPause(wxCommandEvent& evt);
		void OnFastFoward(wxCommandEvent& evt);
		void OnAddROI(wxCommandEvent& evt);
		void OnDeleteROI(wxCommandEvent& evt);
		void OnSelectedROI(wxListEvent& evt);
		void OnDeselectedROI(wxListEvent& evt);

		void OnUnactiveROIMode(wxCommandEvent& evt);
		void OnCountROIMode(wxCommandEvent& evt);
		void OnUncountROIMode(wxCommandEvent& evt);
	public:
		FishPanel* m_FishPanel = nullptr;

		wxButton* m_PlayBtn = nullptr;
		wxButton* m_PauseBtn = nullptr;
		wxButton* m_FastFowardBtn = nullptr;

		wxListView* m_ROILst = nullptr;
		wxTextCtrl* m_ROITxt = nullptr;
		wxButton* m_AddROIBtn = nullptr;
		wxButton* m_DeleteROIBtn = nullptr;
		wxButton* m_UnactiveROIModeBtn = nullptr;
		wxButton* m_CountROIModeBtn = nullptr;
		wxButton* m_UncountROIModeBtn = nullptr;

		cv::VideoCapture m_Cap;
		cv::Mat m_CapFrame;
		cv::Size m_OriginalFrameSize;
		wxSize m_CurrentFrameSize;
		bool m_ResizeHandled = false;

		std::vector<std::unique_ptr<ROI>> m_ROIs;
	protected:
		std::thread* m_FishThread = nullptr;
		std::thread* m_AddROIThread = nullptr;

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