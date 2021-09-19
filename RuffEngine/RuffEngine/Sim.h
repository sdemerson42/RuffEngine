#pragma once

#include "SFML/Graphics.hpp"
#include "../ECSPrimitives/ISystem.h"
#include "../Data/EntityFactory.h"

#include <memory>
#include <vector>

namespace ruff_engine
{
	class Sim
	{
	public:
		~Sim();
		void Execute();
	private:
		std::shared_ptr<sf::RenderWindow> m_window;
		std::vector<std::unique_ptr<ecs::ISystem>> m_systems;
		std::unique_ptr<data::EntityFactory> m_entityFactory;
	};
};
