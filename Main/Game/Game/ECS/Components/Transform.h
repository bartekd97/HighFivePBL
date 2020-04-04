#pragma once

#include <glm/glm.hpp>

struct Transform
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	glm::vec3 worldPosition;
	glm::vec3 worldRotation;
	glm::vec3 worldScale;

	bool dirty = false;
};
