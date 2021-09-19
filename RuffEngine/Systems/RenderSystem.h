#pragma once

#include "../ECSPrimitives/ISystem.h"
#include "../Components/ComponentBank.h"
#include "SFML/Graphics.hpp"

#include <memory>
#include <unordered_map>

namespace systems
{
	using VertexArrayGroup = std::unordered_map<std::string, sf::VertexArray>;
	using LayerGroup = std::unordered_map<std::string, VertexArrayGroup>;

	//TO DO: Texture preload
	class RenderSystem : public ecs::ISystem
	{
	public:
		RenderSystem(
			const std::shared_ptr<sf::RenderWindow>& windowPtr,
			const std::vector<std::string>& renderLayers);
		void Execute() override;
	private:
		std::shared_ptr<sf::RenderWindow> m_window;
		std::unordered_map<std::string, sf::Texture> m_textureMap;
		std::vector<std::string> m_renderLayers;

		void ProcessTexturePath(const std::string& texturePath);
		void AddComponentToGroup(
			const components::RenderComponent& renderComponent,
			/*out*/LayerGroup& layerGroup);
		bool ValidateRenderLayer(const std::string& layer);
		void RenderAllLayers(const LayerGroup& layerGroup);
	};
};
