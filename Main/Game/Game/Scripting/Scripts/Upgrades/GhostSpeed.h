#pragma once

#include "../../Script.h"
#include "Event/Events.h"
#include "HFEngine.h"

namespace Upgrade {
	class GhostSpeed : public ::Script
	{
	private:
		float increaseBy = 0.0f;

	public:
		GhostSpeed()
		{
			RegisterFloatParameter("increaseBy", &increaseBy);
		}

		void Awake()
		{
			EventManager::AddScriptListener(SCRIPT_LISTENER(Events::StatModification::GHOST_MOVE_SPEED, GhostSpeed::OnModGhostMoveSpeed));
		}

		void OnModGhostMoveSpeed(Event& ev)
		{
			float value = ev.GetParam<float>(Events::StatModification::FloatValue);
			value += increaseBy;
			ev.SetParam(Events::StatModification::FloatValue, value);
		}
	};
}