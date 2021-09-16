#include "pch.h"
#include "RenderSystem.h"

namespace Systems
{
	RenderSystem::RenderSystem(
		std::vector<components::RenderComponent>* renderComponents,
		sf::RenderWindow* window) :
		m_renderComponents{ renderComponents },
		m_window{ window }
	{
	}

	void RenderSystem::Execute()
	{
		
	}
};