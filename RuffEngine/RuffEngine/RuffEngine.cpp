#include "Sim.h"

#include <memory>
#include <cassert>

int main()
{
	std::unique_ptr<ruff_engine::Sim> sim =
		std::make_unique<ruff_engine::Sim>();

	assert(sim->Initialize());
	sim->Execute();
}
