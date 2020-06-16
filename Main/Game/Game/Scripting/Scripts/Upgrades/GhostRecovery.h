#pragma once

#include "../../Script.h"
#include "Event/Events.h"
#include "HFEngine.h"

namespace Upgrade {
	class GhostRecovery : public ::Script
	{
	private:
		float increaseBy = 0.0f;

	public:
		GhostRecovery()
		{
			RegisterFloatParameter("increaseBy", &increaseBy);
		}

		void Awake()
		{
			EventManager::AddScriptListener(SCRIPT_LISTENER(Events::StatModification::GHOST_RECOVERY_SPEED, GhostRecovery::OnModGhostRecoverySpeed));
		}

		void OnModGhostRecoverySpeed(Event& ev)
		{
			float value = ev.GetParam<float>(Events::StatModification::FloatValue);
			value += increaseBy;
			ev.SetParam(Events::StatModification::FloatValue, value);
		}
	};
}