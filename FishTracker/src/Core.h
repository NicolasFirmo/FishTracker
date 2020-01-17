#pragma once

#ifdef FT_PLATFORM_WINDOWS
	#define DEBUG_BREAK __debugbreak()
#elif FT_PLATFORM_LINUX
	#include <csignal>
	#define DEBUG_BREAK raise(SIGTRAP);
#else
	#error FishTracker only suports Windows and Linux for now!
#endif // FT_PLATFORM

#ifdef FT_ENABLE_ASSERTS
	#define FT_ASSERT(x, error) { if(!(x)) { std::cerr << "Assertion Failed at " << __FILE__ << " - " << __LINE__ << ": " << error << std::endl; DEBUG_BREAK; } }
#else
	#define FT_ASSERT(x, error)
#endif