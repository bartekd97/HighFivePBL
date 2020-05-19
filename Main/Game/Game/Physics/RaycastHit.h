#pragma once

#include <glm/glm.hpp>
#include "../ECS/ECSTypes.h"

struct RaycastHit
{
	GameObject hittedObject;
	float distance;
	glm::vec3 hitPosition;
};
