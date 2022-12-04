#include "pch.h"
#include "ScriptComponent.h"
#include "../Util/Logger.h"
#include "../Systems/SpawnSystem.h"
#include "../Systems/SoundSystem.h"

#include <string>

namespace components
{
	std::vector<std::vector<int>> ScriptComponent::s_tileMaps;
	int ScriptComponent::s_tileMapRowSize;
	sf::Vector2f ScriptComponent::s_tileHalfSize;

	ScriptComponent::ScriptComponent(ecs::Entity* parent,
		const std::string& sceneLayer) :
		ComponentBase{ parent }, Autolist<ScriptComponent>{ sceneLayer }
	{}

	ScriptComponent::~ScriptComponent()
	{
		if (m_mainScriptContext != nullptr) m_mainScriptContext->Release();
		if (m_collisionScriptContext != nullptr) m_collisionScriptContext->Release();
	}

	void ScriptComponent::Initialize()
	{
		auto result = m_mainScriptContext->Prepare(m_mainScriptFunction);
		if (result < 0)
		{
			std::string msg = "Warning: Script function with declaration ";
			msg += m_mainScriptFunction->GetDeclaration();
			msg += " could not be prepared.";
			util::Logger::Log(msg);
			return;
		}
		m_mainScriptContext->SetArgObject(0, this);

		result = m_pauseScriptContext->Prepare(m_pauseScriptFunction);
		if (result >= 0)
		{
			m_pauseScriptContext->SetArgObject(0, this);
		}

		m_apiStateInt.clear();
		m_apiStateFloat.clear();
		m_apiStateStr.clear();

		m_suspendCycleCounter = 0;
		m_pauseSuspendCycleCounter = 0;
	}

	void ScriptComponent::PrepareScriptContext(
		asIScriptEngine* scriptEngine,
		const std::string& mainPrefix)
	{
		if (m_mainScriptContext == nullptr)
		{
			m_mainScriptContext = scriptEngine->CreateContext();
		}
		else
		{
			auto mainState = m_mainScriptContext->GetState();
			if (mainState == asEXECUTION_SUSPENDED || mainState == asEXECUTION_ABORTED)
			{
				m_mainScriptContext->Abort();
			}
		}
		std::string functionDecl = "void " + mainPrefix + "_Main(ScriptComponent@ api)";
		m_mainScriptFunction = scriptEngine->GetModule("main")->
			GetFunctionByDecl(functionDecl.c_str());

		if (m_collisionScriptContext == nullptr)
		{
			m_collisionScriptContext = scriptEngine->CreateContext();
		}
		else
		{
			auto collisionState = m_collisionScriptContext->GetState();
			if (collisionState == asEXECUTION_SUSPENDED || collisionState == asEXECUTION_ABORTED)
			{
				m_collisionScriptContext->Abort();
			}
		}
		functionDecl = "void " + mainPrefix + 
			"_OnCollision(ScriptComponent@ api, Entity@ collider, float xDir, float yDir)";
		m_collisionScriptFunction = scriptEngine->GetModule("main")->
			GetFunctionByDecl(functionDecl.c_str());

		if (m_pauseScriptContext == nullptr)
		{
			m_pauseScriptContext = scriptEngine->CreateContext();
		}
		else
		{
			auto pauseState = m_pauseScriptContext->GetState();
			if (pauseState == asEXECUTION_SUSPENDED || pauseState == asEXECUTION_ABORTED)
			{
				m_pauseScriptContext->Abort();
			}
		}
		functionDecl = "void " + mainPrefix + "_OnPause(ScriptComponent@ api)";
		m_pauseScriptFunction = scriptEngine->GetModule("main")->
			GetFunctionByDecl(functionDecl.c_str());
	}

	void ScriptComponent::ExecuteScript()
	{
		if (m_suspendCycleCounter++ < m_suspendCycleTotal)
		{
			return;
		}

		m_mainScriptContext->Execute();
	}

	void ScriptComponent::ExecuteCollision(ecs::Entity* collider, float xDir, float yDir)
	{
		m_collisionScriptContext->Prepare(m_collisionScriptFunction);
		m_collisionScriptContext->SetArgObject(0, this);
		m_collisionScriptContext->SetArgObject(1, collider);
		m_collisionScriptContext->SetArgFloat(2, xDir);
		m_collisionScriptContext->SetArgFloat(3, yDir);
		m_collisionScriptContext->Execute();
	}

	void ScriptComponent::ExecutePause()
	{
		if (m_pauseSuspendCycleCounter++ < m_pauseSuspendCycleTotal)
		{
			return;
		}

		m_pauseScriptContext->Execute();
	}

	void ScriptComponent::AddSpawnSystem(systems::SpawnSystem* spawnSystem)
	{
		m_spawnSystem = spawnSystem;
	}

	void ScriptComponent::AddSoundSystem(systems::SoundSystem* soundSystem)
	{
		m_soundSystem = soundSystem;
	}
}