#include "Sim.h"
#include "../Systems/Systems.h"
#include "../Util/Time.h"
#include "../Util/Logger.h"
#include "../Data/SqlQuery.h"
#include "../Data/Parse.h"
#include "SFML/System/Vector2.hpp"
#include "Globals.h"

#include <cassert>
#include <typeindex>

namespace ruff_engine
{

	Sim::Sim()
	{
		RegisterMethod(this, &Sim::OnChangeSceneEvent);
		RegisterMethod(this, &Sim::OnSetPauseEvent);
		m_nextSceneId = -1;
		m_isPaused = false;
	}

	Sim::~Sim()
	{
		m_systems.clear();
	}

	bool Sim::Initialize()
	{
		util::Logger::Log("Sim initializing...");

		m_simData = std::make_unique<data::SimData>();
		bool result = data::Parse::LoadSimData(*m_simData);
		if (!result)
		{
			return false;
		}
		else
		{
			util::Logger::Log("Sim data loaded successfully.");
		}

		result = data::Parse::LoadSceneData(m_simData->dbPath, m_sceneData);
		if (!result)
		{
			return false;
		}
		else
		{
			util::Logger::Log("Scene data loaded successfully.");
		}

		if (!PrepareScriptEngine())
		{
			util::Logger::Log("Warning: Script engine was not prepared successfully.");
			return false;
		}

		m_window = std::make_shared<sf::RenderWindow>(
			sf::VideoMode{ m_simData->winX, m_simData->winY },
			m_simData->name);
		m_entityFactory = std::make_shared<data::EntityFactory>();
		m_entities.resize(2);	// Persistent and scene-only entity vectors

		result = MakeSystems();
		if (!result)
		{
			util::Logger::Log("Warning: Sim systems were not created successfully.");
			return false;
		}
		m_entityFactory->Initialize(m_simData->dbPath, m_scriptEngine,
			m_spawnSystemPtr, m_soundSystemPtr, m_simData->dbPath);

		m_entities.reserve(globals::TOTAL_ENTITIES);

		m_nextSceneId = m_simData->startScene;

		util::Logger::Log("Sim initialized successfully.");
		
		return true;
	}

	bool Sim::MakeSystems()
	{
		util::Logger::Log("Creating Systems...");

		m_systems.push_back(
			std::make_shared<systems::SpawnSystem>(&m_entities, m_entityFactory));
		m_pauseFilteredsystems.emplace_back(m_systems.back());

		m_spawnSystemPtr = static_cast<systems::SpawnSystem*>(m_systems.back().get());
		ecs::Entity::SetSpawnSystem(m_spawnSystemPtr);

		m_systems.push_back(
			std::make_shared<systems::InputSystem>());
		m_pauseFilteredsystems.emplace_back(m_systems.back());

		m_systems.push_back(
			std::make_shared<systems::ScriptSystem>());
		m_pauseFilteredsystems.emplace_back(m_systems.back());

		m_systems.push_back(
			std::make_shared<systems::AnimationSystem>());

		m_systems.push_back(
			std::make_shared<systems::PhysicsSystem>());
		m_physicsSystemPtr = static_cast<systems::PhysicsSystem*>(m_systems.back().get());

		m_systems.push_back(
			std::make_shared<systems::ParticleSystem>());

		m_systems.push_back(
			std::make_shared<systems::SoundSystem>(m_simData->soundPath, m_simData->soundBuffers));
		m_pauseFilteredsystems.emplace_back(m_systems.back());

		m_soundSystemPtr = static_cast<systems::SoundSystem*>(m_systems.back().get());

		m_systems.push_back(
			std::make_shared<systems::RenderSystem>(m_window, m_simData->renderLayers));
		m_pauseFilteredsystems.emplace_back(m_systems.back());
		
		util::Logger::Log("Systems created successfully.");

		return true;
	}

	bool Sim::ChangeScene()
	{
		std::string errMsg{ "Could not successfully change scene." };

		if (!UnloadScene())
		{
			util::Logger::Log(errMsg);
			return false;
		}

		if (!LoadScene(m_nextSceneId))
		{
			util::Logger::Log(errMsg);
			return false;
		}

		m_nextSceneId = -1;

		return true;
	}

	bool Sim::UnloadScene()
	{
		util::Logger::Log("Unloading current scene...");

		// Clear entities

		m_entities[(int)EntityVectorIndex::SCENE_ONLY].clear();

		util::Logger::Log("Current scene unloaded successfully.");

		return true;
	}

	bool Sim::LoadScene(int sceneId)
	{
		util::Logger::Log("Loading scene " + std::to_string(sceneId) + "...");

		m_spawnSystemPtr->Initialize();

		std::vector<int> activeSceneIds;
		activeSceneIds.push_back(sceneId);
		const auto& mainScene = m_sceneData.at(sceneId);

		for (int subId : mainScene.subScenes)
		{
			activeSceneIds.push_back(subId);
		}

		for (int currentId : activeSceneIds)
		{
			std::string sceneLayer = currentId == sceneId ?
				"main" : "sub" + std::to_string(currentId);
			auto& currentScene = m_sceneData.at(currentId);

			for (auto& ed : currentScene.entityData)
			{
				for (int i = 0; i < ed.count; ++i)
				{
					if (ed.isPersistent && ed.isCreated)
					{
						continue; // Do not respawn persistend entities.
					}

					m_spawnSystemPtr->EnqueueSpawn(
						ed.name, sceneLayer, ed.x, ed.y, ed.isActive, ed.isPersistent, ed.initData);

					if (ed.isPersistent)
					{
						ed.isCreated = true;
					}
				}
			}
		}

		m_spawnSystemPtr->SetSceneLayer("main");
		m_spawnSystemPtr->Execute();
		m_physicsSystemPtr->Initialize();

		return true;
	}

	void Sim::Execute()
	{
		// Main engine loop

		util::Logger::Log("Sim beginning main loop.");

		util::Time::BeginTime();

		while (m_window->isOpen())
		{
			sf::Event event;
			while (m_window->pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					m_window->close();
				}
			}

			// Change scene before running systems

			if (m_nextSceneId != -1)
			{
				if (!ChangeScene())
				{
					util::Logger::Log("Scene change failure. Aborting main loop.");
					return;
				}
			}

			auto& systemGroup = m_isPaused ? m_pauseFilteredsystems : m_systems;

			for (auto& system : systemGroup)
			{
				system->Execute();
			}
			util::Time::ResetTime();
		}
		util::Logger::Log("Sim stopping main loop.");
	}

	void Sim::OnChangeSceneEvent(const util::ChangeSceneEvent* event)
	{
		m_nextSceneId = event->sceneId;
	}

	void Sim::OnSetPauseEvent(const util::SetPauseEvent* event)
	{
		m_isPaused = event->value;
	}
}