#pragma once

#include <glad/glad.h>

#include "../../../Resourcing/ShaderManager.h"
#include "../../SystemRender.h"

// TEST SYSTEM
class GravityColliderRenderSystem : public SystemRender
{
public:
	void Init() override;
	void Render() override;
private:
	std::shared_ptr<Shader> shader;
};
