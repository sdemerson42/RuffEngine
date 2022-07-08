#pragma once

#include "../ECSPrimitives/ComponentBase.h"
#include "SFML/System/Vector2.hpp"
#include "SFML/System/Vector3.hpp"
#include "../ECSPrimitives/Autolist.h"
#include "../ECSPrimitives/Entity.h"

namespace components
{
	class LightComponent : public ecs::ComponentBase, public ecs::Autolist<LightComponent>
	{
	public:
		LightComponent(ecs::Entity* parent, const std::string& sceneLayer) :
			ComponentBase{ parent }, Autolist<LightComponent> { sceneLayer }
		{}

		inline void SetOffset(float x, float y)
		{
			m_offset = sf::Vector2f{ x, y };
		}
		inline void SetOffset(const sf::Vector2f& position)
		{
			m_offset = position;
		}
		inline const sf::Vector2f& GetOffset() const
		{
			return m_offset;
		}
		inline void SetColor(float r, float g, float b)
		{
			m_color = sf::Vector3f{ r, g, b };
		}
		inline void SetColor(const sf::Vector3f& color)
		{
			m_color = color;
		}
		inline const sf::Vector3f& GetColor() const
		{
			return m_color;
		}
		inline void SetRadius(float value)
		{
			m_radius = value;
		}
		inline float GetRadius() const
		{
			return m_radius;
		}
	private:
		sf::Vector2f m_offset;
		sf::Vector3f m_color;
		float m_radius;
	};

}