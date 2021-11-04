#pragma once

#include "../ECSPrimitives/ISystem.h"
#include "../ECSPrimitives/Entity.h"
#include "../Data/EntityFactory.h"

#include <vector>
#include <memory>
#include <string>
#include "SFML/System/Vector2.hpp"

namespace systems
{
	// TO DO: Data at spawn
	class SpawnSystem : public ecs::ISystem
	{
	public:
		struct SpawnData
		{
			SpawnData(const std::string& name, float x, float y, bool isActive, const std::string& data) :
				name{ name }, position{ x, y }, isActive{ isActive }, data{ data }
			{}
			std::string name;
			sf::Vector2f position;
			bool isActive;
			std::string data;
		};

		SpawnSystem(std::vector<std::unique_ptr<ecs::Entity>>* entities,
			const std::shared_ptr<data::EntityFactory>& factory);
		void Execute() override;
		ecs::Entity* TrySpawn(const std::string& name, float x, float y,
			bool isActive, const std::string& data);
		ecs::Entity* TrySpawn(const SpawnData& data);
		void EnqueueSpawn(const std::string& name, float x, float y,
			bool isActive, const std::string& data);
		void DespawnEntity(ecs::Entity* entity);
	private:
		std::vector<std::unique_ptr<ecs::Entity>>* m_entities;
		std::vector<ecs::Entity*> m_inactiveEntities;
		std::shared_ptr<data::EntityFactory> m_entityFactory;
		std::vector<SpawnData> m_spawnData;
		std::vector<ecs::Entity*> m_despawnData;
	};
}
