#include "pch.h"

#include "Entity.h"
#include "../Systems/SpawnSystem.h"

namespace ecs
{
	systems::SpawnSystem* Entity::s_spawnSystemPtr{ nullptr };

	Entity::Entity()
	{
		m_transform.rotation = 0.0f;
		m_transform.scale = { 1.0f, 1.0f };
	}

	Entity::~Entity()
	{
		
	}

	void Entity::Despawn()
	{
		s_spawnSystemPtr->DespawnEntity(this);
	}

	void Entity::SetSpawnSystem(systems::SpawnSystem* spawnSystem)
	{
		s_spawnSystemPtr = spawnSystem;
	}
}