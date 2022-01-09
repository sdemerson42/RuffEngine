#include "pch.h"
#include "Components.h"
#include "../Util/Logger.h"
#include "../ECSPrimitives/Entity.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/SpawnSystem.h"
#include "../Systems/SoundSystem.h"
#include "../Util/Time.h"
#include "../Util/Events.h"

namespace components
{
	void ScriptComponent::Suspend(int cycles = 0)
	{
		m_suspendCycleCounter = 0;
		m_suspendCycleTotal = cycles;
		m_mainScriptContext->Suspend();
	}

	void ScriptComponent::Log(const std::string& msg) const
	{
		util::Logger::Log("FROM SCRIPT: " + msg);
	}

	float ScriptComponent::DeltaTime() const
	{
		return util::Time::DeltaTime();
	}

	const sf::Vector2f& ScriptComponent::GetPosition() const
	{
		return m_parent->GetPosition();
	}

	int ScriptComponent::GetInt(const std::string& name)
	{
		return m_apiStateInt[name];
	}

	void ScriptComponent::SetInt(const std::string& name, int value)
	{
		m_apiStateInt[name] = value;
	}

	int ScriptComponent::AddInt(const std::string& name, int value)
	{
		m_apiStateInt[name] += value;
		return m_apiStateInt[name];
	}

	float ScriptComponent::GetFloat(const std::string& name)
	{
		return m_apiStateFloat[name];
	}
	
	void ScriptComponent::SetFloat(const std::string& name, float value)
	{
		m_apiStateFloat[name] = value;
	}

	float ScriptComponent::AddFloat(const std::string& name, float value)
	{
		m_apiStateFloat[name] += value;
		return m_apiStateFloat[name];
	}

	std::string ScriptComponent::GetString(const std::string& name)
	{
		return m_apiStateStr[name];
	}

	void ScriptComponent::SetString(const std::string& name, const std::string& value)
	{
		m_apiStateStr[name] = value;
	}

	std::string ScriptComponent::AddString(const std::string& name, const std::string& value)
	{
		m_apiStateStr[name] += value;
		return m_apiStateStr[name];
	}

	void ScriptComponent::SetPosition(float x, float y)
	{
		m_parent->SetPosition(x, y);
	}

	void ScriptComponent::PlayAnimation(
		const std::string& name,
		float framesPerSecond,
		bool isPingPong,
		bool isLooping)
	{
		auto animationComponent = m_parent->GetComponent<AnimationComponent>();
		animationComponent->PlayAnimation(name, framesPerSecond, isPingPong, isLooping);
	}

	void ScriptComponent::StopAnimation()
	{
		auto animationComponent = m_parent->GetComponent<AnimationComponent>();
		animationComponent->StopAnimation();
	}

	float ScriptComponent::GetRotation() const
	{
		return GetParentTransform().rotation;
	}

	void ScriptComponent::SetRotation(float value)
	{
		m_parent->SetRotation(value);
	}

	const sf::Vector2f& ScriptComponent::GetScale() const
	{
		return GetParentTransform().scale;
	}

	void ScriptComponent::SetScale(float x, float y)
	{
		m_parent->SetScale(x, y);
	}

	const sf::Vector2f& ScriptComponent::GetVelocity() const
	{
		auto physicsComponent = m_parent->GetComponent<PhysicsComponent>();
		if (physicsComponent != nullptr)
		{
			return physicsComponent->GetVelocity();
		}
		return { 0.0f, 0.0f };
	}

	void ScriptComponent::SetVelocity(float x, float y)
	{
		auto physicsComponent = m_parent->GetComponent<PhysicsComponent>();
		if (physicsComponent != nullptr)
		{
			physicsComponent->SetVelocity(x, y);
		}
	}

	const systems::InputSystem::InputData& ScriptComponent::GetInput() const
	{
		return systems::InputSystem::s_inputData;
	}

	const sf::Vector2f& ScriptComponent::GetViewCenter() const
	{
		return systems::RenderSystem::s_view.getCenter();
	}
	
	void ScriptComponent::SetViewCenter(float x, float y)
	{
		systems::RenderSystem::s_view.setCenter(x, y);
	}

	ecs::Entity& ScriptComponent::SpawnEntity(const std::string& name, float x, float y)
	{
		return *m_spawnSystem->TrySpawn(name, x, y, true, false, "");
	}

	ecs::Entity& ScriptComponent::SpawnEntityOnLayer(const std::string& name, const std::string& sceneLayer,
		float x, float y)
	{
		return *m_spawnSystem->TrySpawn(name, sceneLayer, x, y, true, false, "");
	}

	ScriptComponent& ScriptComponent::GetScriptFromEntity(ecs::Entity& entity)
	{
		return *entity.GetComponent<ScriptComponent>();
	}

	void ScriptComponent::Despawn()
	{
		m_spawnSystem->DespawnEntity(GetParent());
	}

	void ScriptComponent::SetSceneLayer(const std::string& layer)
	{
		systems::SpawnSystem::SetSceneLayer(layer);
	}

	void ScriptComponent::PlaySound(const std::string& name, int priority, float volume, 
		float pitch, float positionX, float positionY, float positionZ, bool isLooping)
	{
		m_soundSystem->PlaySound(name, priority, volume, pitch, positionX,
			positionY, positionZ, isLooping);
	}
	void ScriptComponent::StopSound(const std::string& name)
	{
		m_soundSystem->StopSound(name);
	}
	void ScriptComponent::PlayMusic(const std::string& name, float volume, bool isLooping)
	{
		m_soundSystem->PlayMusic(name, volume, isLooping);
	}
	void ScriptComponent::StopMusic()
	{
		m_soundSystem->StopMusic();
	}
	float ScriptComponent::GetMusicVolume()
	{
		return m_soundSystem->GetMusicVolume();
	}
	void ScriptComponent::SetMusicVolume(float value)
	{
		m_soundSystem->SetMusicVolume(value);
	}

	int ScriptComponent::RandomRangeInt(int min, int max)
	{
		int range = max - min + 1;
		int value = rand() % range + min;
		return value;
	}

	void ScriptComponent::SetTextString(const std::string& text)
	{
		auto textComponent = m_parent->GetComponent<components::TextComponent>();
		if (textComponent != nullptr)
		{
			textComponent->SetTextString(text);
		}
	}

	void ScriptComponent::ChangeScene(int sceneId)
	{
		util::ChangeSceneEvent event;
		event.sceneId = sceneId;
		SendEvent(&event);
	}
}