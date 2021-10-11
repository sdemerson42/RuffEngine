#include "pch.h"

#include "PhysicsSystem.h"
#include "../Components/ComponentBank.h"
#include "../ECSPrimitives/Entity.h"
#include "../Util/Math.h"
#include "../Util/Logger.h"
#include "../Util/Time.h"

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
		m_gravity = { 0.0f, 0.0f };
		m_restitution = 0.5f;
	}

	void PhysicsSystem::Execute()
	{
		PopulateCollisions();
		ResolveHorizontalCollions();
		ResolveVerticalCollisions();
		CorrectVelocities();
		UpdateEntities();
	}

	void PhysicsSystem::PopulateCollisions()
	{
		GetCollisionData();

		m_collisions.clear();
		
		for (size_t i = 0; i < m_componentData.size(); ++i)
		{
			auto& aData = m_componentData[i];
			sf::Vector2f aTl{
				aData.aabb.GetLeft() - m_checkProximityBuffer.x,
				aData.aabb.GetTop() - m_checkProximityBuffer.y };
			sf::Vector2f aBr{
				aData.aabb.GetRight() + m_checkProximityBuffer.x,
				aData.aabb.GetBottom() + m_checkProximityBuffer.y };

			for (size_t j = i + 1; j < m_componentData.size(); ++j)
			{
				auto& bData = m_componentData[j];
				const auto& aabb = bData.aabb;
				if (aabb.GetLeft() > aTl.x
					&& aabb.GetRight() < aBr.x
					&& aabb.GetTop() > aTl.y
					&& aabb.GetBottom() < aBr.y)
				{
					Collision collision;
					collision.a = &aData;
					collision.b = &bData;
					m_collisions.push_back(collision);
				}
			}
		}

	}

	void PhysicsSystem::GetCollisionData()
	{
		m_componentData.clear();

		const int totalComponents = ecs::ComponentBank::m_physicsComponentsSize;
		for (int i = 0; i < totalComponents; ++i)
		{
			ComponentCollisionData ccd;

			ccd.component = &ecs::ComponentBank::m_physicsComponents[i];
			ccd.startVelocity = ccd.component->GetVelocity();
			ccd.endVelocity = ccd.startVelocity;
			
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
			ccd.startCenter = ccd.aabb.center;

			m_componentData.push_back(ccd);
		}
	}

	void PhysicsSystem::ResolveHorizontalCollions()
	{
		for (auto& cData : m_componentData)
		{
			cData.aabb.center.x += cData.startVelocity.x * util::Time::DeltaTime();
		}

		for (auto& collision : m_collisions)
		{
			if (!Collide(collision))
			{
				continue;
			}

			if (!collision.a->component->GetIsSolid() ||
				!collision.b->component->GetIsSolid())
			{
				continue;
			}

			const float aIMass = collision.a->component->GetInverseMass();
			const float bIMass = collision.b->component->GetInverseMass();

			if (aIMass == 0.0f && bIMass == 0.0f)
			{
				// TO DO: ???
				continue;
			}

			float relativeVelocity =
				collision.b->startVelocity.x - collision.a->startVelocity.x;
			float impulse = (-1.0f * (m_restitution + 1.0f) * relativeVelocity) /
				(aIMass + bIMass);

			const float aMass = collision.a->component->GetMass();
			const float bMass = collision.b->component->GetMass();
			float displaceRatio = 0.0f;

			if (aMass == 0.0f)
			{
				collision.b->endVelocity.x += impulse / bMass;
			}
			else if (bMass == 0.0f)
			{
				collision.a->endVelocity.x -= impulse / aMass;
				displaceRatio = 1.0f;
			}
			else
			{
				collision.a->endVelocity.x -= impulse / aMass;
				collision.b->endVelocity.x += impulse / bMass;
				displaceRatio = aMass / (aMass + bMass);
			}

			// Displacement

			if (collision.a->aabb.center.x < collision.b->aabb.center.x)
			{
				const float overlap = collision.a->aabb.GetRight()
					- collision.b->aabb.GetLeft();
				collision.a->aabb.center.x -= overlap * displaceRatio;
				collision.b->aabb.center.x += overlap * (1.0f - displaceRatio);
			}
			else
			{
				const float overlap = collision.b->aabb.GetRight() -
					collision.a->aabb.GetLeft();
				collision.a->aabb.center.x += overlap * displaceRatio;
				collision.b->aabb.center.x -= overlap * (1.0f - displaceRatio);
			}
		}
	}

	void PhysicsSystem::ResolveVerticalCollisions()
	{
		for (auto& cData : m_componentData)
		{
			cData.aabb.center.y += cData.startVelocity.y * util::Time::DeltaTime();
		}

		for (auto& collision : m_collisions)
		{
			if (!Collide(collision))
			{
				continue;
			}

			if (!collision.a->component->GetIsSolid() ||
				!collision.b->component->GetIsSolid())
			{
				continue;
			}

			const float aIMass = collision.a->component->GetInverseMass();
			const float bIMass = collision.b->component->GetInverseMass();

			if (aIMass == 0.0f && bIMass == 0.0f)
			{
				// TO DO: ???
				continue;
			}

			float relativeVelocity =
				collision.b->startVelocity.y - collision.a->startVelocity.y;
			float impulse = (-1.0f * (m_restitution + 1.0f) * relativeVelocity) /
				(aIMass + bIMass);

			const float aMass = collision.a->component->GetMass();
			const float bMass = collision.b->component->GetMass();
			float displaceRatio = 0.0f;

			if (aMass == 0.0f)
			{
				collision.b->endVelocity.y += impulse / bMass;
			}
			else if (bMass == 0.0f)
			{
				collision.a->endVelocity.y -= impulse / aMass;
				displaceRatio = 1.0f;
			}
			else
			{
				collision.a->endVelocity.y -= impulse / aMass;
				collision.b->endVelocity.y += impulse / bMass;
				displaceRatio = aMass / (aMass + bMass);
			}

			// Displacement

			if (collision.a->aabb.center.y < collision.b->aabb.center.y)
			{
				const float overlap = collision.a->aabb.GetBottom()
					- collision.b->aabb.GetTop();
				collision.a->aabb.center.y -= overlap * displaceRatio;
				collision.b->aabb.center.y += overlap * (1.0f - displaceRatio);
			}
			else
			{
				const float overlap = collision.b->aabb.GetBottom() -
					collision.a->aabb.GetTop();
				collision.a->aabb.center.y += overlap * displaceRatio;
				collision.b->aabb.center.y -= overlap * (1.0f - displaceRatio);
			}
		}
	}

	void PhysicsSystem::CorrectVelocities()
	{
		for (auto& cData : m_componentData)
		{
			// TO DO: Add gravity logic

			sf::Vector2f vel = cData.endVelocity;
			float speed = sqrt(vel.x * vel.x + vel.y * vel.y);
			if (speed > 0.0f)
			{
				vel /= speed;
			}
			
			speed = std::max(0.0f, speed -
				cData.component->GetDrag() * util::Time::DeltaTime());
			cData.component->SetVelocity(vel * speed);
		}
	}

	void PhysicsSystem::UpdateEntities()
	{
		for (auto& cData : m_componentData)
		{
			sf::Vector2f centerDelta = cData.aabb.center - cData.startCenter;
			cData.component->GetParent()->SetPosition(
				cData.component->GetParent()->GetPosition() + centerDelta);
		}
	}

	bool PhysicsSystem::Collide(const Collision& collision)
	{
		return collision.a->aabb.GetRight() > collision.b->aabb.GetLeft()
			&& collision.a->aabb.GetLeft() < collision.b->aabb.GetRight()
			&& collision.a->aabb.GetBottom() > collision.b->aabb.GetTop()
			&& collision.a->aabb.GetTop() < collision.b->aabb.GetBottom();
	}
}