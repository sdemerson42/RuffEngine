#include "pch.h"
#include "ComponentBase.h"
#include "Entity.h"

namespace ecs
{
	Entity* const ComponentBase::GetParent()
	{
		return m_parent;
	}
}