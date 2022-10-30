#pragma once

#include "SFML/Graphics.hpp"
#include "../ECSPrimitives/ISystem.h"
#include "../Data/EntityFactory.h"
#include "angelscript.h"
#include "../Data/SimData.h"
#include "../Data/SceneData.h"
#include "../Util/EventHandler.h"
#include "../Util/Events.h"

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace systems
{
	class SpawnSystem;
	class SoundSystem;
	class PhysicsSystem;
}

namespace ruff_engine
{
	class Sim : public util::EventHandler
	{
	public:
		enum class EntityVectorIndex
		{
			PERSISTENT = 0,
			SCENE_ONLY = 1
		};

		Sim();
		~Sim();
		bool Initialize();
		void Execute();
	private:
		std::shared_ptr<sf::RenderWindow> m_window;
		std::vector<std::shared_ptr<ecs::ISystem>> m_systems;
		std::vector<std::shared_ptr<ecs::ISystem>> m_pauseFilteredsystems;
		std::vector<std::vector<std::unique_ptr<ecs::Entity>>> m_entities;
		std::shared_ptr<data::EntityFactory> m_entityFactory;
		std::unique_ptr<data::SimData> m_simData;
		std::unordered_map<int, data::SceneData> m_sceneData;
		
		asIScriptEngine* m_scriptEngine;
		systems::SpawnSystem* m_spawnSystemPtr;
		systems::SoundSystem* m_soundSystemPtr;
		systems::PhysicsSystem* m_physicsSystemPtr;

		int m_currentSceneId;
		int m_nextSceneId;
		bool m_isPaused;

		bool MakeSystems();
		bool ChangeScene();
		bool UnloadScene();
		bool LoadScene(int sceneId);
		bool PrepareScriptEngine();
		bool RegisterScriptApi();
		bool CompileScripts();
		bool ValidateScriptStep(int result, const std::string& msg);

		void OnChangeSceneEvent(const util::ChangeSceneEvent* event);
		void OnSetPauseEvent(const util::SetPauseEvent* event);
		void OnSceneDespawn(const util::SceneDespawnEvent* event);
	};
};
