#include "pch.h"
#include "AnimationSystem.h"
#include "../Components/AnimationComponent.h"
#include "../Components/TileComponent.h"
#include "../ECSPrimitives/Entity.h"
#include "SpawnSystem.h"

namespace systems
{
	void AnimationSystem::Execute()
	{
		const auto& sceneLayer = SpawnSystem::GetSceneLayer();
		auto sz = 
			ecs::Autolist<components::AnimationComponent>::Size(sceneLayer);
		for (int i = 0; i < sz; ++i)
		{
			components::AnimationComponent* ac =
				ecs::Autolist<components::AnimationComponent>::Get(sceneLayer, i);
			if (ac->GetIsActive() && ac->GetParent()->GetIsActive())
			{
				ac->Update();
			}
		}

		sz =
			ecs::Autolist<components::TileComponent>::Size(sceneLayer);
		for (int i = 0; i < sz; ++i)
		{
			components::TileComponent* tc =
				ecs::Autolist<components::TileComponent>::Get(sceneLayer, i);
			if (tc->GetIsActive() && tc->GetParent()->GetIsActive())
			{
				tc->Update();
			}
		}
	}
}