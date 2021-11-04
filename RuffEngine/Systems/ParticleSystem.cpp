#include "pch.h"
#include "ParticleSystem.h"
#include "../Components/ComponentBank.h"
#include "../Util/Time.h"
#include "../Util/Math.h"
#include "../Util/Logger.h"

namespace systems
{
	void ParticleSystem::Execute()
	{
		float deltaTime = util::Time::DeltaTime();
		auto sz = ecs::ComponentBank::m_particleComponentsSize;
		for (int i = 0; i < sz; ++i)
		{
			components::ParticleComponent& pc =
				ecs::ComponentBank::m_particleComponents[i];

			if (!pc.GetIsActive() || !pc.GetParent()->GetIsActive())
			{
				continue;
			}

			auto& particles = pc.GetParticles();

			// Spawn new particles

			int spawnTotal = pc.IncSpawnCounter(deltaTime);
			if (spawnTotal > 0)
			{
				for (int j = 0; j < particles.size(); ++j)
				{
					auto& particle = particles[j];
					if (!particle.active)
					{
						CreateParticle(&pc, &particle);
						if (--spawnTotal == 0) break;
					}
				}

				if (spawnTotal > 0)
				{
					for (int j = 0; j < spawnTotal; ++j)
					{
						particles.push_back(components::ParticleComponent::Particle{});
						auto& particle = particles.back();
						CreateParticle(&pc, &particle);
					}
				}
			}
			// Update existing particles
			UpdateParticles(particles, deltaTime);
		}
	}

	void ParticleSystem::CreateParticle(components::ParticleComponent* pc,
		components::ParticleComponent::Particle* particle)
	{
		// Position and velocity

		const auto& speedRange = pc->GetSpeedRange();
		float speed = util::Math::RandomRangeFloat(speedRange.min, speedRange.max);

		switch (pc->GetEmitterShape())
		{
		case components::ParticleComponent::EmitterShape::CONE:
		{
			const auto& parentPosition =
				pc->GetParent()->GetPosition();
			particle->drawBox.center = { parentPosition.x + pc->GetEmitterVar("offsetX"),
				parentPosition.y + pc->GetEmitterVar("offsetY") };

			float angle = util::Math::RandomRangeFloat(0.0f, pc->GetEmitterVar("angleRadians"));
			sf::Vector2f normal = { pc->GetEmitterVar("startX"), pc->GetEmitterVar("startY") };
			normal = util::Math::Rotate(normal, angle);
			particle->velocity = normal * speed;
		}
		}

		// Color
		const auto& colorRange = pc->GetColorRange();
		sf::Color minColor = colorRange.min;
		sf::Color maxColor = colorRange.max;
		particle->color = {
			sf::Uint8(rand() % (maxColor.r - minColor.r + 1) + minColor.r),
			sf::Uint8(rand() % (maxColor.g - minColor.g + 1) + minColor.g),
			sf::Uint8(rand() % (maxColor.b - minColor.b + 1) + minColor.b),
			sf::Uint8(rand() % (maxColor.a - minColor.a + 1) + minColor.a) };

		// Half size
		const auto& scaleRange = pc->GetScaleRange();
		float halfX = pc->GetTextureBox().halfSize.x *
			util::Math::RandomRangeFloat(scaleRange.min.x, scaleRange.max.x);
		particle->drawBox.halfSize = { halfX,
			pc->GetIsUniformScaling() ?
			halfX :
			pc->GetTextureBox().halfSize.y * 
			util::Math::RandomRangeFloat(scaleRange.min.y, scaleRange.max.y) };

		// Life
		const auto& lifeRange = pc->GetLifeRange();
		particle->life = util::Math::RandomRangeFloat(lifeRange.min, lifeRange.max);

		particle->lifeCounter = 0.0f;
		particle->active = true;
	}

	void ParticleSystem::UpdateParticles(
		std::vector<components::ParticleComponent::Particle>& particles,
		float deltaTime)
	{
		for (auto& particle : particles)
		{
			// Process lifetime and move if still active
			particle.lifeCounter += deltaTime;
			if (particle.lifeCounter >= particle.life)
			{
				particle.active = false;
				continue;
			}

			particle.drawBox.center +=
				particle.velocity * deltaTime;
		}
	}
}