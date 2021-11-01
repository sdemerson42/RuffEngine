#pragma once

#include "../ECSPrimitives/ComponentBase.h"
#include "../ECSPrimitives/Box2f.h"

#include <vector>
#include <unordered_map>
#include <string>

namespace components
{
	struct Animation
	{
		Animation() :
			framesPerRow{ 0 }, totalFrames{ 0 }, renderIndex{ 0 }
		{}
		Animation(const std::string& name,
			const ecs::Box2f& startFrame,
			int framesPerRow,
			int totalFrames,
			int renderIndex) :
			name{ name }, startFrame{ startFrame }, framesPerRow{ framesPerRow }, 
			totalFrames{ totalFrames }, renderIndex{ renderIndex }
		{}

		std::string name;
		ecs::Box2f startFrame;
		int framesPerRow;
		int totalFrames;
		int renderIndex;
	};

	class AnimationComponent : public ecs::ComponentBase
	{
	public:
		enum class PingPongStates {
			NONE,
			FORWARD,
			REVERSE
		};

		void Initialize() override;

		void AddAnimation(
			const std::string& name,
			const ecs::Box2f& startFrame,
			int framesPerRow,
			int totalFrames,
			int renderIndex);
		bool PlayAnimation(
			const std::string& name,
			float framesPerSecond,
			bool isPingPong,
			bool isLooping);
		void StopAnimation();
		void Update();
		void SetFrame(
			const ecs::Box2f& frame,
			int renderIndex);
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
