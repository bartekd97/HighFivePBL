#pragma once

#include "../System.h"
#include "Shader.h"

// require MeshRenderer component
class MeshRendererSystem : public System
{
private:
	std::shared_ptr<Shader> toGBufferShader;

public:
	void Init() override;
	void RenderToGBuffer();
};

