#pragma once

#include <vector>
#include <algorithm>
#include <typeindex>

#include "ComponentBase.h"
#include "SFML/System/Vector2.hpp"

namespace ecs
{
	using ComponentVector = std::vector<ComponentBase*>;

	class Entity
	{
		friend class ComponentBank;
	public:
		~Entity();
		
		const sf::Vector2f& GetPosition() const
		{
			return m_position;
		}

		void SetPosition(float x, float y)
		{
			m_position.x = x;
			m_position.y = y;
		}
		void SetPosition(const sf::Vector2f& position)
		{
			m_position = position;
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
		sf::Vector2f m_position;
	};
}