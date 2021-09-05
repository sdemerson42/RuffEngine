#pragma once

#include <vector>
#include <memory>

#include "ComponentBase.h"

namespace ecs
{
	using ComponentVector = std::vector<std::shared_ptr<ComponentBase>>;

	class Entity
	{
	public:
	private:
		ComponentVector m_components;
	};
}