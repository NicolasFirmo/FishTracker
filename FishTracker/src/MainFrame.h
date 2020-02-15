#pragma once
#include "Core.h"
#include "FishFrame.h"
#include "RenderTimer.h"

namespace ft {

	class MainFrame : public wxFrame
	{
	public:
		MainFrame();
		void OnClose(wxCloseEvent& evt);

		void OnLoadVideo(wxCommandEvent& evt);
		wxDECLARE_EVENT_TABLE();
	private:
		RenderTimer* m_Timer;
	public:
		std::vector<FishFrame*> m_FishFrames;
		std::mutex m_FishFramesMutex;

		wxButton* m_LoadBtn = nullptr;
	};

} // namespace ft