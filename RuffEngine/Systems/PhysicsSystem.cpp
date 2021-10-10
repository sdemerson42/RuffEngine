#include "pch.h"

#include "PhysicsSystem.h"
#include "../Components/ComponentBank.h"
#include "../ECSPrimitives/Entity.h"
#include "../Util/Math.h"
#include "../Util/Logger.h"

namespace systems
{
	PhysicsSystem::PhysicsSystem()
	{
		Initialize();
	}

	void PhysicsSystem::Initialize()
	{
		// TO DO: Initialize should be called at the beginning of every scene load.
		m_checkProximityBuffer = { 32.0f, 32.0f };
	}

	void PhysicsSystem::Execute()
	{
		PopulateCollisions();
		int x = 0;
	}

	void PhysicsSystem::PopulateCollisions()
	{
		m_collisions.clear();
		auto ccds = GetCollisionData();

		for (size_t i = 0; i < ccds.size(); ++i)
		{
			const auto& aData = ccds[i];
			sf::Vector2f aTl{ 
				aData.aabb.GetLeft() - m_checkProximityBuffer.x,
				aData.aabb.GetTop() - m_checkProximityBuffer.y };
			sf::Vector2f aBr{ 
				aData.aabb.GetRight() + m_checkProximityBuffer.x, 
				aData.aabb.GetBottom() + m_checkProximityBuffer.y };

			for (size_t j = i + 1; j < ccds.size(); ++j)
			{
				const auto& bData = ccds[j];
				const auto& aabb = bData.aabb;
				if (aabb.GetLeft() > aTl.x
					&& aabb.GetRight() < aBr.x
					&& aabb.GetTop() > aTl.y
					&& aabb.GetBottom() < aBr.y)
				{
					Collision collision;
					collision.a = aData;
					collision.b = bData;
					collision.isCollision = false;
					m_collisions.push_back(collision);
				}
			}
		}

	}

	std::vector<PhysicsSystem::ComponentCollisionData> PhysicsSystem::GetCollisionData()
	{
		std::vector<PhysicsSystem::ComponentCollisionData> rData;

		const int totalComponents = ecs::ComponentBank::m_physicsComponentsSize;
		for (int i = 0; i < totalComponents; ++i)
		{
			ComponentCollisionData ccd;

			ccd.component = &ecs::ComponentBank::m_physicsComponents[i];
			ccd.startVelocity = ccd.component->GetVelocityNormal() * ccd.component->GetSpeed();

			const auto& transform = ccd.component->GetParentTransform();
			ccd.aabb = ccd.component->GetAABB();

			// Correct for rotation if necessary
			if (transform.rotation != 0.0f &&
				ccd.aabb.center.x != 0.0f || ccd.aabb.center.y != 0.0f)
			{
				ccd.aabb.center =
					util::Math::Rotate(
						ccd.aabb.center,
						util::Math::DegreesToRadians(transform.rotation));
			}
			ccd.aabb.center += transform.position;

			rData.push_back(ccd);
		}
		return rData;
	}

}