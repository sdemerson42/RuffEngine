#pragma once

#include "../ECSPrimitives/ISystem.h"
#include "SFML/System/Vector2.hpp"

namespace systems
{
	class InputSystem : public ecs::ISystem
	{
	public:
		void Execute() override;

		struct InputData
		{
			InputData() :
				leftStick{ 0.0f, 0.0f },
				rightStick{ 0.0f, 0.0f },
				button{ 0 }
			{}
			sf::Vector2f leftStick;
			sf::Vector2f rightStick;
			int button;
		};

		static InputData s_inputData;
	};
}