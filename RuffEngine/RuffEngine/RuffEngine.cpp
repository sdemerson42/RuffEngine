#include "Sim.h"
#include "../Util/Logger.h"
#include "Globals.h"
#include <time.h>

#include <memory>

int main()
{
	util::Logger::Log("Starting RuffEngine session.");

	srand(time(NULL));

	std::unique_ptr<ruff_engine::Sim> sim =
		std::make_unique<ruff_engine::Sim>();

	bool result = sim->Initialize();
	if (!result)
	{
		util::Logger::Log("Warning: Sim failed to initialize.");
		return 1;
	}

	sim->Execute();
	
	util::Logger::Log("RuffEngine session complete.");

	return 0;
}
