#pragma once

#include "../../Script.h"
#include "Event/Events.h"
#include "HFEngine.h"

namespace Upgrade {
	class GhostActiveLines : public ::Script
	{
	private:
		int increaseBy = 0;

	public:
		GhostActiveLines()
		{
			RegisterIntParameter("increaseBy", &increaseBy);
		}


		void Awake()
		{
			EventManager::AddScriptListener(SCRIPT_LISTENER(Events::StatModification::GHOST_MAX_ACTIVE_LINES, GhostActiveLines::OnModGhostActiveLines));
		}

		void OnModGhostActiveLines(Event& ev)
		{
			int value = ev.GetParam<int>(Events::StatModification::IntValue);
			value += increaseBy;
			ev.SetParam(Events::StatModification::IntValue, value);
		}
	};
}