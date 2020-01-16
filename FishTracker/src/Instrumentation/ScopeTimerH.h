#pragma once
#include "../Core.h"

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
		ScopeTimerStatus(const char* scopeName, wxFrame* frame = nullptr);
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
		ScopeTimerOStream(const char* scopeName, std::ostream& os);
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
}

#define FT_PROFILE 1
#if FT_PROFILE
// Resolve which function signature macro will be used. Note that this only
// is resolved when the (pre)compiler starts, so the syntax highlighting
// could mark the wrong one in your editor!
#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
#define FT_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
#define FT_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
#define FT_FUNC_SIG __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#define FT_FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#define FT_FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#define FT_FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
#define FT_FUNC_SIG __func__
#else
#define FT_FUNC_SIG "FT_FUNC_SIG unknown!"
#endif

#define FT_FUNCTION_TIMER_STATUS(duration,...) ft::ScopeTimerStatus<duration> timer##__LINE__(FT_FUNC_SIG,__VA_ARGS__)
#define FT_FUNCTION_TIMER_OSTREAM(duration,os) ft::ScopeTimerOStream<duration> timer##__LINE__(FT_FUNC_SIG,os)

#endif // FT_PROFILE
