#pragma once

#include "../ECSPrimitives/ComponentBase.h"
#include "../ECSPrimitives/Box2f.h"
#include "../ECSPrimitives/Autolist.h"

#include <vector>
#include <unordered_map>
#include <string>

namespace ecs
{
	class Entity;
}

namespace components
{
	struct Animation
	{
		Animation() :
			framesPerRow{ 0 }, totalFrames{ 0 }
		{}
		Animation(const std::string& name,
			const ecs::Box2f& startFrame,
			int framesPerRow,
			int totalFrames) :
			name{ name }, startFrame{ startFrame }, framesPerRow{ framesPerRow }, 
			totalFrames{ totalFrames }
		{}

		std::string name;
		ecs::Box2f startFrame;
		int framesPerRow;
		int totalFrames;
	};

	class AnimationComponent : public ecs::ComponentBase, public ecs::Autolist<AnimationComponent>
	{
	public:
		enum class PingPongStates {
			NONE,
			FORWARD,
			REVERSE
		};

		AnimationComponent(ecs::Entity* parent);

		void Initialize() override;

		void AddAnimation(
			const std::string& name,
			const ecs::Box2f& startFrame,
			int framesPerRow,
			int totalFrames);
		bool PlayAnimation(
			const std::string& name,
			float framesPerSecond,
			bool isPingPong,
			bool isLooping);
		void StopAnimation();
		void Update();
		void SetFrame(
			const ecs::Box2f& frame);
		bool GetIsAnimating() const;
		const std::string& GetCurrentAnimationName() const;

	private:
		std::unordered_map<std::string, Animation> m_animations;
		Animation* m_currentAnimation{ nullptr };
		int m_currentAnimationFrame;
		float m_timePerFrame;
		float m_timeCounter;
		bool m_isLooping;
		PingPongStates m_pingPongState;
	};
}
