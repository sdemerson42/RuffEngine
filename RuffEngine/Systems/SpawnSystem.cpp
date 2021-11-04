#include "pch.h"
#include "SpawnSystem.h"
#include "../Components/ComponentBank.h"

#include <algorithm>

namespace systems
{
	SpawnSystem::SpawnSystem(std::vector<std::unique_ptr<ecs::Entity>>* entities,
		const std::shared_ptr<data::EntityFactory>& factory) :
		m_entities{ entities }, m_entityFactory{ factory }
	{}
		
	void SpawnSystem::Execute()
	{
		// Despawn
		for (ecs::Entity* entity : m_despawnData)
		{
			auto entityIter = std::find_if(m_entities->begin(), m_entities->end(),
				[=](std::unique_ptr<ecs::Entity>& entityUptr)
				{
					return entityUptr.get() == entity;
				});
			if (entityIter != m_entities->end())
			{
				entity->SetIsActive(false);
				ecs::ComponentBank::DisconnectAllFromEntity(entity);
				m_inactiveEntities.push_back(entity);
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
		bool isActive, const std::string& data)
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

			m_entities->push_back(std::make_unique<ecs::Entity>());
			auto& entityUptr = m_entities->back();
			m_entityFactory->BuildEntityFromBlueprint(
				name, x, y, true, *entityUptr);
			return entityUptr.get();
	}

	ecs::Entity* SpawnSystem::TrySpawn(const SpawnData& data)
	{
		return TrySpawn(data.name, data.position.x, data.position.y, data.isActive, data.data);
	}

	void SpawnSystem::EnqueueSpawn(const std::string& name, float x, float y,
		bool isActive, const std::string& data)
	{
		m_spawnData.emplace_back(name, x, y, isActive, data);
	}

	void SpawnSystem::DespawnEntity(ecs::Entity* entity)
	{
		m_despawnData.push_back(entity);
	}
}