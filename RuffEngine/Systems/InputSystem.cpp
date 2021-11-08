#include "pch.h"
#include "InputSystem.h"

#include "SFML/Window/Keyboard.hpp"

namespace systems
{
	InputSystem::InputData InputSystem::s_inputData;

	void InputSystem::Execute()
	{
		// TO DO: Configurable input

		s_inputData.leftStick.x = 0.0f;
		s_inputData.leftStick.y = 0.0f;
		s_inputData.rightStick.x = 0.0f;
		s_inputData.rightStick.y = 0.0f;
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

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
		{
			if (s_inputData.button < 2)
			{
				s_inputData.button++;
			}
		}
		else
		{
			s_inputData.button = 0;
		}
	}
};