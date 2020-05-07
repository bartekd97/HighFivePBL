#pragma once

#include "ECS/Components/MeshRenderer.h"
#include "CulledRendererSystem.h"
#include "Resourcing/Shader.h"

// require MeshRenderer component
class MeshRendererSystem : public CulledRendererSystem<MeshRenderer, 4>
{
private:
	std::shared_ptr<Shader> toGBufferShader;

public:
	inline virtual const AABBStruct& GetLocalAABB(MeshRenderer& component) { return component.mesh->AABB; };

	void Init() override;
	void RenderToGBuffer();
};
