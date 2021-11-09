#pragma once

#include "../ECSPrimitives/ISystem.h"
#include "../ECSPrimitives/Transform.h"
#include "SFML/Graphics.hpp"
#include "../Components/RenderComponent.h"
#include "../Components/ParticleComponent.h"

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
		std::vector<RenderLayer> m_renderLayers;
		sf::VertexArray m_debugVertexArray;

		void ProcessTexturePath(const std::string& texturePath);
		void AddComponentToGroup(
			const components::RenderComponent& renderComponent,
			/*out*/LayerGroup& layerGroup);
		void AddParticlesToGroup(
			components::ParticleComponent& particleComponent,
			/*out*/LayerGroup& layerGroup);
		void DebugDrawEntityCenter(const components::RenderComponent& renderComponent);
		bool ValidateRenderLayer(const std::string& layer);
		void RenderAllLayers(const LayerGroup& layerGroup);
	};
};
