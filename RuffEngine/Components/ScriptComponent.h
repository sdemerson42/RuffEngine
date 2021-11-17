#pragma once

#include "../ECSPrimitives/ComponentBase.h"
#include "../AngelScript/include/angelscript.h"
#include "SFML/System/Vector2.hpp"
#include "../Systems/InputSystem.h"
#include "../ECSPrimitives/Autolist.h"

#include <string>
#include <unordered_map>

namespace systems
{
	class SpawnSystem;
	class SoundSystem;
}

namespace ecs
{
	class Entity;
}

namespace components
{
	class ScriptComponent : public ecs::ComponentBase, public ecs::Autolist<ScriptComponent>
	{
	public:
		ScriptComponent(ecs::Entity* parent, const std::string& sceneLayer);
		~ScriptComponent();

		void Initialize() override;
		void PrepareScriptContext(asIScriptEngine* scriptEngine, const std::string& mainPrefix);
		void AddSpawnSystem(systems::SpawnSystem* spawnSystem);
		void AddSoundSystem(systems::SoundSystem* soundSystem);
		void ExecuteScript();
		void ExecuteCollision(ecs::Entity* collider);

		// Script API methods
		void Suspend(int cycles);
		void Log(const std::string& msg) const;
		float DeltaTime() const;
		
		int GetInt(const std::string& name);
		void SetInt(const std::string& name, int value);
		int AddInt(const std::string& name, int value);
		float GetFloat(const std::string& name);
		void SetFloat(const std::string& name, float value);
		float AddFloat(const std::string& name, float value);
		std::string GetString(const std::string& name);
		void SetString(const std::string& name, const std::string& value);
		std::string AddString(const std::string& name, const std::string& value);

		const sf::Vector2f& GetPosition() const;
		void SetPosition(float x, float y);
		void PlayAnimation(
			const std::string& name,
			float framesPerSecond,
			bool isPingPong,
			bool isLooping);
		void StopAnimation();
		float GetRotation() const;
		void SetRotation(float value);
		const sf::Vector2f& GetScale() const;
		void SetScale(float x, float y);
		const sf::Vector2f& GetVelocity() const;
		void SetVelocity(float x, float y);
		const systems::InputSystem::InputData& GetInput() const;
		const sf::Vector2f& GetViewCenter() const;
		void SetViewCenter(float x, float y);
		ecs::Entity& SpawnEntity(const std::string& name, float x, float y);
		ecs::Entity& SpawnEntityOnLayer(const std::string& name, const std::string& sceneLayer,
			float x, float y);
		ScriptComponent& GetScriptFromEntity(ecs::Entity& entity);
		void Despawn();
		void SetSceneLayer(const std::string& layer);
		void PlaySound(
			const std::string& name,
			int priority,
			float volume,
			float pitch,
			float positionX,
			float positionY,
			float positionZ,
			bool isLooping);
		void StopSound(const std::string& name);
		void PlayMusic(const std::string& name, float volume, bool isLooping);
		void StopMusic();
		float GetMusicVolume();
		void SetMusicVolume(float value);
	private:
		asIScriptContext* m_mainScriptContext{ nullptr };
		asIScriptContext* m_collisionScriptContext{ nullptr };
		asIScriptFunction* m_mainScriptFunction{ nullptr };
		asIScriptFunction* m_collisionScriptFunction{ nullptr };
		systems::SpawnSystem* m_spawnSystem;
		systems::SoundSystem* m_soundSystem;
		int m_suspendCycleCounter;
		int m_suspendCycleTotal;

		// API State
		std::unordered_map<std::string, int> m_apiStateInt;
		std::unordered_map<std::string, float> m_apiStateFloat;
		std::unordered_map<std::string, std::string> m_apiStateStr;
	};
};
