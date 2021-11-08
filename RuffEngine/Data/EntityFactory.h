#pragma once

#include "Blueprint.h"
#include "../ECSPrimitives/Entity.h"
#include "angelscript.h"

#include <vector>

namespace systems
{
	class SpawnSystem;
}

namespace data
{
	class EntityFactory
	{
	public:
		bool Initialize(const std::string& entityDbPath, asIScriptEngine* scriptEngine,
			systems::SpawnSystem* spawnSystem);
		bool BuildEntityFromBlueprint(
			const std::string& blueprintName,
			const std::string& sceneLayer,
			float positionX,
			float positionY,
			bool isActive,
			/*out*/ecs::Entity& entity);
	private:
		std::vector<Blueprint> m_blueprints;
		asIScriptEngine* m_scriptEngine;
		systems::SpawnSystem* m_spawnSystem;

		std::vector<std::string> ProcessMultiValueField(const std::string& values) const;

		void AddRenderComponents(
			const data::Blueprint& blueprint, 
			/*out*/ecs::Entity& entity);

		void AddAnimationComponents(
			const data::Blueprint& blueprint,
			/*out*/ecs::Entity& entity);

		void AddScriptComponents(
			const data::Blueprint& blueprint,
			/*out*/ecs::Entity& entity);

		void AddPhysicsComponents(
			const data::Blueprint& blueprint,
			/*out*/ecs::Entity& entity);

		void AddParticleComponents(
			const data::Blueprint& blueprint,
			/*out*/ecs::Entity& entity);
	};
}
