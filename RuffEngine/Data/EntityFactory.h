#pragma once

#include "Blueprint.h"
#include "../ECSPrimitives/Entity.h"

#include <vector>

namespace data
{
	class EntityFactory
	{
	public:
		bool Initialize(const std::string& entityDbPath);
		bool BuildEntityFromBlueprint(
			const std::string& blueprintName,
			float positionX,
			float positionY,
			/*out*/ecs::Entity& entity);
	private:
		std::vector<Blueprint> m_blueprints;

		void AddRenderComponents(
			const data::Blueprint& blueprint, 
			/*out*/ecs::Entity& entity);
	};
}
