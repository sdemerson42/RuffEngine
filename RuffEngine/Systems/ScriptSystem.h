#pragma once

#include "../ECSPrimitives/ISystem.h"

namespace systems
{
	class ScriptSystem : public ecs::ISystem
	{
	public:
		void Execute() override;
	};
}
