#include "pch.h"
#include "ScriptSystem.h"
#include "../Components/ScriptComponent.h"
#include "../ECSPrimitives/Entity.h"

namespace systems
{
	void ScriptSystem::Execute()
	{
		auto sz = ecs::Autolist<components::ScriptComponent>::Size();
		for (int i = 0; i < sz; ++i)
		{
			components::ScriptComponent* sc =
				ecs::Autolist<components::ScriptComponent>::Get(i);
			if (sc->GetIsActive() && sc->GetParent()->GetIsActive())
			{
				sc->ExecuteScript();
			}
		}
	}
}