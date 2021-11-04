#include "Sim.h"
#include "../Systems/Systems.h"
#include "../Util/Time.h"
#include "../Util/Logger.h"
#include "../Components/ScriptComponent.h"
#include "../Data/SqlQuery.h"
#include "SFML/System/Vector2.hpp"
#include "Globals.h"

#include "../AngelScript/add_on/scriptbuilder/scriptbuilder.h"
#include "../AngelScript/add_on/scriptmath/scriptmath.h"
#include "../AngelScript/add_on/scriptstdstring/scriptstdstring.h"
#include "../AngelScript/add_on/scriptarray/scriptarray.h"

#include <cassert>

namespace ruff_engine
{

	Sim::~Sim()
	{
		m_systems.clear();
	}

	bool Sim::Initialize()
	{
		util::Logger::Log("Sim initializing...");

		bool result = LoadSimData();
		if (!result)
		{
			util::Logger::Log("Warning: Sim data failed to load.");
			return false;
		}

		if (!PrepareScriptEngine())
		{
			util::Logger::Log("Warning: Script engine was not prepared successfully.");
			return false;
		}

		m_window = std::make_shared<sf::RenderWindow>(sf::VideoMode{ 800, 600 }, m_simData->name);
		m_entityFactory = std::make_shared<data::EntityFactory>();

		result = MakeSystems();
		if (!result)
		{
			util::Logger::Log("Warning: Sim systems were not created successfully.");
			return false;
		}
		m_entityFactory->Initialize(m_simData->entitiesDbPath, m_scriptEngine, 
			static_cast<systems::SpawnSystem*>(m_systems[0].get()));

		m_entities.reserve(globals::TOTAL_ENTITIES);

		util::Logger::Log("Sim initialized successfully.");
		
		return true;
	}

	bool Sim::LoadSimData()
	{
		util::Logger::Log("Loading Sim data...");

		// TO DO: Load from data
		m_simData = std::make_unique<SimData>();
		m_simData->name = "Demo";
		m_simData->entitiesDbPath = "Demo/Data/Entities.db";
		m_simData->scriptsPath = "Demo/Scripts/";
		m_simData->renderLayers.push_back("default");

		util::Logger::Log("Sim data loaded successfully.");

		return true;
	}

	bool Sim::MakeSystems()
	{
		util::Logger::Log("Creating Systems...");

		m_systems.push_back(
			std::make_unique<systems::SpawnSystem>(&m_entities, m_entityFactory));
		m_systems.push_back(
			std::make_unique<systems::InputSystem>());
		m_systems.push_back(
			std::make_unique<systems::ScriptSystem>());
		m_systems.push_back(
			std::make_unique<systems::AnimationSystem>());
		m_systems.push_back(
			std::make_unique<systems::PhysicsSystem>());
		m_systems.push_back(
			std::make_unique<systems::ParticleSystem>());
		m_systems.push_back(
			std::make_unique<systems::RenderSystem>(m_window, m_simData->renderLayers));
		
		util::Logger::Log("Systems created successfully.");

		return true;
	}

	bool Sim::LoadScene(const std::string& sceneName)
	{
		util::Logger::Log("Loading scene " + sceneName + "...");

		// TO DO: Replace test objects with data

		static_cast<systems::SpawnSystem*>(m_systems[0].get())->
			EnqueueSpawn("Wizard", 100.0f, 100.0f, true, "");

		util::Logger::Log("Scene loaded successfully.");

		return true;
	}

	bool Sim::PrepareScriptEngine()
	{
		util::Logger::Log("Preparing script engine...");

		m_scriptEngine = asCreateScriptEngine();

		// Register add-ons
		RegisterScriptMath(m_scriptEngine);
		RegisterStdString(m_scriptEngine);
		RegisterScriptArray(m_scriptEngine, true);

		if (!RegisterScriptApi())
		{
			util::Logger::Log("Warning: Script api registration failure.");
			return false;
		}

		if (!CompileScripts())
		{
			util::Logger::Log("Warning: Failure to compile scripts.");
			return false;
		}

		util::Logger::Log("Script engine successfully prepared.");

		return true;
	}

