#pragma once

#include "SFML/System/Vector2.hpp"
#include "../ECSPrimitives/Transform.h"

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
		const Transform& GetParentTransform() const;
	protected:
		Entity* m_parent;
	};
}
