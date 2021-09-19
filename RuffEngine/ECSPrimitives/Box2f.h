#pragma once

#include "SFML/System/Vector2.hpp"

namespace ecs
{
	struct Box2f
	{
		Box2f();
		Box2f(float centerX, float centerY, float halfX, float halfY);
		Box2f(const sf::Vector2f& center, const sf::Vector2f& halfSize);

		sf::Vector2f center;
		sf::Vector2f halfSize;

		float GetLeft() const;
		float GetRight() const;
		float GetTop() const;
		float GetBottom() const;

	};
}