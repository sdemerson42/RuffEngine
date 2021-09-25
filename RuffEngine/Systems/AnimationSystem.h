#pragma once

#include "../ECSPrimitives/ISystem.h"

namespace systems
{
	class AnimationSystem : public ecs::ISystem
	{
	public:
		void Execute() override;
	};
}
