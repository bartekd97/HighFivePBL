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
		if (!controller->GetIsFlying()) controller->SetSlow(controller->GetSlow() + slowForce);
	}

	void OnObstacleExit(GameObject object) override
	{

		auto& controller = controllers[object];
		if (!controller->GetIsFlying()) controller->SetSlow(controller->GetSlow() - slowForce);
	}
};
