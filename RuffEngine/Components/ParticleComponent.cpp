#include "pch.h"
#include "ParticleComponent.h"
#include "../Util/Math.h"

namespace components
{
	void ParticleComponent::Initialize()
	{
		switch (m_emitterShape)
		{
		case EmitterShape::CONE:
		{
			sf::Vector2f startVector = { m_emitterVars["startX"], m_emitterVars["startY"] };
			util::Math::NormalizeVector(startVector);
			m_emitterVars["startX"] = startVector.x;
			m_emitterVars["startY"] = startVector.y;
			m_emitterVars["angleRadians"] = util::Math::DegreesToRadians(
				m_emitterVars["angleDegrees"]);
			break;
		}
		}
	}
}