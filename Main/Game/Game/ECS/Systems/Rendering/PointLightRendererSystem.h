#pragma once

#include "ECS/System.h"
#include "Resourcing/Shader.h"
#include "Rendering/Camera.h"

class Mesh;

// require PointLightRenderer component
class PointLightRendererSystem : public System
{
private:
	std::shared_ptr<Shader> pointLightShader;
	std::shared_ptr<Mesh> sphereMesh;

public:
	void Init() override;
	void Render(Camera& viewCamera, glm::vec2 viewportSize);
};