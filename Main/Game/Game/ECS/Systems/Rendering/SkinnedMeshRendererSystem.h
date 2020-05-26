#pragma once

#include <vector>
#include "ECS/Components/SkinnedMeshRenderer.h"
#include "CulledRendererSystem.h"
#include "Resourcing/Shader.h"

// require SkinnedMeshRenderer component
class SkinnedMeshRendererSystem : public CulledRendererSystem<SkinnedMeshRenderer, 1>
{
private:
	std::shared_ptr<Shader> toGBufferShader;
	std::shared_ptr<Shader> toShadowmapShader;
	std::shared_ptr<Shader> forwardShader;

	std::vector<SkinnedMeshRenderer*> delayedForward;

public:
	inline virtual const AABBStruct& GetLocalAABB(SkinnedMeshRenderer& component)
	{
		// TWEAK
		// potential exceeding AABB due to skinning animation
		// so increase original AABB to secure it a bit
		component.extendedLocalAABB.min = component.mesh->AABB.min * 2.0f;
		component.extendedLocalAABB.max = component.mesh->AABB.max * 2.0f;
		return component.extendedLocalAABB;
	};

	void Init() override;
	void RenderToShadowmap(Camera& lightCamera);
	void RenderToGBuffer(Camera& viewCamera, Camera& lightCamera, std::shared_ptr<Texture> shadowmap);
	void RenderForward(Camera& viewCamera, DirectionalLight& dirLight);
};
