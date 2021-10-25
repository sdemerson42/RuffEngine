#include "pch.h"
#include "InputSystem.h"

#include "SFML/Window/Keyboard.hpp"

namespace systems
{
	InputSystem::InputData InputSystem::s_inputData;

	void InputSystem::Execute()
	{
		// TO DO: Configurable input
		s_inputData = InputData{};

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			s_inputData.leftStick.y = -1.0f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			s_inputData.leftStick.y = 1.0f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			s_inputData.leftStick.x = -1.0f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			s_inputData.leftStick.x = 1.0f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))
		{
			s_inputData.rightStick.y = -1.0f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::K))
		{
			s_inputData.rightStick.y = 1.0f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::J))
		{
			s_inputData.rightStick.x = -1.0f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
		{
			s_inputData.rightStick.x = 1.0f;
		}
	}
};