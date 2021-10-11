#pragma once

#include "../ECSPrimitives/ISystem.h"
#include "../ECSPrimitives/Box2f.h"
#include "SFML/System/Vector2.hpp"

#include <vector>
#include <string>

namespace components
{
	// Forward declare
	class PhysicsComponent;
};

namespace systems
{
	class PhysicsSystem : public ecs::ISystem
	{
	public:
		PhysicsSystem();
		void Initialize();
		void Execute() override;
	private:
		struct ComponentCollisionData
		{
			components::PhysicsComponent* component;
			sf::Vector2f startVelocity;
			sf::Vector2f endVelocity;
			sf::Vector2f startCenter;
			ecs::Box2f aabb;
		};

		struct Collision
		{
			ComponentCollisionData* a;
			ComponentCollisionData* b;
		};

		sf::Vector2f m_checkProximityBuffer;
		std::vector<Collision> m_collisions;
		std::vector<ComponentCollisionData> m_componentData;
		
		sf::Vector2f m_gravity;
		float m_restitution;

		void PopulateCollisions();
		void GetCollisionData();
		void ResolveHorizontalCollions();
		void ResolveVerticalCollisions();
		void CorrectVelocities();
		void UpdateEntities();
		bool Collide(const Collision& collision);
	};
}