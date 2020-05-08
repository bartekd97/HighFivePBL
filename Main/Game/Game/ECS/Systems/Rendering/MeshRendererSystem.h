#pragma once

#include "ECS/Components/MeshRenderer.h"
#include "CulledRendererSystem.h"
#include "Resourcing/Shader.h"
#include "Rendering/Camera.h"

// require MeshRenderer component
class MeshRendererSystem : public CulledRendererSystem<MeshRenderer, 4>
{
private:
	std::shared_ptr<Shader> toGBufferShader;
	std::shared_ptr<Shader> toShadowmapShader;

public:
	inline virtual const AABBStruct& GetLocalAABB(MeshRenderer& component) { return component.mesh->AABB; };

	void Init() override;
	void RenderToShadowmap(Camera& lightCamera);
	void RenderToGBuffer(Camera& viewCamera, Camera& lightCamera, std::shared_ptr<Texture> shadowmap);
};
