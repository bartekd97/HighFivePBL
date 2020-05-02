#pragma once

#include "../Script.h"
#include "../../HFEngine.h"
#include "../../ECS/Components/Collider.h"

class TriggerTest : public Script
{
public:
	void Awake()
	{
		if (HFEngine::ECS.SearchComponent<Collider>(GetGameObject()))
		{
			auto& collider = HFEngine::ECS.GetComponent<Collider>(GetGameObject());
			if (collider.type == Collider::ColliderTypes::TRIGGER)
			{
				collider.OnTriggerEnter = TriggerMethodPointer(TriggerTest::OnTriggerEnter);
				collider.OnTriggerExit = TriggerMethodPointer(TriggerTest::OnTriggerExit);
			}
			else
			{
				LogWarning("TriggerTestScript::Awake(): collider has other type than TRIGGER!");
			}
		}
		else
		{
			LogWarning("TriggerTestScript::Awake(): gameObject has no collider!");
		}
	}

	void OnTriggerEnter(GameObject other)
	{
		LogInfo("TriggerTestScript:: OnTriggerEnter()");
	}

	void OnTriggerExit(GameObject other)
	{
		LogInfo("TriggerTestScript:: OnTriggerExit()");
	}
};
