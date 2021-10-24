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
			float absoluteChange;
			ecs::Box2f aabb;
			bool collisionFromTop = false;
			bool collisionFromBottom = false;
			bool collisionFromLeft = false;
			bool collisionFromRight = false;
		};

		struct Collision
		{
			ComponentCollisionData* a;
			ComponentCollisionData* b;
		};

		sf::Vector2f m_checkProximityBuffer;
		std::vector<Collision> m_collisions;
		std::vector<ComponentCollisionData> m_componentData;
		
		float m_gravity;

		void PopulateCollisions();
		void GetCollisionData();
		void ResolveCollisions(bool hPass);
		void DisplaceCollision(bool hPass, Collision& collision, float displaceRatio);
		void CorrectVelocities();
		void UpdateEntities();
		bool Collide(const Collision& collision);

		const float m_displacementError = 0.2f;
	};
}