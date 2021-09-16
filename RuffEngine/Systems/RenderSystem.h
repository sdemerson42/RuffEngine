#pragma once

#include "../ECSPrimitives/ISystem.h"
#include "../Components/ComponentBank.h"
#include "SFML/Graphics.hpp"

#include <memory>

namespace Systems
{
	class RenderSystem : public ecs::ISystem
	{
	public:
		RenderSystem(
			std::vector<components::RenderComponent>* renderComponents,
			sf::RenderWindow* window);
		void Execute() override;
	private:
		std::shared_ptr<sf::RenderWindow> m_window;
		std::vector<components::RenderComponent>* m_renderComponents;
	};
};
