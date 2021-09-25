#include "Sim.h"
#include "../Systems/Systems.h"
#include "../Util/Time.h"
#include "../Util/Logger.h"

#include <cassert>

// Temporary test headers
#include "../Components/AnimationComponent.h"

namespace ruff_engine
{

	Sim::~Sim()
	{
		m_systems.clear();
	}

	bool Sim::Initialize()
	{
		util::Logger::Log("Sim initializing...");

		assert(LoadGameData());

		m_window = std::make_shared<sf::RenderWindow>(sf::VideoMode{ 800, 600 }, m_gameData->name);
		m_entityFactory = std::make_unique<data::EntityFactory>();
		m_entityFactory->Initialize("Demo/Data/Entities.db");

		assert(MakeSystems());
		
		util::Logger::Log("Sim initialized successfully.");

		return true;
	}

	bool Sim::LoadGameData()
	{
		// TO DO: Load from db table
		m_gameData = std::make_unique<GameData>();
		m_gameData->name = "Demo";
		m_gameData->renderLayers.push_back("default");

		return true;
	}

	bool Sim::MakeSystems()
	{
		m_systems.push_back(
			std::make_unique<systems::AnimationSystem>());
		m_systems.push_back(
			std::make_unique<systems::RenderSystem>(m_window, m_gameData->renderLayers));

		return true;
	}

	bool Sim::LoadScene(const std::string& sceneName)
	{
		// TO DO: For now just create some test entities

		m_entities.push_back(
			std::make_unique<ecs::Entity>());
		auto& entity = *m_entities.back().get();

		m_entityFactory->BuildEntityFromBlueprint(
			"Wizard",
			200.0f,
			300.0f,
			entity);

		components::AnimationComponent* animationComponent = 
			entity.GetComponent <components::AnimationComponent>();
		animationComponent->PlayAnimation("walk", 8.0f, true);
		
		return true;
	}

	void Sim::Execute()
	{
		LoadScene("");

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

			for (auto& system : m_systems)
			{
				system->Execute();
			}

			util::Time::ResetTime();
		}
		util::Logger::Log("Sim stopping main loop.");
	}
}