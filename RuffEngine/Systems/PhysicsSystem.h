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
			ecs::Box2f aabb;
		};

		struct Collision
		{
			ComponentCollisionData a;
			ComponentCollisionData b;
			bool isCollision;
		};

		sf::Vector2f m_checkProximityBuffer;
		std::vector<Collision> m_collisions;
		sf::Vector2f m_gravity;

		void PopulateCollisions();
		std::vector<ComponentCollisionData> GetCollisionData();
	};
}