#pragma once

#include <vector>

#include "Components.h"
#include "../ECSPrimitives/Entity.h"

namespace ecs
{

	class ComponentBank
	{
	public:
		static std::vector<RenderComponent>& GetRenderComponents();
	private:
		static std::vector<RenderComponent> m_renderComponents;
		static int m_renderComponentsSize;
	};
}