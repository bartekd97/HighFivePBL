#pragma once

#include "../Script.h"
#include "../../HFEngine.h"

class TestScript : public Script
{
public:
	void Awake()
	{
		LogInfo("Test script Awake()");
		//EventManager::AddScriptListener(Events::Test::TICK, GetGameObject(), [](Event& ev) { LogInfo("TestScript TICK event test");  });
	}

	void Start()
	{
		LogInfo("Test script Start()");
	}

	void Update(float dt)
	{
		auto& transform = HFEngine::ECS.GetComponent<Transform>(GetGameObject());
		transform.RotateSelf(dt * 5.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	}
};
