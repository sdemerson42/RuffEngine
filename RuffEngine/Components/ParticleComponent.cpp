#include "pch.h"
#include "ParticleComponent.h"
#include "../Util/Math.h"
#include "../ECSPrimitives/Entity.h"

namespace components
{
	ParticleComponent::ParticleComponent(ecs::Entity* parent, 
		const std::string& sceneLayer) :
		ComponentBase{ parent }, Autolist<ParticleComponent>{ sceneLayer }
	{}

	void ParticleComponent::Initialize()
	{
		m_emitterVars.clear();
		m_particles.clear();
		m_spawnCounter = 0.0f;
	}

	void ParticleComponent::PostStateSetup()
	{
		switch (m_emitterShape)
		{
		case EmitterShape::CONE:
		{
			m_emitterVars["angleRadians"] = util::Math::DegreesToRadians(
				m_emitterVars["angleDegrees"]);
			sf::Vector2f startVector = { m_emitterVars["startX"], m_emitterVars["startY"] };
			sf::Vector2f offset = { m_emitterVars["offsetX"], m_emitterVars["offsetY"] };
			float parentRotation = util::Math::DegreesToRadians(GetParent()->GetRotation());
			startVector = util::Math::Rotate(startVector, parentRotation);
			offset = util::Math::Rotate(offset, parentRotation);
			util::Math::NormalizeVector(startVector);
			m_emitterVars["psStartX"] = startVector.x;
			m_emitterVars["psStartY"] = startVector.y;
			m_emitterVars["psOffsetX"] = offset.x;
			m_emitterVars["psOffsetY"] = offset.y;
			break;
		}
		}
	}
}