#pragma once

#include "../ECSPrimitives/ComponentBase.h"
#include "../AngelScript/include/angelscript.h"
#include "SFML/System/Vector2.hpp"

#include <string>

namespace components
{
	class ScriptComponent : public ecs::ComponentBase
	{
	public:
		void PrepareScriptContext(asIScriptEngine* scriptEngine, const std::string& mainPrefix);
		void ExecuteScript();

		// Script API methods
		void Suspend(int cycles);
		void Log(const std::string& msg) const;
		float DeltaTime() const;
		const sf::Vector2f& GetPosition() const;
		void SetPosition(float x, float y);
	private:
		asIScriptContext* m_scriptContext{ nullptr };
		int m_suspendCycleCounter;
		int m_suspendCycleTotal;
	};
};
