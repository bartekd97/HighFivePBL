#pragma once

#include <glm/glm.hpp>
#include "../ECS/ECSTypes.h"

struct RaycastHit
{
	RaycastHit()
	{
		hittedObject = NULL_GAMEOBJECT;
		hitPosition = glm::vec3(0.0f);
		distance = -1.0f;
	}

	GameObject hittedObject;
	float distance;
	glm::vec3 hitPosition;
	std::vector<GameObject> triggersHitted;
};
