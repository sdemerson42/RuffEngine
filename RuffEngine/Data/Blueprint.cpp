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

			// Add component data

			ComponentQuery(dbPath, "Render_Data", blueprint);
			ComponentQuery(dbPath, "Animation_Data", blueprint);
			ComponentQuery(dbPath, "Script_Data", blueprint);
			ComponentQuery(dbPath, "Physics_Data", blueprint);
			ComponentQuery(dbPath, "Particle_Data", blueprint);

			blueprints.push_back(blueprint);
			++entityIndex;
		}

		return true;
	}

	void ComponentQuery(
		const std::string& dbPath,
		const std::string& tableName,
		/*out*/Blueprint& blueprint)
	{
		SqlQueryResult queryResult;

		SqlQuery::SubmitQuery(dbPath,
			"SELECT * FROM " + tableName + " WHERE entity_id = " + std::to_string(blueprint.id) + ";",
			queryResult);
		if (queryResult.size() != 0)
		{
			blueprint.componentData.push_back(queryResult);
		}
	}
}