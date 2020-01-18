#include "App.h"

wxIMPLEMENT_APP(ft::App);

namespace ft {

	bool App::OnInit()
	{
		m_MainFrame = new ft::MainFrame();
		m_MainFrame->Show();

		return true;
	}

	void App::OnIdle(wxIdleEvent& evt)
	{
		if (m_RenderLoopOn)
		{
			m_MainFrame->m_FishFrame->m_Panel->PaintNow();
			evt.RequestMore(); // render continuously, not only once on idle
		}
	}

	void App::ActivateRenderLoop(bool on)
	{
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