	bool Sim::RegisterScriptApi()
	{
		util::Logger::Log("Registering script api...");

		// Register objects

		bool fail = false;
		std::string errMsg = "Warning: Script engine failed to register an object type.";

		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectType(
			"ScriptComponent", 0, asOBJ_REF | asOBJ_NOCOUNT), errMsg)) fail = true;

		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectType(
			"Vector2f", 0, asOBJ_REF | asOBJ_NOCOUNT), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectProperty(
			"Vector2f", "float x", asOFFSET(sf::Vector2f, x)), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectProperty(
			"Vector2f", "float y", asOFFSET(sf::Vector2f, y)), errMsg)) fail = true;

		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectType(
		"InputData", 0, asOBJ_REF | asOBJ_NOCOUNT ), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectProperty(
			"InputData", "Vector2f leftStick", asOFFSET(systems::InputSystem::InputData, leftStick)), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectProperty(
			"InputData", "Vector2f rightStick", asOFFSET(systems::InputSystem::InputData, rightStick)), errMsg)) fail = true;

		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectType(
			"Entity", 0, asOBJ_REF | asOBJ_NOCOUNT), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"Entity", "bool HasTag(const string& in)",
			asMETHOD(ecs::Entity, HasTag), asCALL_THISCALL), errMsg)) fail = true;

		if (fail)
		{
			return false;
		}

		fail = false;
		errMsg = "Warning: Script engine failed to register a method.";

		// Register methods
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "void Log(const string &in)",
			asMETHOD(components::ScriptComponent, Log), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "void Suspend(int = 0)",
			asMETHOD(components::ScriptComponent, Suspend), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "const Vector2f& GetPosition()",
			asMETHOD(components::ScriptComponent, GetPosition), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "void SetPosition(float, float)",
			asMETHOD(components::ScriptComponent, SetPosition), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "float DeltaTime()",
			asMETHOD(components::ScriptComponent, DeltaTime), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "void PlayAnimation(const string &in, float, bool, bool)",
			asMETHOD(components::ScriptComponent, PlayAnimation), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "void StopAnimation()",
			asMETHOD(components::ScriptComponent, StopAnimation), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "float GetRotation()",
			asMETHOD(components::ScriptComponent, GetRotation), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "void SetRotation(float)",
			asMETHOD(components::ScriptComponent, SetRotation), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "const Vector2f& GetScale()",
			asMETHOD(components::ScriptComponent, GetScale), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "void SetScale(float, float)",
			asMETHOD(components::ScriptComponent, SetScale), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "const Vector2f& GetVelocity()",
			asMETHOD(components::ScriptComponent, GetVelocity), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "void SetVelocity(float, float)",
			asMETHOD(components::ScriptComponent, SetVelocity), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "const InputData& GetInput()",
			asMETHOD(components::ScriptComponent, GetInput), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "const Vector2f& GetViewCenter()",
			asMETHOD(components::ScriptComponent, GetViewCenter), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "void SetViewCenter(float, float)",
			asMETHOD(components::ScriptComponent, SetViewCenter), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "int GetInt(const string& in)",
			asMETHOD(components::ScriptComponent, GetInt), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "void SetInt(const string& in, int)",
			asMETHOD(components::ScriptComponent, SetInt), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "int AddInt(const string& in, int)",
			asMETHOD(components::ScriptComponent, AddInt), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "float GetFloat(const string& in)",
			asMETHOD(components::ScriptComponent, GetFloat), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "void SetFloat(const string& in, float)",
			asMETHOD(components::ScriptComponent, SetFloat), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "float AddFloat(const string& in, float)",
			asMETHOD(components::ScriptComponent, AddFloat), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "string GetString(const string& in)",
			asMETHOD(components::ScriptComponent, GetString), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "void SetString(const string& in, const string& in)",
			asMETHOD(components::ScriptComponent, SetString), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "string AddString(const string& in, const string& in)",
			asMETHOD(components::ScriptComponent, AddString), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "Entity@ SpawnEntity(const string& in, float, float)",
			asMETHOD(components::ScriptComponent, SpawnEntity), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "ScriptComponent@ GetScriptFromEntity(Entity@)",
			asMETHOD(components::ScriptComponent, GetScriptFromEntity), asCALL_THISCALL), errMsg)) fail = true;

		if (fail)
		{
			return false;
		}

		util::Logger::Log("Script api successfully registered.");

		return true;
	}

	bool Sim::CompileScripts()
	{
		util::Logger::Log("Compiling scripts...");

		data::SqlQueryResult query;
		auto queryResult = data::SqlQuery::SubmitQuery(
			m_simData->entitiesDbPath, "SELECT * FROM Script_Data", query);

		CScriptBuilder builder;
		builder.StartNewModule(m_scriptEngine, "main");

		const auto& scriptFileNames = query.at("script_file_name");
		for (const std::string& fileName : scriptFileNames)
		{
			std::string filePath = m_simData->scriptsPath + fileName;
			int result = builder.AddSectionFromFile(filePath.c_str());
			if (result < 0)
			{
				util::Logger::Log("Warning: Script could not be added to module: "
				+ filePath + ".");
				return false;
			}
		}

		int result = builder.BuildModule();
		if (result < 0)
		{
			util::Logger::Log("Warning: Main script module could not be built.");
			return false;
		}

		util::Logger::Log("Scripts compiled successfully.");

		return true;
	}

	bool Sim::ValidateScriptStep(int result, const std::string& msg)
	{
		if (result < 0)
		{
			util::Logger::Log(msg);
			return false;
		}
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