#include "pch.h"
#include "ScriptSystem.h"
#include "../Components/ScriptComponent.h"
#include "../ECSPrimitives/Entity.h"
#include "SpawnSystem.h"

namespace systems
{
	void ScriptSystem::Execute()
	{
		const auto& sceneLayer = SpawnSystem::GetSceneLayer();
		auto sz = ecs::Autolist<components::ScriptComponent>::Size(sceneLayer);
		for (int i = 0; i < sz; ++i)
		{
			components::ScriptComponent* sc =
				ecs::Autolist<components::ScriptComponent>::Get(sceneLayer, i);
			if (sc->GetIsActive() && sc->GetParent()->GetIsActive())
			{
				sc->ExecuteScript();
			}
		}
	}
}