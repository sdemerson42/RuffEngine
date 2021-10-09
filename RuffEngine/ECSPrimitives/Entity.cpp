#include "pch.h"

#include "Entity.h"
#include "../Components/ComponentBank.h"

namespace ecs
{
	Entity::Entity()
	{
		m_transform.rotation = 0.0f;
		m_transform.scale = { 1.0f, 1.0f };
	}

	Entity::~Entity()
	{
		ComponentBank::DisconnectAllFromEntity(this);
	}
}