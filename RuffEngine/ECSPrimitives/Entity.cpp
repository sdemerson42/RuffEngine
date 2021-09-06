#include "pch.h"

#include "Entity.h"
#include "../Components/ComponentBank.h"

namespace ecs
{
	Entity::~Entity()
	{
		ComponentBank::DisconnectAllFromEntity(this);
	}
}