#pragma once

#include "../ECSPrimitives/ComponentBase.h"
#include "../ECSPrimitives/Box2f.h"
#include "../Util/Math.h"
#include "SFML/System/Vector2.hpp"

#include <unordered_set>
#include <string>

namespace components
{
	class PhysicsComponent : public ecs::ComponentBase
	{
	public:
		const sf::Vector2f& GetVelocityNormal() const
		{
			return m_velocityNormal;
		}
		void SetVelocity(float x, float y)
		{
			m_velocityNormal = { x, y };
			util::Math::NormalizeVector(m_velocityNormal);
		}
		void SetVelocity(const sf::Vector2f& velocity)
		{
			SetVelocity(velocity.x, velocity.y);
		}

		float GetSpeed() const
		{
			return m_velocityMagnitude;
		}
		void SetSpeed(float speed)
		{
			m_velocityMagnitude = speed;
		}

		float GetMass() const
		{
			return m_mass;
		}
		float GetInverseMass() const
		{
			if (m_mass == 0.0f) return m_mass;
			return 1.0f / m_mass;
		}
		void SetMass(float mass)
		{
			m_mass = mass;
		}

		float GetDrag() const
		{
			return m_drag;
		}
		void SetDrag(float drag)
		{
			m_drag = drag;
		}

		const ecs::Box2f& GetAABB() const
		{
			return m_aabb;
		}
		void SetAABB(float centerX, float centerY, float halfX, float halfY)
		{
			m_aabb.center = { centerX, centerY };
			m_aabb.halfSize = { halfX, halfY };
		}
		void SetAABB(const ecs::Box2f& newBox)
		{
			m_aabb = newBox;
		}

		bool GetIsSolid() const
		{
			return m_isSolid;
		}
		void SetIsSolid(bool isSolid)
		{
			m_isSolid = isSolid;
		}

		bool GetIsIgnoreGravity() const
		{
			return m_isIgnoreGravity;
		}
		void SetIsIgnoreGravity(bool value)
		{
			m_isIgnoreGravity = value;
		}

		void AddActiveCollisionLayer(const std::string& layer)
		{
			m_activeCollisionLayers.insert(layer);
		}
		void RemoveActiveCollisionLayer(const std::string& layer)
		{
			m_activeCollisionLayers.erase(layer);
		}
		bool HasActiveCollisionLayer(const std::string& layer) const
		{
			return m_activeCollisionLayers.find(layer) != m_activeCollisionLayers.end();
		}
	private:
		sf::Vector2f m_velocityNormal;
		float m_velocityMagnitude;
		float m_mass;
		float m_drag;

		ecs::Box2f m_aabb;

		bool m_isSolid;
		bool m_isIgnoreGravity;

		std::unordered_set<std::string> m_activeCollisionLayers;
	};

}