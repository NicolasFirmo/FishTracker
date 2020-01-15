#include "App.h"

wxIMPLEMENT_APP(App);

App::App() : m_MainFrame(nullptr)
{
}

App::~App()
{
}

bool App::OnInit()
{
	m_MainFrame = new MainFrame();
	m_MainFrame->Show();
	return true;
}
