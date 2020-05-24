#pragma once

#include <memory>
#include <glm/glm.hpp>

class UniformBuffer;
struct ParticleRenderer
{
	float opacity = 1.0f;
	glm::vec3 color = {1.0f, 1.0f, 1.0f};
	std::shared_ptr<UniformBuffer> particlesBuffer;
};