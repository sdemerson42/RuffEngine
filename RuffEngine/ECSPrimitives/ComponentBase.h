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
		
		virtual void Initialize()
		{}

		Entity* const GetParent();
		const Transform& GetParentTransform() const;

		bool GetIsActive() const
		{
			return m_isActive;
		}
		void SetIsActive(bool value)
		{
			m_isActive = value;
		}
	protected:
		Entity* m_parent;
		bool m_isActive;
	};
}
