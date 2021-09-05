#pragma once

namespace ecs
{
	class Entity;

	class ComponentBase
	{
	public:
		virtual ~ComponentBase()
		{}
		Entity* const GetParent();
	protected:
		Entity* m_parent;
	};
}
