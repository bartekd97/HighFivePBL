#pragma once

#include <glad/glad.h>

#include "../../../Resourcing/ShaderManager.h"
#include "../../SystemRender.h"

// TEST SYSTEM
class CircleColliderRenderSystem : public SystemRender
{
public:
	void Init() override;
	void Render() override;
private:
	std::shared_ptr<Shader> shader;
	GLuint vao{};
	GLuint vboVertices{};
	int size;
};
