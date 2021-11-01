#include "pch.h"
#include "ScriptSystem.h"
#include "../Components/ComponentBank.h"

namespace systems
{
	void ScriptSystem::Execute()
	{
		for (int i = 0; i < ecs::ComponentBank::m_scriptComponentsSize; ++i)
		{
			components::ScriptComponent* sc = &ecs::ComponentBank::m_scriptComponents[i];
			if (sc->GetIsActive() && sc->GetParent()->GetIsActive())
			{
				sc->ExecuteScript();
			}
		}
	}
}