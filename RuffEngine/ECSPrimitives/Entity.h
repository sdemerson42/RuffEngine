#pragma once

#include <vector>
#include <memory>

#include "ComponentBase.h"

namespace ecs
{
	using ComponentVector = std::vector<ComponentBase*>;

	class Entity
	{
		friend class ComponentBank;
	public:
		~Entity();
	private:
		ComponentVector m_components;
	};
}