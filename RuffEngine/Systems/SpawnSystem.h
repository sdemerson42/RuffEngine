#pragma once

#include "../ECSPrimitives/ISystem.h"
#include "../ECSPrimitives/Entity.h"
#include "../Data/EntityFactory.h"

#include <vector>
#include <memory>
#include <string>
#include <set>
#include "SFML/System/Vector2.hpp"

namespace systems
{
	// TO DO: Data at spawn
	class SpawnSystem : public ecs::ISystem
	{
	public:
		struct SpawnData
		{
			SpawnData(const std::string& name, const std::string& sceneLayer,
				float x, float y, 
				bool isActive, bool isPersistent, const std::string& data) :
				name{ name }, sceneLayer{ sceneLayer },
				position{ x, y }, isActive{ isActive },
				isPersistent{ isPersistent }, data{ data }
			{}
			std::string name;
			std::string sceneLayer;
			sf::Vector2f position;
			bool isActive;
			bool isPersistent;
			std::string data;
		};

		SpawnSystem(std::vector<std::vector<std::unique_ptr<ecs::Entity>>>* entities,
			const std::shared_ptr<data::EntityFactory>& factory);
		void  Initialize();
		void Execute() override;
		ecs::Entity* TrySpawn(const std::string& name, const std::string& sceneLayer, 
			float x, float y,
			bool isActive, bool isPersistent, const std::string& data);
		ecs::Entity* TrySpawn(const std::string& name, float x, float y,
			bool isActive, bool isPersistent, const std::string& data);
		ecs::Entity* TrySpawn(const SpawnData& data);
		void EnqueueSpawn(const std::string& name, const std::string& sceneLayer,
			float x, float y, bool isActive, bool isPersistent, 
			const std::string& data);
		void DespawnEntity(ecs::Entity* entity);

		static const std::string& GetSceneLayer();
		static void SetSceneLayer(const std::string& sceneLayer);
	private:
		std::vector<std::vector<std::unique_ptr<ecs::Entity>>>* m_entities;
		std::set<ecs::Entity*> m_inactiveEntities;
		std::shared_ptr<data::EntityFactory> m_entityFactory;
		std::vector<SpawnData> m_spawnData;
		std::vector<ecs::Entity*> m_despawnData;

		static std::string s_sceneLayer;
		static std::string s_sceneLayerChange;
	};
}
