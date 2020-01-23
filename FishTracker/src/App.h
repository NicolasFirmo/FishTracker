#pragma once
#include "MainFrame.h"

namespace ft {

	class App : public wxApp
	{
	public:
		App();
	private:
		virtual bool OnInit() override;
		void OnIdle(wxIdleEvent& evt);
	public:
		void ActivateRenderLoop(bool on);
	private:
		bool m_RenderLoopOn = false;
		ft::MainFrame* m_MainFrame;
	};

} // namespace ft

DECLARE_APP(ft::App)