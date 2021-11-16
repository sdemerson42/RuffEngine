#pragma once

#include "../ECSPrimitives/ISystem.h"
#include "../ECSPrimitives/Transform.h"
#include "SFML/Graphics.hpp"
#include "../Components/RenderComponent.h"
#include "../Components/ParticleComponent.h"
#include "../Components/TextComponent.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace systems
{
	//TO DO: Texture preload
	class RenderSystem : public ecs::ISystem
	{
		struct DrawableGroup
		{
			std::unordered_map<std::string, sf::VertexArray> vertexArrays;
			std::vector<sf::Text*> texts;
			std::vector<sf::Sprite> renderTextureSprites;
		};
		using LayerGroup = std::unordered_map<std::string, DrawableGroup>;

	public:
		struct RenderLayer
		{
			std::string name;
			bool isStatic;
		};

		RenderSystem(
			const std::shared_ptr<sf::RenderWindow>& windowPtr,
			const std::vector<RenderLayer>& renderLayers);
		void Execute() override;

		static sf::View s_view;
		static sf::View s_defaultView;
	private:
		std::shared_ptr<sf::RenderWindow> m_window;
		std::unordered_map<std::string, sf::Texture> m_textureMap;
		std::unordered_map<std::string, sf::Font> m_fontMap;
		std::vector<RenderLayer> m_renderLayers;
		sf::VertexArray m_debugVertexArray;

		void ProcessTexturePath(const std::string& texturePath);
		void ProcessFontPath(const std::string& fontPath);
		void AddRenderTextureToGroup(
			const std::string& renderLayer,
			const sf::RenderTexture& renderTexture,
			/*out*/LayerGroup& layerGroup);
		void AddComponentToGroup(
			const components::RenderComponent& renderComponent,
			/*out*/LayerGroup& layerGroup);
		void AddParticlesToGroup(
			components::ParticleComponent& particleComponent,
			/*out*/LayerGroup& layerGroup);
		void AddTextToGroup(components::TextComponent& textComponent,
			/*out*/LayerGroup& layerGroup);
		void DebugDrawEntityCenter(const components::RenderComponent& renderComponent);
		bool ValidateRenderLayer(const std::string& layer);
		void RenderAllLayers(const LayerGroup& layerGroup);
	};
};
