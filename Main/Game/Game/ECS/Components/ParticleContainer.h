#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "ECS/ECSTypes.h"
#include "Utility/ThreadBusiness.h"

// struct definition for uniform buffer std140 layout (must be padded to vec4 size)
struct Particle
{
	// gets packed into vec4 in uniform buffer
	glm::vec3 position;
	float size;

	// gets packed into vec4 in uniform buffer
	glm::vec3 direction;
	float velocity;

	// gets packed into vec4 in uniform buffer
	float currentLifetime = 0.0f;
	float maxLifetime = 0.0f;
	glm::vec2 __padding;

	bool IsExpired() { return currentLifetime >= maxLifetime; }
};


struct ParticleContainer
{
	static const int MAX_ALLOWED_PARTICLES = 256;

	std::vector<Particle> particles;
	FrameCounter lastUpdate = 0;
	
	void SetMaxParticles(int limit)
	{
		assert(limit >= 0 && limit <= MAX_ALLOWED_PARTICLES && "Out of range");
		particles.resize(limit);
	}

	int _nextFreeIndexCheck = 0;
	Particle* GetFreeParticle()
	{
		for (; _nextFreeIndexCheck < particles.size(); _nextFreeIndexCheck++)
			if (particles[_nextFreeIndexCheck].IsExpired())
				return &particles[_nextFreeIndexCheck++];

		int tmp = _nextFreeIndexCheck;
		_nextFreeIndexCheck = 0;

		for (; _nextFreeIndexCheck < tmp; _nextFreeIndexCheck++)
			if (particles[_nextFreeIndexCheck].IsExpired())
				return &particles[_nextFreeIndexCheck++];

		return NULL;
	}

	ThreadBusiness business;
};