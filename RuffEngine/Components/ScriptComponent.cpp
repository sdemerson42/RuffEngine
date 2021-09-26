#include "pch.h"
#include "ScriptComponent.h"
#include "../Util/Logger.h"

#include <string>

namespace components
{
	void ScriptComponent::PrepareScriptContext(
		asIScriptEngine* scriptEngine,
		const std::string& mainPrefix)
	{
		if (m_scriptContext == nullptr)
		{
			m_scriptContext = scriptEngine->CreateContext();
		}

		std::string functionDecl = "void " + mainPrefix + "_Main(ScriptComponent@ api)";

		asIScriptFunction* mainFunction = scriptEngine->GetModule("main")
			->GetFunctionByDecl(functionDecl.c_str());
		auto result = m_scriptContext->Prepare(mainFunction);

		if (result < 0)
		{
			util::Logger::Log("Warning: Script function with declaration " +
				functionDecl + " could not be prepared.");
			return;
		}

		m_scriptContext->SetArgObject(0, this);
	}

	void ScriptComponent::ExecuteScript()
	{
		if (m_suspendCycleCounter++ < m_suspendCycleTotal)
		{
			return;
		}

		m_scriptContext->Execute();
	}
}