#pragma once

#include "EventBase.h"

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <vector>

namespace util
{
	template<typename T, typename EventT>
	using MFunc = void(T::*)(const EventT*);

	class IDelegate
	{
	public:
		virtual void Call(const EventBase* event) = 0;
	};

	template<typename T, typename EventT>
	class Delegate : public IDelegate
	{
	public:
		Delegate(T* object, MFunc<T, EventT> method) :
			m_object{ object }, m_method{ method }
		{}
		void Call(const EventBase* event)
		{
			(m_object->*m_method)(static_cast<const EventT*>(event));
		}
	private:
		T* m_object;
		MFunc<T, EventT> m_method;
	};

	class EventHandler
	{
	public:
		virtual ~EventHandler();

		template<typename T, typename EventT>
		void RegisterMethod(T* object, MFunc<T, EventT> method)
		{
			std::type_index eventIndex = typeid(EventT);

			m_delegateMap[eventIndex] =
				std::make_unique<Delegate<T, EventT>>(object, method);

			s_handlerMap[eventIndex].push_back(this);
		}
		void HandleEvent(const EventBase* event);
		void SendEvent(const EventBase* event);
	private:
		std::unordered_map<std::type_index, std::unique_ptr<IDelegate>> m_delegateMap;
		static std::unordered_map<std::type_index, std::vector<EventHandler*>> s_handlerMap;
	};
}
