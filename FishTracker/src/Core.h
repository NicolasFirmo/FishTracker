#pragma once

#ifdef FT_PLATFORM_WINDOWS
	#ifdef NDEBUG
		#define MAIN WinMain
	#else
		#define MAIN main
	#endif // NDEBUG
	#define DEBUG_BREAK __debugbreak()

	#define PLATFORM "Windows"
#elif FT_PLATFORM_LINUX
	#include <csignal>
	#define MAIN main
	#define DEBUG_BREAK raise(SIGTRAP);

	#define PLATFORM "Linux"
#else
	#error FishTracker only suports Windows and Linux for now!
#endif // FT_PLATFORM

#ifdef FT_ENABLE_ASSERTS
	#define FT_ASSERT(x, error) { if(!(x)) { std::cerr << "Assertion Failed at " << __FILE__ << " - " << __LINE__ << ": " << error << std::endl; DEBUG_BREAK; } }
#else
	#define FT_ASSERT(x, error)
#endif