#pragma once

namespace ft {
	class ScopeTimer
	{
	public:
		ScopeTimer(wxFrame* frame = nullptr);
		~ScopeTimer();
	private:
		wxFrame* m_Frame;
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimePoint;
	};
}