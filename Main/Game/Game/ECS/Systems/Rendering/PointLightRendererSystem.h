#pragma once

#include "ECS/System.h"
#include "Resourcing/Shader.h"
#include "Rendering/Camera.h"
#include "CulledRendererSystem.h"
#include "ECS/Components/PointLightRenderer.h"

class Mesh;

// require PointLightRenderer component
class PointLightRendererSystem : public CulledRendererSystem<PointLightRenderer, 1>
{
private:
	std::shared_ptr<Shader> pointLightShader;
	std::shared_ptr<Mesh> sphereMesh;

public:
	inline virtual const AABBStruct& GetLocalAABB(PointLightRenderer& component)
	{
		component.localAABB.min = glm::vec3(-component.light.radius);
		component.localAABB.max = glm::vec3(component.light.radius);
		return component.localAABB;
	};

	void Init() override;
	unsigned int Render(Camera& viewCamera, glm::vec2 viewportSize);
};