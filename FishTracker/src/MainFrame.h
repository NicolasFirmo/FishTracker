#pragma once
#include "FishFrame.h"

namespace ft {

	class MainFrame : public wxFrame
	{
	public:
		MainFrame();
		void OnClose(wxCloseEvent& evt);

		void OnLoadVideo(wxCommandEvent& evt);
	public:
		FishFrame* m_FishFrame;
		wxButton* m_LoadBtn = nullptr;

		wxDECLARE_EVENT_TABLE();
	};

}