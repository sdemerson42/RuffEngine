#include "pch.h"
#include "EventHandler.h" 

#include <algorithm>

namespace util
{
	std::unordered_map<std::type_index, std::vector<EventHandler*>> EventHandler::s_handlerMap;

	EventHandler::~EventHandler()
	{
		for (auto& pair : s_handlerMap)
		{
			auto& handlers = pair.second;
			auto iter = std::find(handlers.begin(), handlers.end(), this);
			if (iter != handlers.end())
			{
				handlers.erase(iter);
			}
		}
	}

	void EventHandler::HandleEvent(const EventBase* event)
	{
		auto iter = m_delegateMap.find(std::type_index{ typeid(*event) });
		if (iter != m_delegateMap.end())
		{
			iter->second->Call(event);
		}
	}

	void EventHandler::SendEvent(const EventBase* event)
	{
		std::type_index eventIndex = typeid(*event);
		auto& handlers = s_handlerMap[eventIndex];
		for (auto handler : handlers)
		{
			if (handler != this)
			{
				handler->HandleEvent(event);
			}
		}
	}
}