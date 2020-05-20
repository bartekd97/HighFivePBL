#pragma once

#include <glm/glm.hpp>
#include "Resourcing/Shader.h"

class DirectionalLight
{
public:
	glm::vec3 direction = { 0.5f, -1.0f, 0.75f };
	glm::vec3 color = { 0.04f, 0.06f, 0.1f };
	glm::vec3 ambient = { 0.005f, 0.01f, 0.02f };
	float shadowIntensity = 1.0f;

	inline void Apply(std::shared_ptr<Shader> shader)
	{
		shader->setVector3F("gDirectionalLight.Direction", direction);
		shader->setVector3F("gDirectionalLight.Color", color);
		shader->setVector3F("gDirectionalLight.Ambient", ambient);
		shader->setFloat("gDirectionalLight.ShadowIntensity", shadowIntensity);
	}
};

class PointLight
{
public:
	glm::vec3 position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 color = { 0.9f, 0.6f, 0.3f };
	float radius = 5.0f;
	float intensity = 1.0f;
	float shadowIntensity = 0.0f;

	inline void Apply(std::shared_ptr<Shader> shader)
	{
		shader->setVector3F("gPointLight.Position", position);
		shader->setVector3F("gPointLight.Color", color);
		shader->setFloat("gPointLight.Radius", radius);
		shader->setFloat("gPointLight.Intensity", intensity);
		shader->setFloat("gPointLight.ShadowIntensity", shadowIntensity);
	}
};