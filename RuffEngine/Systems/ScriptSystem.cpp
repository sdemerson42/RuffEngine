#include "pch.h"
#include "ScriptSystem.h"
#include "../Components/ComponentBank.h"

namespace systems
{
	void ScriptSystem::Execute()
	{
		for (int i = 0; i < ecs::ComponentBank::m_scriptComponentsSize; ++i)
		{
			ecs::ComponentBank::m_scriptComponents[i].ExecuteScript();
		}
	}
}