#include "pch.h"
#include "AnimationSystem.h"
#include "../Components/ComponentBank.h"

namespace systems
{
	void AnimationSystem::Execute()
	{
		for (int i = 0; i < ecs::ComponentBank::m_animationComponentsSize; ++i)
		{
			components::AnimationComponent* ac =
				&ecs::ComponentBank::m_animationComponents[i];
			if (ac->GetIsActive() && ac->GetParent()->GetIsActive())
			{
				ecs::ComponentBank::m_animationComponents[i].Update();
			}
		}
	}
}