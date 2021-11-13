#pragma once

#include "SimData.h"
#include "SqlQuery.h"
#include "../Util/Logger.h"
#include "../Systems/RenderSystem.h"

#include <vector>
#include <string>

namespace data
{
	class Parse
	{
	public:
		static std::vector<std::string> ProcessMultiValueField(const std::string& values)
		{
			std::vector<std::string> valueVector;
			std::string s;

			for (char c : values)
			{
				if (c == ' ') continue;
				if (c == ',')
				{
					valueVector.push_back(s);
					s.clear();
					continue;
				}
				s += c;
			}
			valueVector.push_back(s);

			return valueVector;
		}

		static bool LoadSimData(/*out*/SimData& simData)
		{
			// Hard-coded data path
			simData.dbPath = "Sim/Sim.db";

			SqlQueryResult query;
			if (!SqlQuery::SubmitQuery(simData.dbPath,
				"SELECT * FROM SIM_DATA", query))
			{
				util::Logger::Log("Warning: Could not find sim configuration data.");
				return false;
			}

			simData.name = query["name"][0];
			simData.winX = std::stoi(query["win_x"][0]);
			simData.winY = std::stoi(query["win_y"][0]);
			simData.scriptPath = query["script_path"][0];
			if (simData.scriptPath.back() != '/') simData.scriptPath += '/';
			simData.soundPath = query["sound_path"][0];
			if (simData.soundPath.back() != '/') simData.soundPath += '/';
			simData.soundBuffers = ProcessMultiValueField(query["sound_buffers"][0]);
			
			auto renderLayerData = ProcessMultiValueField(query["render_layers"][0]);
			for (int i = 0; i < renderLayerData.size(); i += 2)
			{
				systems::RenderSystem::RenderLayer layer{
					renderLayerData[i],
					renderLayerData[i + 1] == "static" ? true : false };
				simData.renderLayers.push_back(layer);
			}

			return true;
		}
	};
}
