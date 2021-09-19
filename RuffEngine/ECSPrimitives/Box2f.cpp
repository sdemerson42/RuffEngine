#include "pch.h"
#include "Box2f.h"

namespace ecs
{
	Box2f::Box2f()
	{
	}
	
	Box2f::Box2f(float centerX, float centerY, float halfX, float halfY) :
		center{ centerX, centerY }, halfSize{ halfX, halfY }
	{
	}

	Box2f::Box2f(const sf::Vector2f& center, const sf::Vector2f& halfSize) :
		center{ center }, halfSize{ halfSize }
	{
	}

	float Box2f::GetLeft() const
	{
		return center.x - halfSize.x;
	}

	float Box2f::GetRight() const
	{
		return center.x + halfSize.x;
	}

	float Box2f::GetTop() const
	{
		return center.y - halfSize.y;
	}

	float Box2f::GetBottom() const
	{
		return center.y + halfSize.y;
	}
}