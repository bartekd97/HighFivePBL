#pragma once

#include <vector>
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
	std::shared_ptr<Shader> forwardShader;

	std::vector<MeshRenderer*> delayedForward;

public:
	inline virtual const AABBStruct& GetLocalAABB(MeshRenderer& component) { return component.mesh->AABB; };

	void Init() override;
	unsigned int RenderToShadowmap(Camera& lightCamera);
	unsigned int RenderToGBuffer(Camera& viewCamera, Camera& lightCamera, std::shared_ptr<Texture> shadowmap);
	unsigned int RenderForward(Camera& viewCamera, DirectionalLight& dirLight);
};
