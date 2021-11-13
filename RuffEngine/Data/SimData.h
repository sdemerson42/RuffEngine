#pragma once

#include "../Systems/RenderSystem.h"

#include <string>
#include <vector>

namespace data
{
	struct SimData
	{
		std::string name;
		unsigned int winX;
		unsigned int winY;
		std::string dbPath;
		std::string scriptPath;
		std::string soundPath;
		std::vector<std::string> soundBuffers;
		std::vector<systems::RenderSystem::RenderLayer> renderLayers;
	};
}
