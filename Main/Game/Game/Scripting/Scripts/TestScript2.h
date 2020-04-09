#pragma once

#include "../Script.h"
#include "../../HFEngine.h"

class TestScript2 : public Script
{
public:
	void Awake()
	{
		LogInfo("Test script2 Awake()");
	}

	void Update(float dt)
	{
		auto& transform = HFEngine::ECS.GetComponent<Transform>(GetGameObject());
		transform.RotateSelf(dt * 3.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	}
};
