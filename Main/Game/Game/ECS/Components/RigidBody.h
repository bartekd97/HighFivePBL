#pragma once

#include <glm/glm.hpp>

struct RigidBody
{
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 movePosition;
	float mass;
	bool moved = false;
	bool isFalling = false;

	inline void Move(glm::vec3 position)
	{
		movePosition = position;
		moved = true;
	}
	
	void AddForce(glm::vec3 force)
	{
		velocity += force / mass;
	}
};
