#include "pch.h"
#include "ComponentBank.h"

#include "../RuffEngine/Globals.h"

namespace ecs
{
	// Static members

	std::vector<components::RenderComponent> ComponentBank::m_renderComponents{ globals::TOTAL_COMPONENTS };
	int ComponentBank::m_renderComponentsSize = 0;

	std::vector<components::AnimationComponent> ComponentBank::m_animationComponents{ globals::TOTAL_COMPONENTS };
	int ComponentBank::m_animationComponentsSize = 0;

	// Static methods

	bool ComponentBank::DisconnectAllFromEntity(Entity* entity)
	{
		while (DISCONNECT_COMP(entity, RenderComponent))
		{
		}

		return true;
	}

	bool ComponentBank::ConnectRenderComponent(Entity* entity)
	{
		return ConnectToEntity(entity, m_renderComponents, m_renderComponentsSize);
	}

	bool ComponentBank::DisconnectRenderComponent(Entity* entity)
	{
		return DisconnectFromEntity(entity, m_renderComponents, m_renderComponentsSize);
	}

	bool ComponentBank::ConnectAnimationComponent(Entity* entity)
	{
		return ConnectToEntity(entity, m_animationComponents, m_animationComponentsSize);
	}

	bool ComponentBank::DisconnectAnimationComponent(Entity* entity)
	{
		return DisconnectFromEntity(entity, m_animationComponents, m_animationComponentsSize);
	}
}