#pragma once

#include "Blueprint.h"
#include "../ECSPrimitives/Entity.h"
#include "angelscript.h"

#include <vector>

namespace systems
{
	class SpawnSystem;
	class SoundSystem;
}

namespace data
{
	class EntityFactory
	{
	public:
		bool Initialize(const std::string& entityDbPath, asIScriptEngine* scriptEngine,
			systems::SpawnSystem* spawnSystem, systems::SoundSystem* soundSystem,
			const std::string& dbPathName);
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
		systems::SoundSystem* m_soundSystem;
		std::string m_dbPathName;

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

		void AddTextComponents(
			const data::Blueprint& blueprint,
			/*out*/ecs::Entity& entity);

		void AddTileComponents(
			const data::Blueprint& blueprint,
			/*out*/ecs::Entity& entity);
	};
}
