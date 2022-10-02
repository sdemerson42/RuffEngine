#include "pch.h"
#include "ScriptSystem.h"
#include "../Components/ScriptComponent.h"
#include "../ECSPrimitives/Entity.h"
#include "SpawnSystem.h"

namespace systems
{
	ScriptSystem::ScriptSystem()
	{
		RegisterMethod(this, &ScriptSystem::OnSetPauseEvent);
	}

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
				if (!m_isPaused)
				{
					sc->ExecuteScript();
				}
				else
				{
					sc->ExecutePause();
				}
			}
		}
	}

	void ScriptSystem::OnSetPauseEvent(const util::SetPauseEvent* event)
	{
		m_isPaused = event->value;
	}
}