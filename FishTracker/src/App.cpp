#include "App.h"
#include "Instrumentation/ScopeTimer.h"
#include "Instrumentation/Profile.h"

wxIMPLEMENT_APP(ft::App);

namespace ft {

	App::App()
	{
#ifdef FT_PLATFORM_LINUX
		XInitThreads();
#endif
	}

	bool App::OnInit()
	{
		FT_PROFILE_BEGIN_SESSION("Initialization", "result.json");
		FT_PROFILE_FUNCTION();
		m_MainFrame = new ft::MainFrame();
		m_MainFrame->Show();

		return true;
	}

} // namespace ft
