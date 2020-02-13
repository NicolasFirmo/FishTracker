#pragma once
#include "MainFrame.h"

namespace ft {

	class App : public wxApp
	{
	public:
		App();
	private:
		virtual bool OnInit() override;
	private:
		ft::MainFrame* m_MainFrame;
	};

} // namespace ft

DECLARE_APP(ft::App)