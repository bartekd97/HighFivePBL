#pragma once

#include "../../Script.h"
#include "Event/Events.h"
#include "HFEngine.h"

namespace Upgrade {
	class PushbackDistance : public ::Script
	{
	private:
		float increaseBy = 0.0f;

	public:
		PushbackDistance()
		{
			RegisterFloatParameter("increaseBy", &increaseBy);
		}


		void Awake()
		{
			EventManager::AddScriptListener(SCRIPT_LISTENER(Events::StatModification::PUSHBACK_DISTANCE, PushbackDistance::OnModPushbackDistance));
		}

		void OnModPushbackDistance(Event& ev)
		{
			float value = ev.GetParam<float>(Events::StatModification::FloatValue);
			value += increaseBy;
			ev.SetParam(Events::StatModification::FloatValue, value);
		}
	};
}