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

	const sf::Vector2f& ComponentBase::GetParentPosition() const
	{
		return m_parent->GetPosition();
	}
}