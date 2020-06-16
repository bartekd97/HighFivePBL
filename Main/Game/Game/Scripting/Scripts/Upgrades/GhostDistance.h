#pragma once

#include "../../Script.h"
#include "Event/Events.h"
#include "HFEngine.h"

namespace Upgrade {
	class GhostDistance : public ::Script
	{
	private:
		float increaseBy = 0.0f;

	public:
		GhostDistance()
		{
			RegisterFloatParameter("increaseBy", &increaseBy);
		}


		void Awake()
		{
			EventManager::AddScriptListener(SCRIPT_LISTENER(Events::StatModification::GHOST_MAX_DISTANCE, GhostDistance::OnModGhostDistance));
		}

		void OnModGhostDistance(Event& ev)
		{
			float value = ev.GetParam<float>(Events::StatModification::FloatValue);
			value += increaseBy;
			ev.SetParam(Events::StatModification::FloatValue, value);
		}
	};
}