#pragma once
#include "MainFrame.h"

class App : public wxApp
{
private:
	virtual bool OnInit() override;
	void OnIdle(wxIdleEvent& evt);
public:
	void ActivateRenderLoop(bool on);
private:
	bool render_loop_on = false;
	MainFrame* m_MainFrame;
}; DECLARE_APP(App)