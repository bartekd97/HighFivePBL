#pragma once

#include "../../Script.h"
#include "Event/Events.h"
#include "HFEngine.h"

namespace Upgrade {
	class PushbackForce : public ::Script
	{
	private:
		float increaseBy = 0.0f;

	public:
		PushbackForce()
		{
			RegisterFloatParameter("increaseBy", &increaseBy);
		}


		void Awake()
		{
			EventManager::AddScriptListener(SCRIPT_LISTENER(Events::StatModification::PUSHBACK_FORCE, PushbackForce::OnModPushbackForce));
		}

		void OnModPushbackForce(Event& ev)
		{
			float value = ev.GetParam<float>(Events::StatModification::FloatValue);
			value += increaseBy;
			ev.SetParam(Events::StatModification::FloatValue, value);
		}
	};
}