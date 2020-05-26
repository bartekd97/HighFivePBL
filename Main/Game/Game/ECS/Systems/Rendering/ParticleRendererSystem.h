#pragma once

#include "ECS/Components/ParticleRenderer.h"
#include "ECS/System.h"
#include "Resourcing/Shader.h"
#include "Rendering/Camera.h"
#include "CulledRendererSystem.h"

// require ParticleContainer component
// require ParticleRenderer component
class ParticleRendererSystem : public CulledRendererSystem<ParticleRenderer, 1>
{
private:
	std::shared_ptr<Shader> particleShader;
	GLuint VBO;
	GLuint VAO;

public:
	inline virtual const AABBStruct& GetLocalAABB(ParticleRenderer& component)
	{
		component.localAABB.min = glm::vec3(-component.visibilityRadius);
		component.localAABB.max = glm::vec3(component.visibilityRadius);
		return component.localAABB;
	};

	void Init() override;
	void Render(Camera& viewCamera);
};