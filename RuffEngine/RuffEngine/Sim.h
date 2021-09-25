#pragma once

#include "SFML/Graphics.hpp"
#include "../ECSPrimitives/ISystem.h"
#include "../Data/EntityFactory.h"

#include <memory>
#include <vector>
#include <string>

namespace ruff_engine
{
	struct GameData
	{
		std::string name;
		std::vector<std::string> renderLayers;
	};

	class Sim
	{
	public:
		~Sim();
		bool Initialize();
		void Execute();
	private:
		std::shared_ptr<sf::RenderWindow> m_window;
		std::vector<std::unique_ptr<ecs::ISystem>> m_systems;
		std::vector<std::unique_ptr<ecs::Entity>> m_entities;
		std::unique_ptr<data::EntityFactory> m_entityFactory;
		std::unique_ptr<GameData> m_gameData;

		bool LoadGameData();
		bool MakeSystems();
		bool LoadScene(const std::string& sceneName);
	};
};
