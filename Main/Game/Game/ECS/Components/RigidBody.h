#pragma once

#include <glm/glm.hpp>

struct RigidBody
{
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 movePosition;
	bool moved = false;

	inline void Move(glm::vec3 position)
	{
		movePosition = position;
		moved = true;
	}
};
