#include "pch.h"
#include "Time.h"

namespace util
{
	sf::Clock Time::s_clock;
	float Time::s_lastFrameTimeAsSeconds = 0.0f;

	void Time::BeginTime()
	{
		s_clock.restart();
	}

	void Time::ResetTime()
	{
		s_lastFrameTimeAsSeconds = s_clock.getElapsedTime().asSeconds();
		s_clock.restart();
	}

	float Time::DeltaTime()
	{
		return s_lastFrameTimeAsSeconds;
	}
}