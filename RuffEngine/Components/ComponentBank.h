#pragma once

#include <vector>
#include <algorithm>
#include <typeindex>

#include "Components.h"
#include "../ECSPrimitives/Entity.h"
#include "../Logging/Logger.h"

#define CONNECT_COMP(entityPtr, componentType) \
	ecs::ComponentBank::Connect##componentType##(entityPtr)

#define DISCONNECT_COMP(entityPtr, componentType) \
	ecs::ComponentBank::Disconnect##componentType##(entityPtr)

namespace systems
{
	// Forward declare
	class RenderSystem;
}

namespace ecs
{

	class ComponentBank
	{
		friend class systems::RenderSystem;
	public:
		static bool DisconnectAllFromEntity(Entity* entity);
		static bool ConnectRenderComponent(Entity* entity);
		static bool DisconnectRenderComponent(Entity* entity);
	private:
		static std::vector<components::RenderComponent> m_renderComponents;
		static int m_renderComponentsSize;

		template<typename T>
		static bool ConnectToEntity(
			Entity* entity, 
			std::vector<T>& componentVector,
			int& vectorSize)
		{
			if (entity == nullptr)
			{
				util::Logger::Log("Warning: Attempt to connect component to null entity.");
				return false;
			}

			if (componentVector.size() == vectorSize)
			{
				util::Logger::Log("Warning: Component limit reached when trying to connect.");
				return false;
			}

			T* selectedComponent = &componentVector[vectorSize++];
			selectedComponent->m_parent = entity;
			entity->m_components.push_back(selectedComponent);

			return true;
		}

		template<typename T>
		static bool DisconnectFromEntity(
			Entity* entity,
			std::vector<T>& tComponentVector,
			int& vectorSize)
		{
			if (entity == nullptr)
			{
				util::Logger::Log("Warning: Attempt to disconnect component from null entity.");
				return false;
			}

			auto componentType = std::type_index(typeid(T));
			auto& aComponentVector = entity->m_components;

			auto aComponentIter = std::find_if(
				aComponentVector.rbegin(),
				aComponentVector.rend(),
				[&](ComponentBase* compRef)
				{
					return std::type_index(typeid(*compRef)) == componentType;
				});
			if (aComponentIter == aComponentVector.rend())
			{
				return false;
			}

			auto aComponent = static_cast<T*>(*aComponentIter);
			auto bComponent = &tComponentVector[vectorSize - 1];
			
			// Remove aComponent from the entity's reference list
			aComponent->m_parent = nullptr;
			aComponentVector.erase(std::next(aComponentIter).base());

			// Component to disconnect was already the final component.
			if (aComponent == bComponent)
			{
				--vectorSize;
				return true;
			}

			int aIndex = 0;
			while (aIndex < vectorSize)
			{
				if (&tComponentVector[aIndex] == aComponent)
				{
					break;
				}
				++aIndex;
			}

			// Swap component data
			T tempComponent = tComponentVector[aIndex];
			tComponentVector[aIndex] = tComponentVector[vectorSize - 1];
			tComponentVector[vectorSize - 1] = tempComponent;

			// Correct connection for swapped component's parent entity
			auto& bComponentVector = tComponentVector[aIndex].m_parent->m_components;
			auto bComponentIter = std::find(
				bComponentVector.begin(),
				bComponentVector.end(),
				bComponent);

			if (bComponentIter == bComponentVector.end())
			{
				util::Logger::Log("Warning: Attempt to swap a component detached from an Entity.");
				return false;
			}

			*bComponentIter = &tComponentVector[aIndex];
			--vectorSize;

			return true;
		}
	};
}