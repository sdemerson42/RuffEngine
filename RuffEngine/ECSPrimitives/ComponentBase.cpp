#include "pch.h"
#include "ComponentBase.h"
#include "Entity.h"

namespace ecs
{
	ComponentBase::ComponentBase() :
		m_parent{ nullptr }
	{}

	Entity* const ComponentBase::GetParent()
	{
		return m_parent;
	}
}