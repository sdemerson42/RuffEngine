#include "pch.h"
#include "AnimationComponent.h"
#include "RenderComponent.h"
#include "../Util/Logger.h"
#include "../Util/Time.h"
#include "../ECSPrimitives/Entity.h"

namespace components
{
	AnimationComponent::AnimationComponent(ecs::Entity* parent) :
		ComponentBase{ parent }
	{}

	void AnimationComponent::Initialize()
	{
		m_animations.clear();
		m_currentAnimation = nullptr;
		m_currentAnimationFrame = 0;
		m_timeCounter = 0.0f;
	}

	void AnimationComponent::AddAnimation(
		const std::string& name,
		const ecs::Box2f& startFrame,
		int framesPerRow,
		int totalFrames)
	{
		if (m_animations.find(name) != std::end(m_animations))
		{
			// Warn but continue the overwrite
			util::Logger::Log("Warning: Overwriting animation with name " + name + ".");
		}

		if (framesPerRow <= 0)
		{
			util::Logger::Log("Warning: Can't add animation with framesPerRow " + 
				std::to_string(framesPerRow) + ".");
			return;
		}

		if (totalFrames <= 0)
		{
			util::Logger::Log("Warning: Can't add animation with totalFrames " +
				std::to_string(totalFrames) + ".");
			return;
		}

		m_animations[name] =
			Animation{ name, startFrame, framesPerRow, totalFrames };
	}

	bool AnimationComponent::PlayAnimation(
		const std::string& name, 
		float framesPerSecond, 
		bool isPingPong,
		bool isLooping)
	{
		if (framesPerSecond <= 0.0f)
		{
			util::Logger::Log("Warning: Animation framesPerSecond set to invalid value " +
				std::to_string(framesPerSecond) + ".");
			return false;
		}

		if (m_currentAnimation != nullptr &&
			m_currentAnimation->name == name)
		{
			// No log required, but do not restart the current animation.
			return false;
		}

		auto animationIter = m_animations.find(name);
		if (animationIter == std::end(m_animations))
		{
			util::Logger::Log("Warning: Failed to find animation with name " + name + ".");
			return false;
		}

		m_currentAnimation = &animationIter->second;
		m_currentAnimationFrame = 0;
		m_timeCounter = 0.0f;
		m_timePerFrame = 1.0f / framesPerSecond;
		m_isLooping = isLooping;
		m_pingPongState = isPingPong && m_currentAnimation->totalFrames > 2 ?
			PingPongStates::FORWARD : PingPongStates::NONE;

		SetFrame(m_currentAnimation->startFrame);

		return true;
	}

	void AnimationComponent::StopAnimation()
	{
		m_currentAnimation = nullptr;
	}

	void AnimationComponent::Update()
	{
		if (m_currentAnimation == nullptr)
		{
			return;
		}

		m_timeCounter += util::Time::DeltaTime();
		if (m_timeCounter < m_timePerFrame)
		{
			return;
		}
		m_timeCounter -= m_timePerFrame;

		if (m_pingPongState == PingPongStates::REVERSE)
		{
			if (m_currentAnimationFrame == 0)
			{
				m_currentAnimationFrame = -1;
			}
			else
			{
				m_currentAnimationFrame =
					(m_currentAnimationFrame - 1) % m_currentAnimation->totalFrames;
			}
		}
		else
		{
			m_currentAnimationFrame =
				(m_currentAnimationFrame + 1) % m_currentAnimation->totalFrames;
		}

		if (m_currentAnimationFrame == 0)
		{
			if (m_pingPongState == PingPongStates::FORWARD)
			{
				m_pingPongState = PingPongStates::REVERSE;
				m_currentAnimationFrame = m_currentAnimation->totalFrames - 2;
			}
			else if (m_pingPongState == PingPongStates::NONE && !m_isLooping)
			{
				StopAnimation();
				return;
			}
		}

		if (m_currentAnimationFrame == -1)
		{
			if (!m_isLooping)
			{
				StopAnimation();
				return;
			}

			m_currentAnimationFrame = 1;
			m_pingPongState = PingPongStates::FORWARD;
		}

		const int rowPosition = m_currentAnimationFrame % m_currentAnimation->framesPerRow;
		const int columnPosition = m_currentAnimationFrame / m_currentAnimation->framesPerRow;
		const auto& startFrame = m_currentAnimation->startFrame;

		ecs::Box2f frame{
			startFrame.center.x + (float)rowPosition * startFrame.halfSize.x * 2.0f,
			startFrame.center.y + (float)columnPosition * startFrame.halfSize.y * 2.0f,
			startFrame.halfSize.x,
			startFrame.halfSize.y };

		SetFrame(frame);
	}

	void AnimationComponent::SetFrame(
		const ecs::Box2f& frame)
	{
		ecs::Entity* entity = GetParent();
		auto renderComponent = entity->GetComponent<RenderComponent>();
		if (renderComponent == nullptr)
		{
			util::Logger::Log("Warning: Attempting to set frame with no RenderComponent.");
			return;
		}
		renderComponent->SetTextureBox(frame);
	}

	bool AnimationComponent::GetIsAnimating() const
	{
		return m_currentAnimation != nullptr;
	}

	const std::string& AnimationComponent::GetCurrentAnimationName() const
	{
		return m_currentAnimation == nullptr ? "" : m_currentAnimation->name;
	}
}