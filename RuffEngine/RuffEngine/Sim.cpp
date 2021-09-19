#include "Sim.h"

namespace ruff_engine
{

	Sim::~Sim()
	{
		m_systems.clear();
	}

	void Sim::Execute()
	{
		// Main engine loop

		/*while (m_window->isOpen())
		{
			sf::Event event;
			while (m_window->pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					m_window->close();
				}
			}

			for (auto& system : m_systems)
			{
				system->Execute();
			}
		}*/
	}
}