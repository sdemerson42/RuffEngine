#pragma once

#include "SFML/Graphics.hpp"

#include <memory>

namespace ruff_engine
{
	class Sim
	{
	public:
	private:
		std::shared_ptr<sf::RenderWindow> m_window;
	};
};
