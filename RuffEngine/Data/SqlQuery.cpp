#include "SqlQuery.h"
#include "sqlite3.h"

#include "../Util/Logger.h"

namespace data
{
	SqlQueryResult SqlQuery::s_queryResult;

	int SqlQueryCallback(void* notUsed, int argc, char** argv, char** azColName)
	{
		for (int i = 0; i < argc; ++i)
		{
			auto columnName = azColName[i];
			if (argv[i] == nullptr)
			{
				SqlQuery::s_queryResult[columnName].push_back("");
			}
			else
			{
				SqlQuery::s_queryResult[columnName].push_back(argv[i]);
			}
		}
		return 0;
	}

	bool SqlQuery::SubmitQuery(
		const std::string& dbPath, 
		const std::string& query, 
		/*out*/SqlQueryResult& result)
	{
		s_queryResult.clear();

		sqlite3* dataBase;
		char* zErrMsg = 0;
		int resultValue;

		resultValue = sqlite3_open_v2(dbPath.c_str(), &dataBase, SQLITE_OPEN_READONLY, nullptr);
		if (resultValue != 0) 
		{
			std::string errMsg = "Warning: Failed to open database " + dbPath + " - ";
			errMsg += sqlite3_errmsg(dataBase);
			util::Logger::Log(errMsg);
			sqlite3_close(dataBase);
			return false;
		}
		
		resultValue = sqlite3_exec(dataBase, query.c_str(), SqlQueryCallback, 0, &zErrMsg);
		if (resultValue != SQLITE_OK) 
		{
			std::string errMsg = "Warning: SQL error - ";
			errMsg += zErrMsg;
			util::Logger::Log(errMsg);
			sqlite3_free(zErrMsg);
			return false;
		}

		sqlite3_close(dataBase);
		result = s_queryResult;
		s_queryResult.clear();

		return true;
	}
}