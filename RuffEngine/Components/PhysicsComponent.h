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
		const sf::Vector2f& GetVelocity() const
		{
			return m_velocity;
		}
		void SetVelocity(float x, float y)
		{
			m_velocity = { x, y };
		}
		void SetVelocity(const sf::Vector2f& velocity)
		{
			m_velocity = velocity;
		}

		float GetSpeed() const
		{
			float sumSquares = m_velocity.x * m_velocity.x +
				m_velocity.y * m_velocity.y;
			return sqrt(sumSquares);
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

		float GetRestitution() const
		{
			return m_restitution;
		}
		void SetRestution(float value)
		{
			m_restitution = std::min(std::max(value, 0.0f), 1.0f);
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
		const std::unordered_set<std::string>& GetActiveCollisionLayers()
		{
			return m_activeCollisionLayers;
		}
	private:
		sf::Vector2f m_velocity;
		float m_rotationSpeed;
		float m_mass;
		float m_restitution;
		float m_drag;

		ecs::Box2f m_aabb;

		bool m_isSolid;
		bool m_isIgnoreGravity;

		std::unordered_set<std::string> m_activeCollisionLayers;
	};

}