#include "pch.h"
#include "AnimationSystem.h"
#include "../Components/AnimationComponent.h"
#include "../ECSPrimitives/Entity.h"

namespace systems
{
	void AnimationSystem::Execute()
	{
		auto sz = ecs::Autolist<components::AnimationComponent>::Size();
		for (int i = 0; i < sz; ++i)
		{
			components::AnimationComponent* ac =
				ecs::Autolist<components::AnimationComponent>::Get(i);
			if (ac->GetIsActive() && ac->GetParent()->GetIsActive())
			{
				ac->Update();
			}
		}
	}
}