#pragma once

namespace ecs
{
	class ISystem
	{
	public:
		virtual ~ISystem()
		{}
		virtual void Execute() = 0;
	};
};
