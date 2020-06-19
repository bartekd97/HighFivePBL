#pragma once

#include "../Script.h"
#include "HFEngine.h"
#include "ECS/Components/Transform.h"
#include "InputManager.h"

class PlayerCameraFollower : public Script
{
private:
	bool hasGhostMovement = false;
	glm::vec3 currentCameraTarget;
	float cameraSmoothing = 0.75f;
	GameObject mortalObject;
	GameObject ghostObject;

public:
	bool Paused = false;

	PlayerCameraFollower()
	{
		RegisterVec3Parameter("cameraOffset", &cameraOffset);
		RegisterFloatParameter("yLookOffset", &yLookOffset);
	}

	void Awake()
	{
		mortalObject = GetGameObject();
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Ghost::MOVEMENT_START, PlayerCameraFollower::GhostMovementStart));
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Ghost::MOVEMENT_STOP, PlayerCameraFollower::GhostMovementStop));
	}

	void Start()
	{
		// TODO: load it from some external, global structure
		ghostObject = HFEngine::ECS.GetGameObjectByName("PlayerGhost").value();

		currentCameraTarget = GetCameraTargetPosition();
	}

	void GhostMovementStart(Event& event) { hasGhostMovement = true; }
	void GhostMovementStop(Event& event) { hasGhostMovement = false; }

	void LateUpdate(float dt)
	{
		glm::vec3 targetPosition = GetCameraTargetPosition();
		currentCameraTarget = glm::mix(targetPosition, currentCameraTarget, cameraSmoothing);

		glm::vec3 cameraPosition = GetCameraTargetSourcePosition(currentCameraTarget);

		if (Paused) return;

		HFEngine::MainCamera.SetView(cameraPosition, currentCameraTarget);

		// TODO: Remove it, its for testing only
		float scale = HFEngine::MainCamera.GetScale();
		if (InputManager::GetKeyStatus(GLFW_KEY_UP))
			scale *= 0.98f;
		else if (InputManager::GetKeyStatus(GLFW_KEY_DOWN))
			scale *= 1.02f;
		HFEngine::MainCamera.SetScale(scale);
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

	glm::vec3 GetCameraTargetSourcePosition(glm::vec3 targetPosition)
	{
		return targetPosition + cameraOffset;
	}


private:
	glm::vec3 cameraOffset = {0.0f, 25.0f, 15.0f};
	float yLookOffset = 0.0f;
};