#pragma once

#include "../Script.h"
#include "HFEngine.h"

class RotateAround : public Script
{
private:
	float speed = 90.0f;

public:
	RotateAround()
	{
		RegisterFloatParameter("speed", &speed);
	}

	void LateUpdate(float dt)
	{
		auto& transform = HFEngine::ECS.GetComponent<Transform>(GetGameObject());
		transform.RotateSelf(speed * dt, transform.GetUp());
	}
};
