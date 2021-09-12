#include "gtest/gtest.h"

#include "../Data/SqlQuery.h"

#include <string>

namespace unit_tests
{
	TEST(DataTest, SqlQueryTest)
	{
		data::SqlQueryResult queryResult;
		const std::string goodQuery = "SELECT * FROM data ORDER BY var;";
		const std::string badQuery = "SELECT * FROM invalid_table;";
		const std::string goodDbPath = "TestData/QueryTest.db";
		const std::string badDbPath = "TestData/Invalid.db";

		EXPECT_FALSE(data::SqlQuery::SubmitQuery(badDbPath, goodQuery, queryResult));
		EXPECT_FALSE(data::SqlQuery::SubmitQuery(goodDbPath, badQuery, queryResult));
		ASSERT_TRUE(data::SqlQuery::SubmitQuery(goodDbPath, goodQuery, queryResult));
		
		constexpr int testDataSize = 5;
		ASSERT_EQ(queryResult["value"].size(), testDataSize);
		EXPECT_EQ(queryResult["var"][0], "a");
		EXPECT_EQ(queryResult["value"][0], "1");
		EXPECT_EQ(queryResult["var"][1], "b");
		EXPECT_EQ(queryResult["value"][1], "2");
		EXPECT_EQ(queryResult["var"][2], "c");
		EXPECT_EQ(queryResult["value"][2], "3");
		EXPECT_EQ(queryResult["var"][3], "d");
		EXPECT_EQ(queryResult["value"][3], "4");
		EXPECT_EQ(queryResult["var"][4], "e");
		EXPECT_EQ(queryResult["value"][4], "5");
	}
}