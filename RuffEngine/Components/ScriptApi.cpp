#include "pch.h"
#include "Components.h"
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

	void ScriptComponent::PlayAnimation(
		const std::string& name,
		float framesPerSecond,
		bool isPingPong,
		bool isLooping)
	{
		auto animationComponent = m_parent->GetComponent<AnimationComponent>();
		animationComponent->PlayAnimation(name, framesPerSecond, isPingPong, isLooping);
	}

	void ScriptComponent::StopAnimation()
	{
		auto animationComponent = m_parent->GetComponent<AnimationComponent>();
		animationComponent->StopAnimation();
	}

	float ScriptComponent::GetRotation() const
	{
		return GetParentTransform().rotation;
	}

	void ScriptComponent::SetRotation(float value)
	{
		m_parent->SetRotation(value);
	}

	const sf::Vector2f& ScriptComponent::GetScale() const
	{
		return GetParentTransform().scale;
	}

	void ScriptComponent::SetScale(float x, float y)
	{
		m_parent->SetScale(x, y);
	}

	const sf::Vector2f& ScriptComponent::GetVelocity() const
	{
		auto physicsComponent = m_parent->GetComponent<PhysicsComponent>();
		if (physicsComponent != nullptr)
		{
			return physicsComponent->GetVelocity();
		}
		return { 0.0f, 0.0f };
	}

	void ScriptComponent::SetVelocity(float x, float y)
	{
		auto physicsComponent = m_parent->GetComponent<PhysicsComponent>();
		if (physicsComponent != nullptr)
		{
			physicsComponent->SetVelocity(x, y);
		}
	}

	const systems::InputSystem::InputData& ScriptComponent::GetInput() const
	{
		return systems::InputSystem::s_inputData;
	}
}