#include "gtest/gtest.h"
#include "EntityTest.h"
#include "../Components/Components.h"
#include "../Components/ComponentBank.h"
#include "../RuffEngine/Globals.h"

namespace unit_tests
{
	using namespace components;

	TEST_F(EntityTest, EntityComponentConnections)
	{
		ecs::Entity* nullEntity = nullptr;
		EXPECT_FALSE(CONNECT_COMP(nullEntity, RenderComponent));
		EXPECT_FALSE(DISCONNECT_COMP(nullEntity, RenderComponent));
		EXPECT_FALSE(DISCONNECT_COMP(&m_entity1, RenderComponent));
		EXPECT_TRUE(CONNECT_COMP(&m_entity1, RenderComponent));
		EXPECT_TRUE(DISCONNECT_COMP(&m_entity1, RenderComponent));
		EXPECT_FALSE(DISCONNECT_COMP(&m_entity1, RenderComponent));

		for (int i = 0; i < globals::TOTAL_COMPONENTS; ++i)
		{
			EXPECT_TRUE(CONNECT_COMP(&m_entity1, RenderComponent));
		}
		EXPECT_FALSE(CONNECT_COMP(&m_entity1, RenderComponent));
	}

}