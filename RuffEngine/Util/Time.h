#pragma once

#include "SFML/System/Clock.hpp"

namespace util
{
	class Time
	{
	public:
		static void BeginTime();
		static void ResetTime();
		static float DeltaTime();
	private:
		static sf::Clock s_clock;
		static float s_lastFrameTimeAsSeconds;
	};
}
