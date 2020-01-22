#include "App.h"
#include "Instrumentation/ScopeTimer.h"
#include "Instrumentation/Profile.h"

wxIMPLEMENT_APP(ft::App);

namespace ft {

	bool App::OnInit()
	{
		FT_PROFILE_BEGIN_SESSION("Initialization", "result.json");
		FT_PROFILE_FUNCTION();
		m_MainFrame = new ft::MainFrame();
		m_MainFrame->Show();

		return true;
	}

	void App::OnIdle(wxIdleEvent& evt)
	{
		FT_PROFILE_FUNCTION();
		if (m_RenderLoopOn)
			m_MainFrame->m_FishFrame->m_FishPanel->PaintNow();
	}

	void App::ActivateRenderLoop(bool on)
	{
		FT_PROFILE_FUNCTION();
		if (on && !m_RenderLoopOn)
		{
			Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(App::OnIdle));
			m_RenderLoopOn = true;
		}
		else if (!on && m_RenderLoopOn)
		{
			Disconnect(wxEVT_IDLE, wxIdleEventHandler(App::OnIdle));
			m_RenderLoopOn = false;
		}
	}

} // namespace ft
