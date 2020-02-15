#pragma once

namespace ft {

	class MainFrame;

	class RenderTimer : public wxTimer
	{
		MainFrame* m_MainFrame;
	public:
		RenderTimer(MainFrame* parent);
		void Notify();
		void Start();
	};

} // namespace ft