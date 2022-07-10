#include "pch.h"
#include "RenderSystem.h"
#include "../Util/Logger.h"
#include "../Util/Math.h"
#include "../ECSPrimitives/Entity.h"
#include "../RuffEngine/Globals.h"
#include "../Components/TileComponent.h"
#include "../Components/RenderComponent.h"
#include "../Components/ParticleComponent.h"
#include "../Components/TextComponent.h"
#include "../Components//LightComponent.h"

#include <algorithm>

namespace systems
{
	sf::View RenderSystem::s_view;
	sf::View RenderSystem::s_defaultView;

	RenderSystem::RenderSystem(
		const std::shared_ptr<sf::RenderWindow>& windowPtr,
		const std::vector<RenderLayer>& renderLayers) :
		m_window{ windowPtr },
		m_renderLayers{ renderLayers }
	{
		sf::Vector2f windowSize{ m_window->getSize() };
		s_view.setSize(windowSize);
		s_view.setCenter(windowSize.x / 2.0f, windowSize.y / 2.0f);
		s_defaultView.setSize(windowSize);
		s_defaultView.setCenter(windowSize.x / 2.0f, windowSize.y / 2.0f);
		
		if (!m_lightingShader.loadFromFile("Sim/Shaders/Lighting.vert", sf::Shader::Vertex))
		{
			util::Logger::Log("Warning: Lighting vertex shader not loaded.");
		}
		if (!m_lightingShader.loadFromFile("Sim/Shaders/Lighting.frag", sf::Shader::Fragment))
		{
			util::Logger::Log("Warning: Lighting fragment shader not loaded.");
		}
		// TODO: Configurable darkness values
		m_darknessColor = sf::Vector3f{ 0.01f, 0.01f, 0.01f };
	}

