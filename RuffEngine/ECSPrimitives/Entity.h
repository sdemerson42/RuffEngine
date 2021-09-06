#pragma once

#include <vector>
#include <algorithm>
#include <typeindex>

#include "ComponentBase.h"

namespace ecs
{
	using ComponentVector = std::vector<ComponentBase*>;

	class Entity
	{
		friend class ComponentBank;
	public:
		~Entity();
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
	private:
		ComponentVector m_components;
	};
}