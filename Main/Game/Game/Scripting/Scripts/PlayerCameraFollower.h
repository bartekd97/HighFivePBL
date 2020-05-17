#pragma once

#include "../Script.h"
#include "HFEngine.h"
#include "ECS/Components/Transform.h"

class PlayerCameraFollower : public Script
{
public:
	PlayerCameraFollower()
	{
		RegisterVec3Parameter("cameraOffset", &cameraOffset);
		RegisterFloatParameter("yLookOffset", &yLookOffset);
	}

	void LateUpdate(float dt)
	{
		auto& transform = HFEngine::ECS.GetComponent<Transform>(GetGameObject());
		glm::vec3 targetPosition = transform.GetWorldPosition();
		targetPosition.y = yLookOffset;
		glm::vec3 cameraPosition = targetPosition + cameraOffset;

		HFEngine::MainCamera.SetView(cameraPosition, targetPosition);
	}


private:
	glm::vec3 cameraOffset = {0.0f, 25.0f, 15.0f};
	float yLookOffset = 0.0f;
};