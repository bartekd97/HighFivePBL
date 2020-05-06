#pragma once

#include "ECS/Components/MeshRenderer.h"
#include "CulledRendererSystem.h"
#include "Resourcing/Shader.h"

// require MeshRenderer component
class MeshRendererSystem : public CulledRendererSystem<MeshRenderer>
{
private:
	std::shared_ptr<Shader> toGBufferShader;

public:
	void Init() override;
	void RenderToGBuffer();
};
