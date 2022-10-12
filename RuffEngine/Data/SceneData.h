#pragma once

#include <string>
#include <vector>

namespace data
{
	struct SceneData
	{
		struct SceneEntityData
		{
			std::string name;
			float x;
			float y;
			bool isPersistent;
			bool isCreated;
			bool isActive;
			int count;
			std::string initData;
		};

		int id;
		std::vector<SceneEntityData> entityData;
		std::vector<int> subScenes;
	};

}
