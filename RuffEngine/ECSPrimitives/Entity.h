#pragma once

#include <vector>
#include <algorithm>
#include <typeindex>
#include <string>
#include <unordered_set>
#include <memory>

#include "ComponentBase.h"
#include "Transform.h"
#include "../Components/ParticleComponent.h"

namespace systems
{
	class SpawnSystem;
}

namespace ecs
{
	using ComponentVector = std::vector<std::unique_ptr<ComponentBase>>;

	class Entity
	{
		friend class ComponentBank;
	public:
		Entity();

		~Entity();
		
		inline const sf::Vector2f& GetPosition() const
		{
			return m_transform.position;
		}

		inline void SetPosition(float x, float y)
		{
			m_transform.position.x = x;
			m_transform.position.y = y;
		}
		
		inline void SetPosition(const sf::Vector2f& position)
		{
			m_transform.position = position;
		}

		inline float GetRotation() const
		{
			return m_transform.rotation;
		}

		inline void SetRotation(float rotation)
		{
			m_transform.rotation = rotation;
			auto pc = GetComponent<components::ParticleComponent>();
			if (pc != nullptr)
			{
				pc->PostStateSetup();
			}
		}

		inline const sf::Vector2f& GetScale() const
		{
			return m_transform.scale;
		}

		inline void SetScale(float x, float y)
		{
			m_transform.scale.x = x;
			m_transform.scale.y = y;
		}

		inline void SetScale(const sf::Vector2f& scale)
		{
			m_transform.scale = scale;
		}

		inline const Transform& GetTransform() const
		{
			return m_transform;
		}

		inline const std::unordered_set<std::string>& GetTags() const
		{
			return m_tags;
		}

		inline void AddTag(const std::string& tag)
		{
			m_tags.insert(tag);
		}

		inline bool HasTag(const std::string& tag)
		{
			return m_tags.find(tag) != std::end(m_tags);
		}

		inline bool GetIsActive() const
		{
			return m_isActive;
		}

		inline void SetIsActive(bool value)
		{
			m_isActive = value;
		}

		inline const std::string& GetSceneLayer() const
		{
			return m_sceneLayer;
		}

		inline void SetSceneLayer(const std::string& sceneLayer)
		{
			m_sceneLayer = sceneLayer;
		}

		void Despawn();
		
		template<typename T>
		T* const GetComponent()
		{
			std::type_index tIndex{ typeid(T) };

			auto cIter = std::find_if(
				m_components.begin(),
				m_components.end(),
				[&](std::unique_ptr<ComponentBase>& cPtr)
				{
					return std::type_index{ typeid(*cPtr) } == tIndex;
				});

			if (cIter == m_components.end())
			{
				return nullptr;
			}

			return static_cast<T*>(cIter->get());
		}

		template<typename T>
		T* AddComponent()
		{
			m_components.push_back(std::make_unique<T>(this, m_sceneLayer));
			return static_cast<T*>(m_components.back().get());
		}

		static void SetSpawnSystem(systems::SpawnSystem* spawnSystem);
	private:
		ComponentVector m_components;
		Transform m_transform;
		std::unordered_set<std::string> m_tags;
		bool m_isActive;
		std::string m_sceneLayer;
		
		static systems::SpawnSystem* s_spawnSystemPtr;
	};
}