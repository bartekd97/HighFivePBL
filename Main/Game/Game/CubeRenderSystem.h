#pragma once

#include <glad/glad.h>

#include "ShaderManager.h"
#include "SystemRender.h"

class CubeRenderSystem : public SystemRender
{
public:
	void Init() override;
	void Render() override;
private:
	std::shared_ptr<Shader> shader;
	GLuint vao{};
	GLuint vboVertices{};
	GLuint vboNormals{};
};
