#include "pch.h"
#include "ScriptComponent.h"
#include "../Util/Logger.h"
#include "../Systems/SpawnSystem.h"

#include <string>

namespace components
{
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

		m_apiStateInt.clear();
		m_apiStateFloat.clear();
		m_apiStateStr.clear();

		m_suspendCycleCounter = 0;
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
				m_mainScriptContext->Abort();
			}
		}
		functionDecl = "void " + mainPrefix + "_OnCollision(ScriptComponent@ api, Entity@ collider)";
		m_collisionScriptFunction = scriptEngine->GetModule("main")->
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

	void ScriptComponent::ExecuteCollision(ecs::Entity* collider)
	{
		m_collisionScriptContext->Prepare(m_collisionScriptFunction);
		m_collisionScriptContext->SetArgObject(0, this);
		m_collisionScriptContext->SetArgObject(1, collider);
		m_collisionScriptContext->Execute();
	}

	void ScriptComponent::AddSpawnSystem(systems::SpawnSystem* spawnSystem)
	{
		m_spawnSystem = spawnSystem;
	}
}