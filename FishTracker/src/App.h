#pragma once
#include "MainFrame.h"

class App : public wxApp
{
private:
	MainFrame* m_MainFrame;
public:
	App();
	~App();

	virtual bool OnInit() override;
};