#pragma once

#include "ECS/System.h"
#include "Resourcing/Shader.h"
#include "Rendering/Camera.h"


// require Container component
// require ParticleRenderer component
class ParticleRendererSystem : public System
{
private:
	std::shared_ptr<Shader> particleShader;
	GLuint VBO;
	GLuint VAO;

public:
	void Init() override;
	void Render(Camera& viewCamera);
};