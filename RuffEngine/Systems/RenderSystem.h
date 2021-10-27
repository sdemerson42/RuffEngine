#pragma once

#include "../ECSPrimitives/ISystem.h"
#include "../ECSPrimitives/Transform.h"
#include "../Components/ComponentBank.h"
#include "SFML/Graphics.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

namespace systems
{
	//TO DO: Texture preload
	class RenderSystem : public ecs::ISystem
	{
		using VertexGroup = std::unordered_map<std::string, sf::VertexArray>;
		using LayerGroup = std::unordered_map<std::string, VertexGroup>;

	public:
		RenderSystem(
			const std::shared_ptr<sf::RenderWindow>& windowPtr,
			const std::vector<std::string>& renderLayers);
		void Execute() override;

		static sf::View s_view;
	private:
		std::shared_ptr<sf::RenderWindow> m_window;
		std::unordered_map<std::string, sf::Texture> m_textureMap;
		std::vector<std::string> m_renderLayers;
		sf::VertexArray m_debugVertexArray;

		void ProcessTexturePath(const std::string& texturePath);
		void AddComponentToGroup(
			const components::RenderComponent& renderComponent,
			/*out*/LayerGroup& layerGroup);
		void DebugDrawEntityCenter(const components::RenderComponent& renderComponent);
		bool ValidateRenderLayer(const std::string& layer);
		void RenderAllLayers(const LayerGroup& layerGroup);
	};
};
