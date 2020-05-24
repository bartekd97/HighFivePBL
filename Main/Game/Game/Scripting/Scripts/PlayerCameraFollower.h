#pragma once

#include "../Script.h"
#include "HFEngine.h"
#include "ECS/Components/Transform.h"

class PlayerCameraFollower : public Script
{
private:
	bool hasGhostMovement = false;
	glm::vec3 currentCameraTarget;
	float cameraSmoothing = 0.75f;
	GameObject mortalObject;
	GameObject ghostObject;

public:
	PlayerCameraFollower()
	{
		RegisterVec3Parameter("cameraOffset", &cameraOffset);
		RegisterFloatParameter("yLookOffset", &yLookOffset);
	}

	void Awake()
	{
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Ghost::MOVEMENT_START, PlayerCameraFollower::GhostMovementStart));
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Ghost::MOVEMENT_STOP, PlayerCameraFollower::GhostMovementStop));
	}

	void Start()
	{
		mortalObject = GetGameObject();
		ghostObject = HFEngine::ECS.GetByNameInChildren(GetGameObject(), "Ghost")[0];

		currentCameraTarget = GetCameraTargetPosition();
	}

	void GhostMovementStart(Event& event) { hasGhostMovement = true; }
	void GhostMovementStop(Event& event) { hasGhostMovement = false; }

	void LateUpdate(float dt)
	{
		glm::vec3 targetPosition = GetCameraTargetPosition();
		currentCameraTarget = glm::mix(targetPosition, currentCameraTarget, cameraSmoothing);

		glm::vec3 cameraPosition = currentCameraTarget + cameraOffset;
		HFEngine::MainCamera.SetView(cameraPosition, currentCameraTarget);
	}

	glm::vec3 GetCameraTargetPosition()
	{
		auto& transformMortal = HFEngine::ECS.GetComponent<Transform>(mortalObject);
		glm::vec3 targetPosition;
		if (hasGhostMovement)
		{
			auto& transformGhost = HFEngine::ECS.GetComponent<Transform>(ghostObject);
			targetPosition = (transformMortal.GetWorldPosition() + transformGhost.GetWorldPosition()) / 2.0f;
			targetPosition.y = yLookOffset;
		}
		else
		{
			targetPosition = transformMortal.GetWorldPosition();
			targetPosition.y = yLookOffset;
		}
		return targetPosition;
	}


private:
	glm::vec3 cameraOffset = {0.0f, 25.0f, 15.0f};
	float yLookOffset = 0.0f;
};