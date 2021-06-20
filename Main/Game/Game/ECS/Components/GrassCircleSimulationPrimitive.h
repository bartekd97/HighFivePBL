#pragma once

#include <glm/glm.hpp>

struct GrassCircleSimulationPrimitive
{
	enum class DirectionMode
	{
		SCRIPTED = 0,
		RADIAL = 1
	};

	float radius;
	float innerRadius;
	
	float ring = 0.0f;
	float innerRing = 0.0f;

	float targetHeight = 0.0f;

	DirectionMode directionMode = DirectionMode::SCRIPTED;

	glm::vec3 lastPosition;
	glm::vec2 lastDirection;
};