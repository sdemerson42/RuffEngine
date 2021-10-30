#pragma once

#include "../ECSPrimitives/ISystem.h"
#include "../Components/ParticleComponent.h"

namespace systems
{
	class ParticleSystem : public ecs::ISystem
	{
	public:
		void Execute() override;
		void CreateParticle(
			components::ParticleComponent* pc,
			components::ParticleComponent::Particle* particle);
		void UpdateParticles(
			std::vector<components::ParticleComponent::Particle>& particles,
			float deltaTime);
	};
}
