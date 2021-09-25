#include "pch.h"
#include "AnimationSystem.h"
#include "../Components/ComponentBank.h"

namespace systems
{
	void AnimationSystem::Execute()
	{
		for (int i = 0; i < ecs::ComponentBank::m_animationComponentsSize; ++i)
		{
			ecs::ComponentBank::m_animationComponents[i].Update();
		}
	}
}