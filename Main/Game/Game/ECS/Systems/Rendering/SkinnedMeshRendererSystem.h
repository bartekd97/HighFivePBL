#pragma once

#include "ECS/System.h"
#include "Resourcing/Shader.h"

// require SkinnedMeshRenderer component
class SkinnedMeshRendererSystem : public System
{
private:
	std::shared_ptr<Shader> toGBufferShader;

public:
	void Init() override;
	void RenderToGBuffer();
};
