#include "pch.h"
#include "ComponentBank.h"

#include "../RuffEngine/Globals.h"

namespace ecs
{
	// Static members

	std::vector<RenderComponent> ComponentBank::m_renderComponents{ globals::TOTAL_COMPONENTS };
	int ComponentBank::m_renderComponentsSize = 0;

	// Static methods

	std::vector<RenderComponent>& ComponentBank::GetRenderComponents()
	{
		return m_renderComponents;
	}

}