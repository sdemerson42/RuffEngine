#pragma once

#include "../ECSPrimitives/ComponentBase.h"
#include "../ECSPrimitives/Range.h"
#include "../ECSPrimitives/Box2f.h"
#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Color.hpp"
#include "../ECSPrimitives/Autolist.h"

#include <string>
#include <unordered_map>

namespace ecs
{
	class Entity;
}

namespace components
{
	class ParticleComponent : public ecs::ComponentBase, public ecs::Autolist<ParticleComponent>
	{
	public:
		struct Particle
		{
			bool active = false;
			ecs::Box2f drawBox;
			float life;
			float lifeCounter;
			sf::Color color;
			sf::Vector2f velocity;
		};

		enum class EmitterShape
		{
			CONE
		};

		ParticleComponent(ecs::Entity* parent);

		void Initialize() override;

		void PostStateSetup();

		EmitterShape GetEmitterShape() const
		{
			return m_emitterShape;
		}
		void SetEmitterShape(EmitterShape shape)
		{
			m_emitterShape = shape;
		}

		void AddEmitterVar(const std::string& key, float value)
		{
			m_emitterVars[key] = value;
		}
		float GetEmitterVar(const std::string& key) const
		{
			return m_emitterVars.at(key);
		}

		const std::string& GetTexturePath() const
		{
			return m_texturePath;
		}
		void SetTexturePath(const std::string& path)
		{
			m_texturePath = path;
		}

		const std::string& GetRenderLayer() const
		{
			return m_renderLayer;
		}
		void SetRenderLayer(const std::string& path)
		{
			m_renderLayer = path;
		}

		const ecs::Box2f& GetTextureBox() const
		{
			return m_textureBox;
		}
		void SetTextureBox(float centerX, float centerY, float halfX, float halfY)
		{
			m_textureBox = ecs::Box2f{ centerX, centerY, halfX, halfY };
		}
		void SetTextureBox(const sf::Vector2f& center, const sf::Vector2f& halfSize)
		{
			m_textureBox = ecs::Box2f{ center, halfSize };
		}

		bool GetIsUniformScaling() const
		{
			return m_isUniformScaling;
		}
		void SetIsUniformScaling(bool value)
		{
			m_isUniformScaling = value;
		}

		const ecs::Range<sf::Vector2f>& GetScaleRange() const
		{
			return m_scaleRange;
		}
		void SetScaleRange(float minX, float maxX, float minY, float maxY)
		{
			m_scaleRange = { sf::Vector2f{minX, minY}, sf::Vector2f{maxX, maxY} };
		}

		const ecs::Range<sf::Color>& GetColorRange() const
		{
			return m_colorRange;
		}
		void SetColorRange(const sf::Color& minColor, const sf::Color& maxColor)
		{
			m_colorRange = { minColor, maxColor };
		}

		const ecs::Range<float>& GetLifeRange() const
		{
			return m_lifeRange;
		}
		void SetLifeRange(float minSeconds, float maxSeconds)
		{
			m_lifeRange = { minSeconds, maxSeconds };
		}

		const ecs::Range<float>& GetSpeedRange() const
		{
			return m_speedRange;
		}
		void SetSpeedRange(float minSpeed, float maxSpeed)
		{
			m_speedRange = { minSpeed, maxSpeed };
		}

		float GetSpawnRate() const
		{
			return m_spawnRate;
		}
		void SetSpawnRate(float spawnPerSecond)
		{
			m_spawnRate = spawnPerSecond;
			m_particles.resize((int)m_spawnRate);
		}

		int IncSpawnCounter(float deltaTime)
		{
			m_spawnCounter += deltaTime * m_spawnRate;
			if (m_spawnCounter >= 1.0f)
			{
				int rVal = (int)m_spawnCounter;
				m_spawnCounter -= (float)rVal;
				return rVal;
			}
			return 0;
		}

		std::vector<Particle>& GetParticles()
		{
			return m_particles;
		}

	private:
		EmitterShape m_emitterShape;
		std::unordered_map<std::string, float> m_emitterVars;
		std::string m_texturePath;
		std::string m_renderLayer;
		ecs::Box2f m_textureBox;
		bool m_isUniformScaling;
		ecs::Range<sf::Vector2f> m_scaleRange;
		ecs::Range<sf::Color> m_colorRange;
		ecs::Range<float> m_lifeRange;
		ecs::Range<float> m_speedRange;
		float m_spawnRate;

		std::vector<Particle> m_particles;
		float m_spawnCounter = 0.0f;
	};
}
