#include "ScopeTimer.h"

namespace ft {

	ScopeTimer::ScopeTimer(wxFrame* frame)
		: m_Frame(frame), m_StartTimePoint(std::chrono::steady_clock::now())
	{
	}

	ScopeTimer::~ScopeTimer()
	{
		auto endTimePoint = std::chrono::steady_clock::now();
		auto time = std::chrono::duration_cast<std::chrono::microseconds>(endTimePoint - m_StartTimePoint).count();
		wxLogStatus(m_Frame, "Took: %lldms", time);
	}
}
