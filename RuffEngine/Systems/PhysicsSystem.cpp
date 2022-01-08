#include "pch.h"

#include "PhysicsSystem.h"
#include "../Components/PhysicsComponent.h"
#include "../Components/ScriptComponent.h"
#include "../ECSPrimitives/Entity.h"
#include "../Util/Math.h"
#include "../Util/Logger.h"
#include "../Util/Time.h"
#include "SpawnSystem.h"

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
		m_gravity = { 100.0f };
	}

	void PhysicsSystem::Execute()
	{
		PopulateCollisions();
		ResolveCollisions(false);
		ResolveCollisions(true);
		CorrectVelocities();
		UpdateEntities();
		UpdateScripts();
	}

	void PhysicsSystem::PopulateCollisions()
	{
		GetCollisionData();

		m_collisions.clear();
		m_onCollisions.clear();
		
		for (size_t i = 0; i < m_componentData.size(); ++i)
		{
			auto& aData = m_componentData[i];
			const auto& aLayers = aData.component->GetActiveCollisionLayers();
			for (size_t j = i + 1; j < m_componentData.size(); ++j)
			{
				auto& bData = m_componentData[j];

				// Check collision layers
				bool sharedLayer = false;
				for (const std::string& layer : aLayers)
				{
					if (bData.component->HasActiveCollisionLayer(layer))
					{
						sharedLayer = true;
						break;
					}
				}
				if (!sharedLayer) continue;

				sf::Vector2f aTl{
				aData.aabb.GetLeft() - m_checkProximityBuffer.x,
				aData.aabb.GetTop() - m_checkProximityBuffer.y };
				sf::Vector2f aBr{
					aData.aabb.GetRight() + m_checkProximityBuffer.x,
					aData.aabb.GetBottom() + m_checkProximityBuffer.y };

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

		const auto& sceneLayer = SpawnSystem::GetSceneLayer();
		auto sz = ecs::Autolist<components::PhysicsComponent>::Size(sceneLayer);
		for (int i = 0; i < sz; ++i)
		{
			auto pc = ecs::Autolist<components::PhysicsComponent>::Get(sceneLayer, i);
			if (!pc->GetIsActive() || !pc->GetParent()->GetIsActive())
			{
				continue;
			}

			ComponentCollisionData ccd;

			ccd.component = &*pc;
			const auto& transform = ccd.component->GetParentTransform();
			ccd.aabb = ccd.component->GetAABB();

			// Correct for present rotation if necessary
			if (transform.rotation != 0.0f &&
				ccd.aabb.center.x != 0.0f || ccd.aabb.center.y != 0.0f)
			{
				ccd.aabb.center = util::Math::Rotate(
					ccd.aabb.center,
					util::Math::DegreesToRadians(transform.rotation));
			}

			ccd.startVelocity = ccd.component->GetVelocity();
			ccd.aabb.center += transform.position;
			ccd.startCenter = ccd.aabb.center;
			ccd.endVelocity = ccd.startVelocity;

			m_componentData.push_back(ccd);
		}
	}

	void PhysicsSystem::ResolveCollisions(bool hPass)
	{
		for (auto& cData : m_componentData)
		{
			if (hPass)
			{
				float change = cData.startVelocity.x * util::Time::DeltaTime();
				cData.aabb.center.x += change;
				cData.absoluteChange = abs(change);
			}
			else
			{
				float change = cData.startVelocity.y * util::Time::DeltaTime();
				cData.aabb.center.y += change;
				cData.absoluteChange = abs(change);
			}
		}

		for (auto& collision : m_collisions)
		{
			if (!Collide(collision))
			{
				continue;
			}

			// This collision may trigger scripts
			m_onCollisions.insert(&collision);

			if (!collision.a->component->GetIsSolid() ||
				!collision.b->component->GetIsSolid())
			{
				continue;
			}

			const float aIMass = collision.a->component->GetInverseMass();
			const float bIMass = collision.b->component->GetInverseMass();

			// Orientation references

			float& aCenterCoord = hPass ?
				collision.a->aabb.center.x : collision.a->aabb.center.y;
			float& bCenterCoord = hPass ?
				collision.b->aabb.center.x : collision.b->aabb.center.y;
			const bool aFirst = aCenterCoord < bCenterCoord;
			bool& aNegativeCollision = hPass ?
				collision.a->collisionFromLeft : collision.a->collisionFromTop;
			bool& bNegativeCollision = hPass ?
				collision.b->collisionFromLeft : collision.b->collisionFromTop;
			bool& aPositiveCollision = hPass ?
				collision.a->collisionFromRight : collision.a->collisionFromBottom;
			bool& bPositiveCollision = hPass ?
				collision.b->collisionFromRight : collision.b->collisionFromBottom;
			float& aStartVelocityCoord = hPass ?
				collision.a->startVelocity.x : collision.a->startVelocity.y;
			float& bStartVelocityCoord = hPass ?
				collision.b->startVelocity.x : collision.b->startVelocity.y;
			float& aEndVelocityCoord = hPass ?
				collision.a->endVelocity.x : collision.a->endVelocity.y;
			float& bEndVelocityCoord = hPass ?
				collision.b->endVelocity.x : collision.b->endVelocity.y;
			
			const bool aCanChangeVelocity = aIMass != 0.0f &&
				((aFirst && !aPositiveCollision) ||
					(!aFirst && !aNegativeCollision));
			const bool bCanChangeVelocity = bIMass != 0.0f &&
				((aFirst && !bNegativeCollision) ||
					(!aFirst && !bPositiveCollision));

			if (!aCanChangeVelocity && !bCanChangeVelocity)
			{
				continue;
			}

			float relativeVelocity =
				bStartVelocityCoord - aStartVelocityCoord;

			if (aFirst)
			{
				if (relativeVelocity >= 0.0f)
				{
					continue;
				}
			}
			else
			{
				if (relativeVelocity <= 0.0f)
				{
					continue;
				}
			}

			float restitution =
				std::min(
					collision.a->component->GetRestitution(), 
					collision.b->component->GetRestitution());
			float impulse = (-1.0f * (restitution + 1.0f) * relativeVelocity) /
				(aIMass + bIMass);

			const float aMass = collision.a->component->GetMass();
			const float bMass = collision.b->component->GetMass();
			float displaceRatio = 0.0f;

			if (aMass == 0.0f && bCanChangeVelocity)
			{
				bEndVelocityCoord += impulse / bMass;
			}
			else if (bMass == 0.0f && aCanChangeVelocity)
			{
				aEndVelocityCoord -= impulse / aMass;
				displaceRatio = 1.0f;
			}
			else
			{
				aEndVelocityCoord -= impulse / aMass;
				bEndVelocityCoord += impulse / bMass;
				displaceRatio = collision.a->absoluteChange /
					(collision.a->absoluteChange + collision.b->absoluteChange);
			}

			if (aFirst)
			{
				aPositiveCollision = true;
				bNegativeCollision = true;
			}
			else
			{
				aNegativeCollision = true;
				bPositiveCollision = true;
			}

			DisplaceCollision(hPass, collision, displaceRatio);
		}
	}

	void PhysicsSystem::DisplaceCollision(bool hPass, Collision& collision, float displaceRatio)
	{
		float& aCenterCoord = hPass ?
			collision.a->aabb.center.x : collision.a->aabb.center.y;
		float& bCenterCoord = hPass ?
			collision.b->aabb.center.x : collision.b->aabb.center.y;
		const bool aFirst = aCenterCoord < bCenterCoord;
		float aNegativeBoxCoord = hPass ?
			collision.a->aabb.GetLeft() : collision.a->aabb.GetTop();
		float bNegativeBoxCoord = hPass ?
			collision.b->aabb.GetLeft() : collision.b->aabb.GetTop();
		float aPositiveBoxCoord = hPass ?
			collision.a->aabb.GetRight() : collision.a->aabb.GetBottom();
		float bPositiveBoxCoord = hPass ?
			collision.b->aabb.GetRight() : collision.b->aabb.GetBottom();

		float overlap = 0.0f;
		if (aFirst)
		{
			overlap = aPositiveBoxCoord - bNegativeBoxCoord;
		}
		else
		{
			overlap = bPositiveBoxCoord - aNegativeBoxCoord;
		}

		const auto aDisplace = std::min(overlap * displaceRatio, collision.a->absoluteChange);
		const auto bDisplace = std::min(overlap * (1.0f - displaceRatio), collision.b->absoluteChange);;
		const auto aError = aDisplace == 0.0f ? 0.0f : m_displacementError;
		const auto bError = bDisplace == 0.0f ? 0.0f : m_displacementError;

		if (aFirst)
		{
			aCenterCoord -= aDisplace + aError;
			bCenterCoord += bDisplace + bError;
		}
		else
		{
			aCenterCoord += aDisplace + aError;
			bCenterCoord -= bDisplace + bError;
		}
	}

	void PhysicsSystem::CorrectVelocities()
	{
		for (auto& cData : m_componentData)
		{
			if (cData.component->GetMass() != 0.0f &&
				!cData.component->GetIsIgnoreGravity())
			{
				cData.endVelocity.y += m_gravity * util::Time::DeltaTime();
			}

			float drag = cData.component->GetDrag();
			float adjustedDrag = drag * util::Time::DeltaTime();
			if (drag != 0.0f)
			{
				if (!cData.component->GetIsIgnoreGravity())
				{
					float& x = cData.endVelocity.x;
					if (x > 0.0f)
					{
						x = std::max(0.0f, x - adjustedDrag);
					}
					else
					{
						x = std::min(0.0f, x + adjustedDrag);
					}
				}
				else
				{
					float speed = sqrt(cData.endVelocity.x * cData.endVelocity.x +
						cData.endVelocity.y * cData.endVelocity.y);
					if (speed != 0.0f)
					{
						cData.endVelocity /= speed;
						speed = std::max(0.0f, speed - adjustedDrag);
						cData.endVelocity *= speed;
					}
				}
				
			}

			cData.component->SetVelocity(cData.endVelocity);
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

	void PhysicsSystem::UpdateScripts()
	{
		for (Collision* collision : m_onCollisions)
		{
			for (int i = 0; i < 2; ++i)
			{
				ComponentCollisionData* collisionData = (i == 0 ?
					collision->a : collision->b);
				ComponentCollisionData* otherCollisionData = (i == 0 ?
					collision->b : collision->a);
				components::ScriptComponent* script =
					collisionData->component->GetParent()->GetComponent<components::ScriptComponent>();
				if (script != nullptr)
				{
					sf::Vector2f normal{ 0.0f, 0.0f };
					if (collisionData->collisionFromLeft) normal.x = -1.0f;
					if (collisionData->collisionFromRight) normal.x = 1.0f;
					if (collisionData->collisionFromTop) normal.y = -1.0f;
					if (collisionData->collisionFromBottom) normal.y = 1.0f;
					script->ExecuteCollision(
						otherCollisionData->component->GetParent(),
						normal.x,
						normal.y);
				}
			}
		}
	}

	bool PhysicsSystem::Collide(const Collision& collision)
	{
		return collision.a->aabb.GetRight() > collision.b->aabb.GetLeft() + m_displacementError
			&& collision.a->aabb.GetLeft() < collision.b->aabb.GetRight() - m_displacementError
			&& collision.a->aabb.GetBottom() > collision.b->aabb.GetTop() + m_displacementError
			&& collision.a->aabb.GetTop() < collision.b->aabb.GetBottom() - m_displacementError;
	}
}