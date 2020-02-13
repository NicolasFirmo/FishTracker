#pragma once

namespace ft {

	class MainFrame;

	class PaintTimer : public wxTimer
	{
		MainFrame* m_MainFrame;
	public:
		PaintTimer(MainFrame* parent);
		void Notify();
		void Start();
	};

} // namespace ft