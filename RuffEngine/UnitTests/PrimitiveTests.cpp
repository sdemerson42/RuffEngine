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

		EXPECT_TRUE(CONNECT_COMP(&m_entity1, RenderComponent));
		EXPECT_NE(m_entity1.GetComponent<RenderComponent>(), nullptr);
		EXPECT_TRUE(CONNECT_COMP(&m_entity2, RenderComponent));
		EXPECT_NE(m_entity2.GetComponent<RenderComponent>(), nullptr);
		EXPECT_TRUE(CONNECT_COMP(&m_entity3, RenderComponent));
		EXPECT_NE(m_entity3.GetComponent<RenderComponent>(), nullptr);

		EXPECT_TRUE(DISCONNECT_COMP(&m_entity2, RenderComponent));
		EXPECT_EQ(m_entity2.GetComponent<RenderComponent>(), nullptr);
		ASSERT_NE(m_entity1.GetComponent<RenderComponent>(), nullptr);
		ASSERT_NE(m_entity3.GetComponent<RenderComponent>(), nullptr);

		EXPECT_EQ(
			m_entity1.GetComponent<RenderComponent>()->GetParent(),
			&m_entity1);
		EXPECT_EQ(
			m_entity3.GetComponent<RenderComponent>()->GetParent(),
			&m_entity3);

		EXPECT_TRUE(DISCONNECT_COMP(&m_entity1, RenderComponent));
		EXPECT_TRUE(DISCONNECT_COMP(&m_entity3, RenderComponent));

		for (int i = 0; i < globals::TOTAL_COMPONENTS; ++i)
		{
			EXPECT_TRUE(CONNECT_COMP(&m_entity1, RenderComponent));
		}
		EXPECT_FALSE(CONNECT_COMP(&m_entity1, RenderComponent));
	}

}