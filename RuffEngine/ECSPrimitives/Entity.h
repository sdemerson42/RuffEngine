#pragma once

#include <vector>
#include <algorithm>
#include <typeindex>

#include "ComponentBase.h"
#include "Transform.h"

namespace ecs
{
	using ComponentVector = std::vector<ComponentBase*>;

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

		template<typename T>
		T* const GetComponent()
		{
			std::type_index tIndex{ typeid(T) };

			auto cIter = std::find_if(
				m_components.begin(),
				m_components.end(),
				[&](ComponentBase* cPtr)
				{
					return std::type_index{ typeid(*cPtr) } == tIndex;
				});

			if (cIter == m_components.end())
			{
				return nullptr;
			}

			return static_cast<T*>(*cIter);
		}

		template<typename T>
		std::vector<T*> GetComponents()
		{
			std::vector<T*> returnComponents;
			std::type_index tIndex{ typeid(T) };

			for (auto cPtr : m_components)
			{
				if (std::type_index{ typeid(*cPtr) } == tIndex)
				{
					returnComponents.push_back(static_cast<T*>(cPtr));
				}
			}

			return returnComponents;
		}
	private:
		ComponentVector m_components;
		Transform m_transform;
	};
}