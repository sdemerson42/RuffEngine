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

	std::vector<components::ScriptComponent> ComponentBank::m_scriptComponents{ globals::TOTAL_COMPONENTS };
	int ComponentBank::m_scriptComponentsSize = 0;

	std::vector<components::PhysicsComponent> ComponentBank::m_physicsComponents{ globals::TOTAL_COMPONENTS };
	int ComponentBank::m_physicsComponentsSize = 0;

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

	bool ComponentBank::ConnectScriptComponent(Entity* entity)
	{
		return ConnectToEntity(entity, m_scriptComponents, m_scriptComponentsSize);
	}

	bool ComponentBank::DisconnectScriptComponent(Entity* entity)
	{
		return DisconnectFromEntity(entity, m_scriptComponents, m_scriptComponentsSize);
	}

	bool ComponentBank::ConnectPhysicsComponent(Entity* entity)
	{
		return ConnectToEntity(entity, m_physicsComponents, m_physicsComponentsSize);
	}

	bool ComponentBank::DisconnectPhysicsComponent(Entity* entity)
	{
		return DisconnectFromEntity(entity, m_physicsComponents, m_physicsComponentsSize);
	}
}