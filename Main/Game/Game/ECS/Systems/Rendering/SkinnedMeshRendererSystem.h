#pragma once

#include <stack>
#include "ECS/Components/SkinnedMeshRenderer.h"
#include "CulledRendererSystem.h"
#include "Resourcing/Shader.h"

struct SkinnedMeshRenderer;

// require SkinnedMeshRenderer component
class SkinnedMeshRendererSystem : public CulledRendererSystem<SkinnedMeshRenderer, 2>
{
private:
	std::shared_ptr<Shader> toGBufferShader;
	std::shared_ptr<Shader> toShadowmapShader;
	std::shared_ptr<Shader> forwardShader;

	std::stack<SkinnedMeshRenderer*> delayedForward;

public:
	inline virtual const AABBStruct& GetLocalAABB(SkinnedMeshRenderer& component)
	{
		// TWEAK
		// potential exceeding AABB due to skinning animation
		// so increase original AABB to secure it a bit
		component.extendedLocalAABB.min = component.mesh->AABB.min * 1.5f;
		component.extendedLocalAABB.max = component.mesh->AABB.max * 1.5f;
		return component.extendedLocalAABB;
	};

	void Init() override;
	void RenderToShadowmap(Camera& lightCamera);
	void RenderToGBuffer(Camera& viewCamera, Camera& lightCamera, std::shared_ptr<Texture> shadowmap);
	void RenderForward(Camera& viewCamera, DirectionalLight& dirLight);
};
