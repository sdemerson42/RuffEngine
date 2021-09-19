#pragma once

#include "SFML/System/Vector2.hpp"

namespace ecs
{
	class Entity;

	class ComponentBase
	{
		friend class ComponentBank;
	public:
		ComponentBase();
		virtual ~ComponentBase()
		{}
		Entity* const GetParent();
		const sf::Vector2f& GetParentPosition() const;
	protected:
		Entity* m_parent;
	};
}
