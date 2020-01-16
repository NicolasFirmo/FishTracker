#include "App.h"

wxIMPLEMENT_APP(App);

bool App::OnInit()
{
	render_loop_on = false;

	m_MainFrame = new MainFrame();
	m_MainFrame->Show();

	return true;
}

void App::OnIdle(wxIdleEvent& evt)
{
    m_OnIdleBeforeFrame = std::chrono::steady_clock::now();
    if (render_loop_on)
    {
        m_MainFrame->Render();
        evt.RequestMore(); // render continuously, not only once on idle
    }
    m_OnIdleAfterFrame = std::chrono::steady_clock::now();

    ShowFrameTime();
}

void App::ShowFrameTime()
{
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(m_OnIdleAfterFrame - m_OnIdleBeforeFrame).count();
    wxLogStatus("Frame duration: %lld (%3.2ffps)", duration, 1000000.0f/duration);
}

void App::activateRenderLoop(bool on)
{
    if (on && !render_loop_on)
    {
        Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(App::OnIdle));
        render_loop_on = true;
    }
    else if (!on && render_loop_on)
    {
        Disconnect(wxEVT_IDLE, wxIdleEventHandler(App::OnIdle));
        render_loop_on = false;
    }
}
