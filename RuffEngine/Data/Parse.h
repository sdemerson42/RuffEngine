#pragma once

#include "SimData.h"
#include "SceneData.h"
#include "SqlQuery.h"
#include "../Util/Logger.h"
#include "../Systems/RenderSystem.h"

#include <vector>
#include <string>
#include <unordered_map>

namespace data
{
	class Parse
	{
	public:
		static std::vector<std::string> ProcessMultiValueField(const std::string& values)
		{
			std::vector<std::string> valueVector;

			if (values == "") return valueVector;

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
			int groupIndexCounter = 0;
			bool inLayer = false;

			for (int i = 0; i < renderLayerData.size(); i += 2)
			{
				int groupIndex = -1;
				bool isLit = false;
				auto layerName = renderLayerData[i];

				// Texture grouping logic
				if (layerName[0] == '[')
				{
					layerName = layerName.substr(1);
					inLayer = true;
					isLit = true;
				}
				if (layerName[layerName.length() - 1] == ']')
				{
					layerName = layerName.substr(0, layerName.length() - 1);
					inLayer = false;
					groupIndex = groupIndexCounter++;
				}
				if (inLayer)
				{
					groupIndex = groupIndexCounter;
					isLit = true;
				}

				systems::RenderSystem::RenderLayer layer{
					layerName,
					renderLayerData[i + 1] == "static" ? true : false,
					groupIndex,
					isLit };
				simData.renderLayers.push_back(layer);
			}

			simData.startScene = std::stoi(query["start_scene"][0]);

			return true;
		}

		static bool LoadSceneData(const std::string& dbPathName, 
			/*out*/std::unordered_map<int, SceneData>& sceneData)
		{
			SqlQueryResult result;
			if (!SqlQuery::SubmitQuery(dbPathName,
				"SELECT * FROM Scene_Data", result))
			{
				util::Logger::Log("Warning: Could not read scene data.");
				return false;
			}

			int totalScenes = result["scene_id"].size();
			for (int i = 0; i < totalScenes; ++i)
			{
				SceneData sd;
				sd.id = std::stoi(result["scene_id"][i]);
				
				auto persist = ProcessMultiValueField(result["persistent_entities"][i]);
				for (int k = 0; k < persist.size(); k += 3)
				{
					SceneData::SceneEntityData data;
					data.count = 1;
					data.isActive = true;
					data.isPersistent = true;
					data.isCreated = false;
					data.name = persist[k];
					data.x = std::stof(persist[k + 1]);
					data.y = std::stof(persist[k + 2]);
					sd.entityData.push_back(data);
				}

				auto scene = ProcessMultiValueField(result["scene_entities"][i]);
				for (int k = 0; k < scene.size(); k += 3)
				{
					SceneData::SceneEntityData data;
					data.count = 1;
					data.isActive = true;
					data.isPersistent = false;
					data.name = scene[k];
					data.x = std::stof(scene[k + 1]);
					data.y = std::stof(scene[k + 2]);
					sd.entityData.push_back(data);
				}

				auto cache = ProcessMultiValueField(result["scene_cache_entities"][i]);
				for (int k = 0; k < cache.size(); k += 2)
				{
					SceneData::SceneEntityData data;
					data.count = std::stoi(cache[k + 1]);
					data.isActive = false;
					data.isPersistent = false;
					data.name = cache[k];
					data.x = 0.0f;
					data.y = 0.0f;
					sd.entityData.push_back(data);
				}

				auto subScenes = ProcessMultiValueField(result["sub_scenes"][i]);
				for (int k = 0; k < subScenes.size(); ++k)
				{
					sd.subScenes.push_back(std::stoi(subScenes[k]));
				}
				sceneData[sd.id] = sd;
			}
			return true;
		}
	};
}
