#include "pch.h"
#include "SpawnSystem.h"
#include "../Util/Logger.h"
#include "../RuffEngine/Sim.h"

#include <algorithm>

namespace systems
{
	SpawnSystem::SpawnSystem(
		std::vector<std::vector<std::unique_ptr<ecs::Entity>>>* entities,
		const std::shared_ptr<data::EntityFactory>& factory) :
		m_entities{ entities }, m_entityFactory{ factory }
	{}
	
	void SpawnSystem::Initialize()
	{
		m_spawnData.clear();
		m_despawnData.clear();
		m_inactiveEntities.clear();
	}

	void SpawnSystem::Execute()
	{
		// Despawn
		for (ecs::Entity* entity : m_despawnData)
		{
			std::vector<std::unique_ptr<ecs::Entity>>* sceneEntities =
				&(*m_entities)[(int)ruff_engine::Sim::EntityVectorIndex::SCENE_ONLY];
			auto entityIter = std::find_if(sceneEntities->begin(), sceneEntities->end(),
				[=](std::unique_ptr<ecs::Entity>& entityUptr)
				{
					return entityUptr.get() == entity;
				});
			if (entityIter != sceneEntities->end())
			{
				entity->SetIsActive(false);
				m_inactiveEntities.push_back(entity);
			}
			else
			{
				util::Logger::Log("Warning: Attempt to despawn persistent Entity.");
			}
		}
		m_despawnData.clear();

		// Spawn
		for (const auto& data : m_spawnData)
		{
			if (data.isActive)
			{
				TrySpawn(data);
			}
		}
		m_spawnData.clear();
	}
	
	ecs::Entity* SpawnSystem::TrySpawn(const std::string& name, float x, float y,
		bool isActive, bool isPersistent, const std::string& data)
	{
		ecs::Entity* foundInactiveEntity = nullptr;
			for (int i = 0; i < m_inactiveEntities.size(); ++i)
			{
				if (m_inactiveEntities[i]->HasTag(name))
				{
					foundInactiveEntity = m_inactiveEntities[i];
					m_inactiveEntities.erase(m_inactiveEntities.begin() + i);
					break;
				}
			}

			if (foundInactiveEntity != nullptr)
			{
				foundInactiveEntity->SetIsActive(true);
				m_entityFactory->BuildEntityFromBlueprint(
					name, x, y, true, *foundInactiveEntity);
				return foundInactiveEntity;
			}

			int persistIndex = isPersistent ?
				(int)ruff_engine::Sim::EntityVectorIndex::PERSISTENT :
				(int)ruff_engine::Sim::EntityVectorIndex::SCENE_ONLY;

			(*m_entities)[persistIndex].push_back(std::make_unique<ecs::Entity>());

			util::Logger::Log("Info: Entity with primary tag " + name + " added to pool.");

			auto& entityUptr = (*m_entities)[persistIndex].back();
			m_entityFactory->BuildEntityFromBlueprint(
				name, x, y, true, *entityUptr);
			return entityUptr.get();
	}

	ecs::Entity* SpawnSystem::TrySpawn(const SpawnData& data)
	{
		return TrySpawn(data.name, data.position.x, data.position.y, data.isActive,
			data.isPersistent, data.data);
	}

	void SpawnSystem::EnqueueSpawn(const std::string& name, float x, float y,
		bool isActive, bool isPersistent, const std::string& data)
	{
		m_spawnData.emplace_back(name, x, y, isActive, isPersistent, data);
	}

	void SpawnSystem::DespawnEntity(ecs::Entity* entity)
	{
		m_despawnData.push_back(entity);
	}
}