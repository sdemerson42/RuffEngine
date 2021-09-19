#pragma once

#include "../ECSPrimitives/ComponentBase.h"
#include "../ECSPrimitives/Box2f.h"
#include "SFML/Graphics/Color.hpp"

#include <string>

namespace components
{
	class RenderComponent : public ecs::ComponentBase
	{
	public:
		const std::string& GetTexturePath() const
		{
			return m_texturePath;
		}
		const ecs::Box2f& GetTextureBox() const
		{
			return m_textureBox;
		}
		const ecs::Box2f& GetDrawBox() const
		{
			return m_drawBox;
		}
		const sf::Color& GetColor() const
		{
			return m_color;
		}
		const std::string& GetRenderLayer() const
		{
			return m_renderLayer;
		}

		void SetTexturePath(const std::string& path)
		{
			m_texturePath = path;
		}
		void SetTextureBox(float centerX, float centerY, float halfX, float halfY)
		{
			m_textureBox.center.x = centerX;
			m_textureBox.center.y = centerY;
			m_textureBox.halfSize.x = halfX;
			m_textureBox.halfSize.y = halfY;
		}
		void SetTextureBox(const ecs::Box2f& box)
		{
			m_textureBox = box;
		}
		void SetDrawBox(float centerX, float centerY, float halfX, float halfY)
		{
			m_drawBox.center.x = centerX;
			m_drawBox.center.y = centerY;
			m_drawBox.halfSize.x = halfX;
			m_drawBox.halfSize.y = halfY;
		}
		void SetDrawBox(const ecs::Box2f& box)
		{
			m_drawBox = box;
		}
		void SetColor(unsigned int r, unsigned int g, unsigned int b, unsigned int a)
		{
			m_color.r = sf::Uint8(r);
			m_color.g = sf::Uint8(g);
			m_color.b = sf::Uint8(b);
			m_color.a = sf::Uint8(a);
		}
		void SetColor(const sf::Color& color)
		{
			m_color = color;
		}
		void SetRenderLayer(const std::string& renderLayer)
		{
			m_renderLayer = renderLayer;
		}
	private:
		std::string m_texturePath;
		ecs::Box2f m_textureBox;
		ecs::Box2f m_drawBox;
		sf::Color m_color;
		std::string m_renderLayer;
	};
}