	void RenderSystem::Execute()
	{
		LayerGroup layerGroup;

		if (globals::DEBUG_MODE)
		{
			m_debugVertexArray.clear();
			m_debugVertexArray.setPrimitiveType(sf::PrimitiveType::Quads);
		}
		
		auto sz = ecs::Autolist<components::TileComponent>::SizeAll();
		for (int i = 0; i < sz; ++i)
		{
			auto tileComponent = ecs::Autolist<components::TileComponent>::GetAll(i);
			if (!tileComponent->GetIsActive() || !tileComponent->GetParent()->GetIsActive())
			{
				continue;
			}
			AddRenderTextureToGroup(*tileComponent, layerGroup);
		}

		sz = ecs::Autolist<components::RenderComponent>::SizeAll();
		for (int i = 0; i < sz; ++i)
		{
			auto renderComponent = ecs::Autolist<components::RenderComponent>::GetAll(i);
			if (!renderComponent->GetIsActive() || !renderComponent->GetParent()->GetIsActive())
			{
				continue;
			}
			ProcessTexturePath(renderComponent->GetTexturePath());
			AddComponentToGroup(*renderComponent, layerGroup);
		}

		sz = ecs::Autolist<components::ParticleComponent>::SizeAll();
		for (int i = 0; i < sz; ++i)
		{
			auto particleComponent = ecs::Autolist<components::ParticleComponent>::GetAll(i);
			auto particleCount = particleComponent->GetParticles().size();
			if (particleCount == 0)
			{
				continue;
			}
			ProcessTexturePath(particleComponent->GetTexturePath());
			AddParticlesToGroup(*particleComponent, layerGroup);
		}

		sz = ecs::Autolist<components::TextComponent>::SizeAll();
		for (int i = 0; i < sz; ++i)
		{
			auto textComponent = ecs::Autolist<components::TextComponent>::GetAll(i);
			ProcessFontPath(textComponent->GetFontPath());
			AddTextToGroup(*textComponent, layerGroup);
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

	void RenderSystem::ProcessFontPath(const std::string& fontPath)
	{
		const auto fontMapIter = m_fontMap.find(fontPath);
		if (fontMapIter != std::end(m_fontMap))
		{
			return;
		}

		auto loadResult = m_fontMap[fontPath].loadFromFile(fontPath);
		util::Logger::Log(loadResult ?
			"Font at path " + fontPath + " loaded." :
			"Warning: Font at path " + fontPath + " failed to load.");
	}

	void RenderSystem::AddRenderTextureToGroup(components::TileComponent& tileComponent,
		/*out*/LayerGroup& layerGroup)
	{
		const sf::Vector2f& parentPosition = tileComponent.GetParent()->GetPosition();
		const auto& textures = tileComponent.GetRenderTextures();
		for (const auto& texture : textures)
		{
			sf::Sprite sprite;
			sprite.setTexture(texture.renderTexture->getTexture());
			sprite.setPosition(parentPosition);
			layerGroup[texture.renderLayer].renderTextureSprites.push_back(sprite);
		}
	}

	void RenderSystem::AddComponentToGroup(
		const components::RenderComponent& renderComponent,
		/*out*/LayerGroup& layerGroup)
	{
		const auto& componentLayer = renderComponent.GetRenderLayer();

		// Don't render this component if it's on an invalid layer
		if (!ValidateRenderLayer(componentLayer)) return;

		auto& drawGroup = layerGroup[componentLayer];
		const auto& componentTexturePath = renderComponent.GetTexturePath();

		if (drawGroup.vertexArrays.find(componentTexturePath) == drawGroup.vertexArrays.end())
		{
			drawGroup.vertexArrays[componentTexturePath].setPrimitiveType(sf::PrimitiveType::Quads);
		}

		// Add vertices to the array
		sf::VertexArray& vertexArray = drawGroup.vertexArrays[componentTexturePath];

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

		if (globals::DEBUG_MODE)
		{
			DebugDrawEntityCenter(renderComponent);
		}
	}

	void RenderSystem::AddParticlesToGroup(
		components::ParticleComponent& particleComponent,
		/*out*/LayerGroup& layerGroup)
	{
		const auto& componentLayer = particleComponent.GetRenderLayer();

		// Don't render this component if it's on an invalid layer
		if (!ValidateRenderLayer(componentLayer)) return;

		auto& drawGroup = layerGroup[componentLayer];
		const auto& componentTexturePath = particleComponent.GetTexturePath();

		if (drawGroup.vertexArrays.find(componentTexturePath) == drawGroup.vertexArrays.end())
		{
			drawGroup.vertexArrays[componentTexturePath].setPrimitiveType(sf::PrimitiveType::Quads);
		}

		// Add vertices to the array
		sf::VertexArray& vertexArray = drawGroup.vertexArrays[componentTexturePath];
		auto& particles = particleComponent.GetParticles();

		const auto& textureBox = particleComponent.GetTextureBox();
		float texLeft = textureBox.GetLeft();
		float texRight = textureBox.GetRight();
		float texTop = textureBox.GetTop();
		float texBottom = textureBox.GetBottom();

		for (const auto& particle : particles)
		{
			if (!particle.active) continue;

			const auto& drawBox = particle.drawBox;

			float drawLeft = drawBox.GetLeft();
			float drawRight = drawBox.GetRight();
			float drawTop = drawBox.GetTop();
			float drawBottom = drawBox.GetBottom();

			sf::Vector2f drawTopLeft{ drawLeft, drawTop };
			sf::Vector2f drawTopRight{ drawRight, drawTop };
			sf::Vector2f drawBottomLeft{ drawLeft, drawBottom };
			sf::Vector2f drawBottomRight{ drawRight, drawBottom };

			sf::Vertex mutableVertex;

			mutableVertex.color = particle.color;
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
		}
	}

	void RenderSystem::AddTextToGroup(components::TextComponent& textComponent,
		/*out*/LayerGroup& layerGroup)
	{
		const auto& componentLayer = textComponent.GetRenderLayer();

		// Don't render this component if it's on an invalid layer
		if (!ValidateRenderLayer(componentLayer)) return;

		auto& drawGroup = layerGroup[componentLayer];
		sf::Text* text = &textComponent.GetText();
		if (text->getFont() == nullptr)
		{
			text->setFont(m_fontMap[textComponent.GetFontPath()]);
		}

		ecs::Entity* parent = textComponent.GetParent();
		auto offset = textComponent.GetOffset();
		float rotation = parent->GetRotation();
		if (rotation != 0.0f)
		{
			offset = util::Math::Rotate(offset, util::Math::DegreesToRadians(rotation));
		}
		auto position = parent->GetPosition() + offset;

		text->setPosition(position);
		text->setRotation(rotation);

		drawGroup.texts.push_back(&textComponent.GetText());
	}

	void RenderSystem::AddPointLights(const sf::Vector2u& texSize)
	{
		m_lightPositions.clear();
		m_lightColors.clear();
		m_lightRadii.clear();

		const auto sz = ecs::Autolist<components::LightComponent>::SizeAll();
		for (size_t i = 0; i < sz; ++i)
		{
			auto lightComponent = ecs::Autolist<components::LightComponent>::GetAll(i);

			if (!lightComponent->GetParent()->GetIsActive() || !lightComponent->GetIsActive())
			{
				continue;
			}

			const auto& position = lightComponent->GetOffset() + lightComponent->GetParentTransform().position;
			const auto& radius = lightComponent->GetRadius();
			const auto& viewCenter = s_view.getCenter();
			const auto& viewSize = s_view.getSize();

			float x1 = viewCenter.x - viewSize.x / 2;
			float y1 = viewCenter.y - viewSize.y / 2;
			float x2 = viewCenter.x + viewSize.x / 2;
			float y2 = viewCenter.y + viewSize.y / 2;

			float xn = std::max(x1, std::min(viewCenter.x, x2));
			float yn = std::max(y1, std::min(viewCenter.y, y2));
			float dx = xn - viewCenter.x;
			float dy = yn - viewCenter.y;
			if (dx * dx + dy * dy > radius * radius)
			{
				continue;
			}

			m_lightPositions.push_back(sf::Glsl::Vec2{ position });
			
			m_lightColors.push_back(sf::Glsl::Vec4{});
			auto& currentColor = m_lightColors.back();
			const auto& color = lightComponent->GetColor();
			currentColor.w = 1.0f;
			currentColor.x = color.x;
			currentColor.y = color.y;
			currentColor.z = color.z;
			m_lightRadii.push_back(radius);
		}
		
		if (m_lightPositions.empty())
		{
			m_lightingShader.setUniform("lightTotal", 0);
			return;
		}
		
		int lightTotal = int(m_lightPositions.size());

		m_lightingShader.setUniformArray("pointPositions", &m_lightPositions[0], lightTotal);
		m_lightingShader.setUniformArray("pointColors", &m_lightColors[0], lightTotal);
		m_lightingShader.setUniformArray("pointRadii", &m_lightRadii[0], lightTotal);
		m_lightingShader.setUniform("lightTotal", lightTotal);
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
		auto resultIter = std::find_if(std::begin(m_renderLayers), std::end(m_renderLayers),
			[&](const RenderLayer& renderLayer)
			{
				return renderLayer.name == layer;
			});
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
		for (size_t i = 0; i < m_renderLayers.size(); ++i)
		{
			const auto& layer = m_renderLayers[i];

			m_window->setView(layer.isStatic ? s_defaultView : s_view);

			const auto& vaGroupIter = layerGroup.find(layer.name);
			if (vaGroupIter == std::end(layerGroup))
			{
				continue;
			}

			const auto& drawGroup = layerGroup.at(layer.name);

			if (layer.groupIndex != -1)
			{
				if (i == 0 || m_renderLayers[i - 1].groupIndex != layer.groupIndex)
				{
					unsigned int size = 0;
					if (!drawGroup.renderTextureSprites.empty())
					{
						const auto* renderTexture = drawGroup.renderTextureSprites[0].getTexture();
						size = std::max(renderTexture->getSize().x, renderTexture->getSize().y);
					}
					else
					{
						size = std::max(m_window->getSize().x, m_window->getSize().y);
					}
					m_lightingShader.setUniform("texSize", float(size));
					m_lightingShader.setUniform("dark", sf::Glsl::Vec4{ m_darknessColor.x, m_darknessColor.y, m_darknessColor.z, 1.0f });
					m_compositeRenderTexture.create(size, size);
					m_compositeRenderTexture.clear(sf::Color::Black);
				}
			}

			sf::RenderTarget* target{ nullptr };
			if (layer.groupIndex != -1)
			{
				target = &m_compositeRenderTexture;
			}
			else
			{
				target = m_window.get();
			}

			for (const auto& sprite : drawGroup.renderTextureSprites)
			{
				target->draw(sprite);
			}

			for (const auto& vaPair : drawGroup.vertexArrays)
			{
				const sf::Texture& texture = m_textureMap.at(vaPair.first);
				sf::RenderStates states{ &texture };
				target->draw(vaPair.second, states);
			}

			for (const auto& text : drawGroup.texts)
			{
				target->draw(*text);
			}

			if (globals::DEBUG_MODE)
			{
				target->draw(m_debugVertexArray);
			}

			// If we've finished a composite group, draw it now
			if (layer.groupIndex != -1)
			{
				if (i == m_renderLayers.size() - 1 || m_renderLayers[i + 1].groupIndex != layer.groupIndex)
				{
					m_compositeRenderTexture.display();
					sf::Sprite compositeSprite;
					compositeSprite.setTexture(m_compositeRenderTexture.getTexture());
					sf::RenderStates states;

					if (layer.isLit)
					{
						AddPointLights(m_compositeRenderTexture.getSize());
						states.shader = &m_lightingShader;
					}
					
					m_window->draw(compositeSprite, states);
				}
			}
		}
		m_window->display();
	}
};