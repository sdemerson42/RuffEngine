#pragma once

#include "SFML/System/Vector2.hpp"

namespace ecs
{
	struct Transform
	{
		sf::Vector2f position;
		sf::Vector2f scale;
		float rotation = 0.0f;
	};
}
