#include "ScopeTimerH.h"

namespace ft {

	template<typename D>
	ScopeTimerStatus<D>::ScopeTimerStatus(const char* scopeName, wxFrame* frame)
		: m_ScopeName(scopeName), m_Frame(frame), m_StartTimePoint(std::chrono::steady_clock::now())
	{
	}
	template<typename D>
	ScopeTimerStatus<D>::~ScopeTimerStatus()
	{
		auto endTimePoint = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<D>(endTimePoint - m_StartTimePoint).count();
		wxLogStatus(m_Frame, Message::value(), m_ScopeName, duration);
	}

	template<typename D>
	ScopeTimerOStream<D>::ScopeTimerOStream(const char* scopeName, std::ostream& os)
		: m_ScopeName(scopeName), m_Os(os), m_StartTimePoint(std::chrono::steady_clock::now())
	{
	}
	template<typename D>
	ScopeTimerOStream<D>::~ScopeTimerOStream()
	{
		auto endTimePoint = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<D>(endTimePoint - m_StartTimePoint).count();
		m_Os << m_ScopeName << " Took: " << (int)duration << TimeUnity::value();
	}

} // namespace ft
