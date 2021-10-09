#pragma once

#include "SFML/System/Vector2.hpp"
#include <math.h>

namespace util
{
	class Math
	{
	public:
		static constexpr double pi = 3.141592653589793238;
		static constexpr double radiansPerDegree = pi / 180.0;
		
		static double DegreesToRadians(double theta)
		{
			return theta * radiansPerDegree;
		}

		static sf::Vector2f Rotate(const sf::Vector2f point, double radians)
		{
			double x = double(point.x);
			double y = double(point.y);

			double xp = x * cos(radians) - y * sin(radians);
			double yp = x * sin(radians) + y * cos(radians);

			return sf::Vector2f{ float(xp), float(yp) };
		}
	protected:
		Math()
		{}
	};
}
