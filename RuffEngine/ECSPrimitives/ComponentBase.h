#pragma once

namespace ecs
{
	class Entity;

	class ComponentBase
	{
		friend class ComponentBank;
	public:
		ComponentBase();
		virtual ~ComponentBase()
		{}
		Entity* const GetParent();
	protected:
		Entity* m_parent;
	};
}
