#include "Utils.hpp"

#include <chrono>

namespace Utils
{
	long long getCurrentTimeNS()
	{
		auto duration = std::chrono::high_resolution_clock::now().time_since_epoch();
		return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
	}

	float getCurrentTimeMS()
	{
		return static_cast<float>(getCurrentTimeNS()) / 1000000.0f;
	}
}
