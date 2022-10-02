#include "Sim.h"

#include "../Util/Logger.h"
#include "../Components/ScriptComponent.h"

#include "../AngelScript/add_on/scriptbuilder/scriptbuilder.h"
#include "../AngelScript/add_on/scriptmath/scriptmath.h"
#include "../AngelScript/add_on/scriptstdstring/scriptstdstring.h"
#include "../AngelScript/add_on/scriptarray/scriptarray.h"

namespace ruff_engine
{

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

	bool Sim::CompileScripts()
	{
		util::Logger::Log("Compiling scripts...");

		data::SqlQueryResult query;
		auto queryResult = data::SqlQuery::SubmitQuery(
			m_simData->dbPath, "SELECT * FROM Script_Data", query);

		CScriptBuilder builder;
		builder.StartNewModule(m_scriptEngine, "main");

		const auto& scriptFileNames = query.at("script_file_name");
		for (const std::string& fileName : scriptFileNames)
		{
			std::string filePath = m_simData->scriptPath + fileName;
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
			"InputData", 0, asOBJ_REF | asOBJ_NOCOUNT), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectProperty(
			"InputData", "Vector2f leftStick", asOFFSET(systems::InputSystem::InputData, leftStick)), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectProperty(
			"InputData", "Vector2f rightStick", asOFFSET(systems::InputSystem::InputData, rightStick)), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectProperty(
			"InputData", "int button", asOFFSET(systems::InputSystem::InputData, button)), errMsg)) fail = true;

		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectType(
			"Entity", 0, asOBJ_REF | asOBJ_NOCOUNT), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"Entity", "bool HasTag(const string& in)",
			asMETHOD(ecs::Entity, HasTag), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"Entity", "void Despawn()",
			asMETHOD(ecs::Entity, Despawn), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"Entity", "const Vector2f& GetPosition()",
			asMETHOD(ecs::Entity, GetPosition), asCALL_THISCALL), errMsg)) fail = true;

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
			"ScriptComponent", "void PauseSuspend(int = 0)",
			asMETHOD(components::ScriptComponent, PauseSuspend), asCALL_THISCALL), errMsg)) fail = true;
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
			"ScriptComponent", "Entity@ SpawnEntity(const string& in, const string& in, float, float)",
			asMETHOD(components::ScriptComponent, SpawnEntityOnLayer), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "ScriptComponent@ GetScriptFromEntity(Entity@)",
			asMETHOD(components::ScriptComponent, GetScriptFromEntity), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "void Despawn()",
			asMETHOD(components::ScriptComponent, Despawn), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "void SetSceneLayer(const string& in)",
			asMETHOD(components::ScriptComponent, SetSceneLayer), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "void PlaySound(const string& in, int, float, float, float, float, float, bool)",
			asMETHOD(components::ScriptComponent, PlaySound), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "void StopSound(const string& in)",
			asMETHOD(components::ScriptComponent, StopSound), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "void PlayMusic(const string& in, float, bool)",
			asMETHOD(components::ScriptComponent, PlayMusic), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "void StopMusic()",
			asMETHOD(components::ScriptComponent, StopMusic), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "float GetMusicVolume()",
			asMETHOD(components::ScriptComponent, GetMusicVolume), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "void SetMusicVolume(float)",
			asMETHOD(components::ScriptComponent, SetMusicVolume), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "int RandomRangeInt(int, int)",
			asMETHOD(components::ScriptComponent, RandomRangeInt), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "void SetTextString(const string& in)",
			asMETHOD(components::ScriptComponent, SetTextString), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "void ChangeScene(int)",
			asMETHOD(components::ScriptComponent, ChangeScene), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "void AddDynamicTileMap(int, int, int, const string& in, const string& in, const string& in, const string& in, const string& in)",
			asMETHOD(components::ScriptComponent, AddDynamicTileMap), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "string ReadFile(const string& in)",
			asMETHOD(components::ScriptComponent, ReadFile), asCALL_THISCALL), errMsg)) fail = true;
		if (!ValidateScriptStep(m_scriptEngine->RegisterObjectMethod(
			"ScriptComponent", "void SetPause(bool)",
			asMETHOD(components::ScriptComponent, SetPause), asCALL_THISCALL), errMsg)) fail = true;

		if (fail)
		{
			return false;
		}

		util::Logger::Log("Script api successfully registered.");

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
}