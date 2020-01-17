#pragma once

#ifdef FT_PLATFORM_WINDOWS
	#define DEBUG_BREAK __debugbreak()
	#if FT_WINDOWS_VERSION == 10
		const int FT_TICKFREQ_SHIFT = 13;
	#elif FT_WINDOWS_VERSION == 8
		const int FT_TICKFREQ_SHIFT = 10;
	#else
		#error FishTracker does not suport this windows version!
	#endif // FT_WINDOWS_VERSION
#elif FT_PLATFORM_LINUX
	#include <csignal>
	#define DEBUG_BREAK raise(SIGTRAP);
	const int FT_TICKFREQ_SHIFT = 21;
#else
	#error FishTracker only suports Windows and Linux for now!
#endif // FT_PLATFORM

#ifdef FT_ENABLE_ASSERTS
	#define FT_ASSERT(x, error) { if(!(x)) { std::cerr << "Assertion Failed at " << __FILE__ << " - " << __LINE__ << ": " << error << std::endl; DEBUG_BREAK; } }
#else
	#define FT_ASSERT(x, error)
#endif