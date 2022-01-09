#pragma once

#include "EventBase.h"

namespace util
{
	struct ChangeSceneEvent : public EventBase
	{
		int sceneId;
	};
}
