#pragma once

#include "SqlQuery.h"

#include <string>

namespace data
{
	struct Blueprint
	{
		std::string name;
		int id;
		std::vector<SqlQueryResult> componentData;
	};

	bool LoadBlueprintData(
		const std::string& dbPath,
		/*out*/std::vector<Blueprint>& blueprints);

	void ComponentQuery(
		const std::string& dbPath,
		const std::string& tableName,
		/*out*/Blueprint& blueprint);
}
