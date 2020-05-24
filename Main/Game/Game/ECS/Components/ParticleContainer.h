#pragma once

#include <vector>
#include <glm/glm.hpp>

// struct definition for uniform buffer std140 layout (must be padded to vec4 size)
struct Particle
{
	// gets packed into vec4 in uniform buffer
	glm::vec3 position;
	float scale;

	// gets packed into vec4 in uniform buffer
	glm::vec3 direction;
	float velocity;

	// gets packed into vec4 in uniform buffer
	float currentLifetime = 0.0f;
	float maxLifetime = 0.0f;
	glm::vec2 __padding;

	bool isExpired() { return currentLifetime >= maxLifetime; }
};


struct ParticleContainer
{
	static const int MAX_ALLOWED_PARTICLES = 256;

	std::vector<Particle> particles;
	
	void setMaxParticles(int limit)
	{
		assert(limit <= MAX_ALLOWED_PARTICLES && "Out of range");
		particles.resize(limit);
	}

	int _nextFreeIndexCheck = 0;
	Particle* getFreeParticle()
	{
		for (; _nextFreeIndexCheck < particles.size(); _nextFreeIndexCheck++)
			if (particles[_nextFreeIndexCheck].isExpired())
				return &particles[_nextFreeIndexCheck++];

		int tmp = _nextFreeIndexCheck;
		_nextFreeIndexCheck = 0;

		for (; _nextFreeIndexCheck < tmp; _nextFreeIndexCheck++)
			if (particles[_nextFreeIndexCheck].isExpired())
				return &particles[_nextFreeIndexCheck++];

		return NULL;
	}
};