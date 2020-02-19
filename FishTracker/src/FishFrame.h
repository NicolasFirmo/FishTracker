#pragma once
#include "FishPanel.h"
#include "Scale.h"
#include "SetScaleLenFrame.h"
#include "ROI.h"
#include "Tracking.h"

namespace ft {

	class MainFrame;

	class FishFrame : public wxFrame
	{
	public:
		FishFrame(MainFrame* parent, const std::string& videoPath);
		~FishFrame();
		inline void SetIndex(size_t idx) { m_Index = idx; }
		inline bool Opened() const { return m_VideoAvaliable; }
		void Update();

	private:
		void OnClose(wxCloseEvent& evt);
		void OnSize(wxSizeEvent& evt);

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

		void OnSumThresholdChange(wxCommandEvent& evt);
		void OnMovementThresholdChange(wxCommandEvent& evt);

		void OnSetScale(wxCommandEvent& evt);

	public:
		static const int m_RightPanelWidth;
		static const int m_ButtonHeight;

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

		wxSlider* m_SliderSumThreshold = nullptr;
		wxSlider* m_SliderMovementThreshold = nullptr;

		wxButton* m_SetScaleBtn = nullptr;

		cv::Mat m_ToRenderFrame;
		cv::Size m_OriginalFrameSize;
		bool m_ResizeHandled = false;

		std::vector<std::unique_ptr<ROI>> m_ROIs;
		std::unique_ptr<Target> m_Fish;
		std::unique_ptr<Scale> m_Scale;
		cv::Rect m_BackgroundUpdateRect;

		bool m_Closing = false;
		// Thread Events
		bool m_EndOfTheVideoHandled = true;
		bool m_FinishedAddingROIHandled = true;
		bool m_FinishedSettingScaleHandled = true;
		bool m_SetLineLenghtHandled = true;
		// -------------
	private:
		MainFrame* m_Parent;
		size_t m_Index;

		cv::VideoCapture m_Cap;
		cv::Mat m_CapFrame;

		std::unique_ptr<std::thread> m_FishThread = nullptr;
		std::unique_ptr<std::thread> m_AddROIThread = nullptr;
		std::unique_ptr<std::thread> m_SetScaleThread = nullptr;

		SetScaleLenFrame* m_SetScaleLenFrame = nullptr;

		std::chrono::nanoseconds m_VideoFrameDuration;
		std::chrono::nanoseconds m_SleepDuration;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_FrameTimePoint;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_SleepTimePoint;

		bool m_VideoAvaliable = false;
		bool m_VideoPlaying = false;
		bool m_VideoFastFoward = false;
		int64_t m_CurrentFrameDuration = 0;

		wxDECLARE_EVENT_TABLE();
	};

} // namespace ft