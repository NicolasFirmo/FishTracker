#pragma once
#include "MainFrame.h"

class App : public wxApp
{
private:
	virtual bool OnInit() override;
	void OnIdle(wxIdleEvent& evt);

	void ShowFrameTime();

public:
	void activateRenderLoop(bool on);
private:
	bool render_loop_on;
	MainFrame* m_MainFrame;

	std::chrono::time_point<std::chrono::steady_clock> m_OnIdleBeforeFrame;
	std::chrono::time_point<std::chrono::steady_clock> m_OnIdleAfterFrame;
};

DECLARE_APP(App)