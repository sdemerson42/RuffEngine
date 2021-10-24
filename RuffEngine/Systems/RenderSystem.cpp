#include "pch.h"
#include "RenderSystem.h"
#include "../Util/Logger.h"
#include "../Util/Math.h"
#include "../Components/ComponentBank.h"
#include "../RuffEngine/Globals.h"

#include <algorithm>

namespace systems
{
	RenderSystem::RenderSystem(
		const std::shared_ptr<sf::RenderWindow>& windowPtr,
		const std::vector<std::string>& renderLayers) :
		m_window{ windowPtr },
		m_renderLayers{ renderLayers }
	{
	}

	void RenderSystem::Execute()
	{
		LayerGroup layerGroup;

		if (globals::DEBUG_MODE)
		{
			m_debugVertexArray.clear();
			m_debugVertexArray.setPrimitiveType(sf::PrimitiveType::Quads);
		}
		

		for (int i = 0; i < ecs::ComponentBank::m_renderComponentsSize; ++i)
		{
			const auto& renderComponent = ecs::ComponentBank::m_renderComponents[i];
			ProcessTexturePath(renderComponent.GetTexturePath());
			AddComponentToGroup(renderComponent, layerGroup);
		}

		RenderAllLayers(layerGroup);
	}

	void RenderSystem::ProcessTexturePath(const std::string& texturePath)
	{
		const auto textureMapIter = m_textureMap.find(texturePath);
		if (textureMapIter != std::end(m_textureMap))
		{
			return;
		}

		auto loadResult = m_textureMap[texturePath].loadFromFile(texturePath);	
		util::Logger::Log(loadResult ?
			"Texture at path " + texturePath + " loaded." :
			"Warning: Texture at path " + texturePath + " failed to load.");
	}

	void RenderSystem::AddComponentToGroup(
		const components::RenderComponent& renderComponent,
		/*out*/LayerGroup& layerGroup)
	{
		const auto& componentLayer = renderComponent.GetRenderLayer();

		// Don't render this component if it's on an invalid layer
		if (!ValidateRenderLayer(componentLayer)) return;

		auto& vaGroup = layerGroup[componentLayer];
		const auto& componentTexturePath = renderComponent.GetTexturePath();

		if (vaGroup.find(componentTexturePath) == vaGroup.end())
		{
			vaGroup[componentTexturePath].setPrimitiveType(sf::PrimitiveType::Quads);
		}

		// Add vertices to the array
		sf::VertexArray& vertexArray = vaGroup[componentTexturePath];

		const auto& textureBox = renderComponent.GetTextureBox();
		float texLeft = textureBox.GetLeft();
		float texRight = textureBox.GetRight();
		float texTop = textureBox.GetTop();
		float texBottom = textureBox.GetBottom();

		const auto& transform = renderComponent.GetParentTransform();
		const auto& drawBox = renderComponent.GetDrawBox();

		float drawLeft = drawBox.GetLeft() * transform.scale.x;
		float drawRight = drawBox.GetRight() * transform.scale.x;
		float drawTop = drawBox.GetTop() * transform.scale.y;
		float drawBottom = drawBox.GetBottom() * transform.scale.y;

		sf::Vector2f drawTopLeft{ drawLeft, drawTop };
		sf::Vector2f drawTopRight{ drawRight, drawTop };
		sf::Vector2f drawBottomLeft{ drawLeft, drawBottom };
		sf::Vector2f drawBottomRight{ drawRight, drawBottom };

		if (transform.rotation != 0.0f)
		{
			double radians = util::Math::DegreesToRadians(double(transform.rotation));
			drawTopLeft =
				util::Math::Rotate(sf::Vector2f{ drawLeft, drawTop }, radians);
			drawTopRight =
				util::Math::Rotate(sf::Vector2f{ drawRight, drawTop }, radians);
			drawBottomLeft =
				util::Math::Rotate(sf::Vector2f{ drawLeft, drawBottom}, radians);
			drawBottomRight =
				util::Math::Rotate(sf::Vector2f{ drawRight, drawBottom }, radians);
		}

		drawTopLeft += transform.position;
		drawTopRight += transform.position;
		drawBottomLeft += transform.position;
		drawBottomRight += transform.position;

		sf::Vertex mutableVertex;

		mutableVertex.color = renderComponent.GetColor();
		mutableVertex.position = drawTopLeft;
		mutableVertex.texCoords = sf::Vector2f{ texLeft, texTop };
		vertexArray.append(mutableVertex);

		mutableVertex.position = drawTopRight;
		mutableVertex.texCoords = sf::Vector2f{ texRight, texTop };
		vertexArray.append(mutableVertex);

		mutableVertex.position = drawBottomRight;
		mutableVertex.texCoords = sf::Vector2f{ texRight, texBottom };
		vertexArray.append(mutableVertex);

		mutableVertex.position = drawBottomLeft;
		mutableVertex.texCoords = sf::Vector2f{ texLeft, texBottom };
		vertexArray.append(mutableVertex);

		DebugDrawEntityCenter(renderComponent);
	}

	void RenderSystem::DebugDrawEntityCenter(
		const components::RenderComponent& renderComponent)
	{
		const auto& parentTransform = renderComponent.GetParentTransform();
		m_debugVertexArray.append(sf::Vertex{ sf::Vector2f{parentTransform.position.x -1.0f,
			parentTransform.position.y - 1.0f}, sf::Color::White });
		m_debugVertexArray.append(sf::Vertex{ sf::Vector2f{parentTransform.position.x + 1.0f,
			parentTransform.position.y - 1.0f}, sf::Color::White });
		m_debugVertexArray.append(sf::Vertex{ sf::Vector2f{parentTransform.position.x + 1.0f,
			parentTransform.position.y + 1.0f}, sf::Color::White });
		m_debugVertexArray.append(sf::Vertex{ sf::Vector2f{parentTransform.position.x - 1.0f,
			parentTransform.position.y + 1.0f}, sf::Color::White });
	}

	bool RenderSystem::ValidateRenderLayer(const std::string& layer)
	{
		auto resultIter = std::find(std::begin(m_renderLayers), std::end(m_renderLayers), layer);
		if (resultIter == std::end(m_renderLayers))
		{
			util::Logger::Log("Warning: Render component layer " + layer + " does not exist.");
			return false;
		}
		return true;
	}
	
	void RenderSystem::RenderAllLayers(const LayerGroup& layerGroup)
	{
		m_window->clear();

		// Draw each layer in order
		for (const auto& layer : m_renderLayers)
		{
			const auto& vaGroupIter = layerGroup.find(layer);
			if (vaGroupIter == std::end(layerGroup))
			{
				continue;
			}

			const auto& vaGroup = layerGroup.at(layer);

			for (const auto& vaPair : vaGroup)
			{
				const sf::Texture& texture = m_textureMap.at(vaPair.first);
				sf::RenderStates states{ &texture };
				m_window->draw(vaPair.second, states);
			}

			if (globals::DEBUG_MODE)
			{
				m_window->draw(m_debugVertexArray);
			}
		}
		
		m_window->display();
	}
};