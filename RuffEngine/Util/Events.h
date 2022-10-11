#pragma once

#include "EventBase.h"

namespace util
{
	struct ChangeSceneEvent : public EventBase
	{
		int sceneId;
	};

	struct SetPauseEvent : public EventBase
	{
		bool value;
	};

	struct SetPhysicsCellSizeEvent : public EventBase
	{
		int x;
		int y;
	};
}
