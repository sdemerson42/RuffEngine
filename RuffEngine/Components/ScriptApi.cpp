#include "pch.h"
#include "ScriptComponent.h"
#include "../Util/Logger.h"
#include "../ECSPrimitives/Entity.h"
#include "../Util/Time.h"

namespace components
{
	void ScriptComponent::Suspend(int cycles = 0)
	{
		m_suspendCycleCounter = 0;
		m_suspendCycleTotal = cycles;
		m_scriptContext->Suspend();
	}

	void ScriptComponent::Log(const std::string& msg) const
	{
		util::Logger::Log("FROM SCRIPT: " + msg);
	}

	float ScriptComponent::DeltaTime() const
	{
		return util::Time::DeltaTime();
	}

	const sf::Vector2f& ScriptComponent::GetPosition() const
	{
		return m_parent->GetPosition();
	}

	void ScriptComponent::SetPosition(float x, float y)
	{
		m_parent->SetPosition(x, y);
	}
}