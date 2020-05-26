#pragma once

#include <glm/glm.hpp>

struct ParticleEmitter
{
	enum class EmitterShape {
		CIRCLE,
		RECTANGLE
	};

	EmitterShape shape;
	glm::vec2 sourcShapeeSize; // x,y
	glm::vec2 targetShapeSize; // x,y

	glm::vec2 lifetime; // min,max
	glm::vec2 velocity; // min,max
	glm::vec2 size; // min,max

	float rate; // particles per second

	bool emitting = false;
	float timeLeftSinceEmit = 0.0f;

	int _delayedEmitCount = 0;
};