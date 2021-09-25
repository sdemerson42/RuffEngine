#include "pch.h"
#include "AnimationComponent.h"
#include "RenderComponent.h"
#include "../Util/Logger.h"
#include "../Util/Time.h"
#include "../ECSPrimitives/Entity.h"

namespace components
{
	void AnimationComponent::AddAnimation(
		const std::string& name,
		const ecs::Box2f& startFrame,
		int framesPerRow,
		int totalFrames,
		int renderIndex)
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
			Animation{ name, startFrame, framesPerRow, totalFrames, renderIndex };
	}

	bool AnimationComponent::PlayAnimation(
		const std::string& name, 
		float framesPerSecond, 
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

		SetFrame(m_currentAnimation->startFrame, m_currentAnimation->renderIndex);

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
		m_currentAnimationFrame =
			(m_currentAnimationFrame + 1) % m_currentAnimation->totalFrames;

		if (m_currentAnimationFrame == 0 && !m_isLooping)
		{
			m_currentAnimation = nullptr;
			return;
		}

		const int rowPosition = m_currentAnimationFrame % m_currentAnimation->framesPerRow;
		const int columnPosition = m_currentAnimationFrame / m_currentAnimation->framesPerRow;
		const auto& startFrame = m_currentAnimation->startFrame;
		const int renderIndex = m_currentAnimation->renderIndex;

		ecs::Box2f frame{
			startFrame.center.x + (float)rowPosition * startFrame.halfSize.x * 2.0f,
			startFrame.center.y + (float)columnPosition * startFrame.halfSize.y * 2.0f,
			startFrame.halfSize.x,
			startFrame.halfSize.y };

		SetFrame(frame, renderIndex);
	}

	void AnimationComponent::SetFrame(
		const ecs::Box2f& frame,
		int renderIndex)
	{
		ecs::Entity* entity = GetParent();
		auto renderComponents = entity->GetComponents<RenderComponent>();

		if (renderIndex >= renderComponents.size())
		{
			util::Logger::Log("Warning: Can't set frame to out-of-range renderIndex " +
				std::to_string(renderIndex) + ".");
			return;
		}

		RenderComponent* renderComponent = renderComponents[renderIndex];
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