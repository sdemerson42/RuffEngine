#pragma once

#include "../ECSPrimitives/ComponentBase.h"
#include "../ECSPrimitives/Autolist.h"
#include "SFML/Graphics/Text.hpp"
#include "../ECSPrimitives/Entity.h"

#include <string>

namespace components
{
	class TextComponent : public ecs::ComponentBase, public ecs::Autolist<TextComponent>
	{
	public:
		TextComponent(ecs::Entity* parent, const std::string& sceneLayer) :
			ecs::ComponentBase{ parent }, Autolist<TextComponent>{ sceneLayer }
		{}
		sf::Text& GetText()
		{
			return m_text;
		}
		const std::string& GetFontPath() const
		{
			return m_fontPath;
		}
		void SetFontPath(const std::string& path)
		{
			m_fontPath = path;
		}
		const std::string& GetTextString() const
		{
			return m_text.getString();
		}
		void SetTextString(const std::string& text) 
		{
			m_text.setString(text);
		}
		const sf::Vector2f& GetOffset() const
		{
			return m_offset;
		}
		void SetOffset(float x, float y)
		{
			m_offset = { x, y };
		}
		void SetOutlineColor(const sf::Color& color)
		{
			m_text.setOutlineColor(color);
		}
		void SetFillColor(const sf::Color& color)
		{
			m_text.setFillColor(color);
		}
		unsigned int GetSize() const
		{
			return m_text.getCharacterSize();
		}
		void SetSize(unsigned int size)
		{
			m_text.setCharacterSize(size);
		}
		float GetOutlineThickness() const
		{
			return m_text.getOutlineThickness();
		}
		void SetOutlineThickness(float value)
		{
			m_text.setOutlineThickness(value);
		}
		const std::string& GetRenderLayer() const
		{
			return m_renderLayer;
		}
		void SetRenderLayer(const std::string& renderLayer)
		{
			m_renderLayer = renderLayer;
		}
	private:
		sf::Text m_text;
		std::string m_fontPath;
		sf::Vector2f m_offset;
		std::string m_renderLayer;
	};
}