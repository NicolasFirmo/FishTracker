#include "App.h"

wxIMPLEMENT_APP(App);

bool App::OnInit()
{
    m_MainFrame = new MainFrame();
	m_MainFrame->Show();

	return true;
}

void App::OnIdle(wxIdleEvent& evt)
{
    if (render_loop_on)
    {
        m_MainFrame->Render();
        evt.RequestMore(); // render continuously, not only once on idle
    }
}

void App::ActivateRenderLoop(bool on)
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