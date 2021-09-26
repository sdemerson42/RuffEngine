#include "Sim.h"
#include "../Util/Logger.h"

#include <memory>

int main()
{
	std::unique_ptr<ruff_engine::Sim> sim =
		std::make_unique<ruff_engine::Sim>();

	bool result = sim->Initialize();
	if (!result)
	{
		util::Logger::Log("Warning: Sim failed to initialize.");
		return 1;
	}

	sim->Execute();
	
	return 0;
}
