#pragma once

#include "gtest/gtest.h"

#include "../ECSPrimitives/Entity.h"
#include "../ECSPrimitives/ComponentBase.h"

namespace unit_tests
{
	class EntityTest : public testing::Test
	{
	protected:
		void SetUp() override;
		void TearDown() override;

		ecs::Entity m_entity;
	};
}