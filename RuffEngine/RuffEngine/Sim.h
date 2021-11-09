#pragma once

#include "SFML/Graphics.hpp"
#include "../ECSPrimitives/ISystem.h"
#include "../Data/EntityFactory.h"
#include "angelscript.h"
#include "../Systems/RenderSystem.h"

#include <memory>
#include <vector>
#include <string>

namespace ruff_engine
{
	struct SimData
	{
		std::string name;
		std::string entitiesDbPath;
		std::string scriptsPath;
		std::vector<systems::RenderSystem::RenderLayer> renderLayers;
	};

	class Sim
	{
	public:
		enum class EntityVectorIndex
		{
			PERSISTENT = 0,
			SCENE_ONLY = 1
		};

		~Sim();
		bool Initialize();
		void Execute();
	private:
		std::shared_ptr<sf::RenderWindow> m_window;
		std::vector<std::unique_ptr<ecs::ISystem>> m_systems;
		std::vector<std::vector<std::unique_ptr<ecs::Entity>>> m_entities;
		std::shared_ptr<data::EntityFactory> m_entityFactory;
		std::unique_ptr<SimData> m_simData;
		asIScriptEngine* m_scriptEngine;

		bool LoadSimData();
		bool MakeSystems();
		bool LoadScene(const std::string& sceneName);
		bool PrepareScriptEngine();
		bool RegisterScriptApi();
		bool CompileScripts();
		bool ValidateScriptStep(int result, const std::string& msg);
	};
};
