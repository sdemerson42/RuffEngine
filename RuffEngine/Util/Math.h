#pragma once

#include "SFML/System/Vector2.hpp"
#include <math.h>

namespace util
{
	class Math
	{
	public:
		static constexpr float pi = 3.141592653589793238;
		static constexpr float radiansPerDegree = pi / 180.0f;
		
		static float DegreesToRadians(float theta)
		{
			return theta * radiansPerDegree;
		}

		static sf::Vector2f Rotate(const sf::Vector2f point, float radians)
		{
			float xp = point.x * cos(radians) - point.y * sin(radians);
			float yp = point.x * sin(radians) + point.y * cos(radians);

			return sf::Vector2f{ xp, yp };
		}

		static void NormalizeVector(/*out*/ sf::Vector2f& vector)
		{
			if (vector.x == 0.0f && vector.y == 0.0f)
			{
				return;
			}

			float sumSquares = vector.x * vector.x + vector.y * vector.y;
			float magnitude = sqrt(sumSquares);
			vector.x /= magnitude;
			vector.y /= magnitude;
		}
	protected:
		Math()
		{}
	};
}
