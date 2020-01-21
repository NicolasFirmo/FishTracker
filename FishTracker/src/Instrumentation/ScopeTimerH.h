#pragma once
#include "Macros.h"

namespace ft {

	// duration TYPES
	using nanoseconds = std::chrono::nanoseconds;
	using microseconds = std::chrono::microseconds;
	using milliseconds = std::chrono::milliseconds;
	using seconds = std::chrono::seconds;
	using minutes = std::chrono::minutes;
	using hours = std::chrono::hours;

	template<typename D = microseconds>
	class ScopeTimerStatus
	{
	public:
		ScopeTimerStatus(const char* scopeName = "Scope", wxFrame* frame = nullptr);
		~ScopeTimerStatus();
	private:
		const char* m_ScopeName;
		struct Message;
		wxFrame* m_Frame;
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimePoint;
	};

	template<>struct ScopeTimerStatus<nanoseconds>::Message { static constexpr const char* value() { return "%s Took: %lldns"; } };
	template<>struct ScopeTimerStatus<microseconds>::Message { static constexpr const char* value() { return "%s Took: %lldus"; } };
	template<>struct ScopeTimerStatus<milliseconds>::Message { static constexpr const char* value() { return "%s Took: %lldms"; } };
	template<>struct ScopeTimerStatus<seconds>::Message { static constexpr const char* value() { return "%s Took: %llds"; } };
	template<>struct ScopeTimerStatus<minutes>::Message { static constexpr const char* value() { return "%s Took: %lldm"; } };
	template<>struct ScopeTimerStatus<hours>::Message { static constexpr const char* value() { return "%s Took: %lldh"; } };

	template<typename D = microseconds>
	class ScopeTimerOStream
	{
	public:
		ScopeTimerOStream(const char* scopeName = "Scope", std::ostream& os = std::cout);
		~ScopeTimerOStream();
	private:
		const char* m_ScopeName;
		struct TimeUnity;
		std::ostream& m_Os;
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimePoint;
	};

	template<>struct ScopeTimerOStream<nanoseconds>::TimeUnity { static constexpr const char* value() { return "ns\n"; } };
	template<>struct ScopeTimerOStream<microseconds>::TimeUnity { static constexpr const char* value() { return "us\n"; } };
	template<>struct ScopeTimerOStream<milliseconds>::TimeUnity { static constexpr const char* value() { return "ms\n"; } };
	template<>struct ScopeTimerOStream<seconds>::TimeUnity { static constexpr const char* value() { return "s\n"; } };
	template<>struct ScopeTimerOStream<minutes>::TimeUnity { static constexpr const char* value() { return "m\n"; } };
	template<>struct ScopeTimerOStream<hours>::TimeUnity { static constexpr const char* value() { return "h\n"; } };

} // namespace ft

#define FT_TIMER 1
#if FT_TIMER
#define FT_FUNCTION_TIMER_STATUS(duration,...) ::ft::ScopeTimerStatus<duration> timer##__LINE__(FT_FUNC_SIG,__VA_ARGS__)
#define FT_FUNCTION_TIMER_OSTREAM(duration,...) ::ft::ScopeTimerOStream<duration> timer##__LINE__(FT_FUNC_SIG,__VA_ARGS__)
#else
#define FT_FUNCTION_TIMER_STATUS(duration,...) 
#define FT_FUNCTION_TIMER_OSTREAM(duration,...)
#endif // FT_TIMER