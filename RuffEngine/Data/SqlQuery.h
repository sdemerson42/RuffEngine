#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace data
{
	using SqlQueryResult = std::unordered_map<std::string, std::vector<std::string>>;

	int SqlQueryCallback(void* notUsed, int argc, char** argv, char** azColName);

	class SqlQuery
	{
	public:
		static bool SubmitQuery(
			const std::string& dbPath,
			const std::string& query,
			/*out*/SqlQueryResult& result);

		static SqlQueryResult s_queryResult;
	};
}
