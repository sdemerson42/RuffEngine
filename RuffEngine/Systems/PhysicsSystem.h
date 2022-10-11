#pragma once

#include "../ECSPrimitives/ISystem.h"
#include "../ECSPrimitives/Box2f.h"
#include "SFML/System/Vector2.hpp"

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

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

		std::unordered_map<int, std::vector<ComponentCollisionData>> m_staticComponentData;
		sf::Vector2f m_checkProximityBuffer;
		std::vector<Collision> m_collisions;
		std::unordered_set<Collision*> m_onCollisions;
		std::vector<ComponentCollisionData> m_componentData;
		
		float m_gravity;

		void PopulateStatics();
		void PopulateCollisions();
		void GetCollisionData();
		void ResolveCollisions(bool hPass);
		void DisplaceCollision(bool hPass, Collision& collision, float displaceRatio);
		void CorrectVelocities();
		void UpdateEntities();
		void UpdateScripts();
		bool Collide(const Collision& collision);
		int GetCellHash(int x, int y);

		const float m_displacementError = 0.2f;
		sf::Vector2i m_staticCellSize{ 100, 100 };
	};
}