#include "Sim.h"

#include <memory>

int main()
{
	std::unique_ptr<ruff_engine::Sim> sim =
		std::make_unique<ruff_engine::Sim>();

	sim->Execute();
}
