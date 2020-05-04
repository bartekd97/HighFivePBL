#pragma once

#include <glm/glm.hpp>
#include "Resourcing/Shader.h"

class DirectionalLight
{
public:
	glm::vec3 direction = { 0.0f, -1.0f, 0.75f };
	glm::vec3 color = { 0.9f, 0.9f, 0.9f };
	glm::vec3 ambient = { 0.05f, 0.05f, 0.05f };

	inline void Apply(std::shared_ptr<Shader> shader)
	{
		shader->setVector3F("gDirectionalLight.Direction", direction);
		shader->setVector3F("gDirectionalLight.Color", color);
		shader->setVector3F("gDirectionalLight.Ambient", ambient);
	}
};