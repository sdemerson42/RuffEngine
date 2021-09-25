#include "Blueprint.h"
#include "../Util/Logger.h"

namespace data
{
	bool LoadBlueprintData(
		const std::string& dbPath,
		/*out*/std::vector<Blueprint>& blueprints)
	{

		SqlQueryResult entityQueryResult;
		if (!SqlQuery::SubmitQuery(dbPath, "SELECT * FROM Entities;", entityQueryResult))
		{
			util::Logger::Log("Warning: Failed to load entity data from " + dbPath + ".");
			return false;
		}

		int entityIndex = 0;
		const int entityTotal = entityQueryResult["id"].size();
		while (entityIndex < entityTotal)
		{
			Blueprint blueprint;
			blueprint.name = entityQueryResult["name"][entityIndex];
			blueprint.id = std::stoi(entityQueryResult["id"][entityIndex]);
			
			// Load component data
			SqlQueryResult mutableQueryResult;

			// Render components
			SqlQuery::SubmitQuery(dbPath,
				"SELECT * FROM Render_Data WHERE entity_id = " + std::to_string(blueprint.id) + ";",
				mutableQueryResult);
			if (mutableQueryResult.size() != 0)
			{
				blueprint.componentData.push_back(mutableQueryResult);
			}
			mutableQueryResult.clear();

			// Animation components
			SqlQuery::SubmitQuery(dbPath,
				"SELECT * FROM Animation_Data WHERE entity_id = " + std::to_string(blueprint.id) + ";",
				mutableQueryResult);
			if (mutableQueryResult.size() != 0)
			{
				blueprint.componentData.push_back(mutableQueryResult);
			}
			mutableQueryResult.clear();

			blueprints.push_back(blueprint);
			++entityIndex;
		}

		return true;
	}
}