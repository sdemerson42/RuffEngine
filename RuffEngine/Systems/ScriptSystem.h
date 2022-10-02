#pragma once

#include "../ECSPrimitives/ISystem.h"
#include "../Util/EventHandler.h"
#include "../Util/Events.h"

namespace systems
{
	class ScriptSystem : public ecs::ISystem, public util::EventHandler
	{
	public:
		ScriptSystem();
		void Execute() override;
	private:
		bool m_isPaused{ false };

		void OnSetPauseEvent(const util::SetPauseEvent* event);
	};
}
