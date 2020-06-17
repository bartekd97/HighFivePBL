#pragma once

#include "ObstacleController.h"

class MudController : public ObstacleController
{
private: // PARAMETERS
	float slowForce = 5.0f;
private: // VARIABLES

public:
	MudController()
	{
		RegisterFloatParameter("slowForce", &slowForce);
	}

	void OnObstacleEnter(GameObject object) override
	{
		auto& controller = controllers[object];
		if (!controller->GetIsFlying()) {
			controller->SetSlow(controller->GetSlow() + slowForce);
		}

		auto& scriptContainer = HFEngine::ECS.GetComponent<ScriptContainer>(object);
		if (controllers[object] == scriptContainer.GetScript<PlayerController>())
		{
			AudioManager::StopMovement();
			AudioManager::SetMovementSound("mud", 0.5f);
			scriptContainer.GetScript<PlayerController>()->SetIsReadyToStartMovementTrue();
		}
	}

	void OnObstacleExit(GameObject object) override
	{

		auto& controller = controllers[object];
		if (!controller->GetIsFlying()) {
			controller->SetSlow(controller->GetSlow() - slowForce);
		}

		auto& scriptContainer = HFEngine::ECS.GetComponent<ScriptContainer>(object);
		if (controllers[object] == scriptContainer.GetScript<PlayerController>())
		{
			AudioManager::StopMovement();
			AudioManager::SetMovementSound("footsteps_in_grass", 0.1f);
			scriptContainer.GetScript<PlayerController>()->SetIsReadyToStartMovementTrue();
		}
	}
};
