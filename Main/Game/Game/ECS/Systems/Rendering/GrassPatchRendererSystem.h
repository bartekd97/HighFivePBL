#pragma once

#include "ECS/System.h"
#include "Resourcing/Shader.h"
#include "Rendering/Camera.h"
#include "CulledRendererSystem.h"
#include "ECS/Components/GrassPatchRenderer.h"

class Model;
class UniformBuffer;

// require GrassPatchRenderer component
class GrassPatchRendererSystem : public CulledRendererSystem<GrassPatchRenderer, 1>, public ISystemUpdate
{
private:
	std::shared_ptr<Shader> toGBufferShader;
	std::shared_ptr<Shader> toShadowmapShader;
	std::shared_ptr<UniformBuffer> randomBuffer;
	std::shared_ptr<Model> grassModel;
	std::shared_ptr<Texture> windNoise;

	float time = 0.0f;

public:
	inline virtual const AABBStruct& GetLocalAABB(GrassPatchRenderer& component)
	{
		glm::vec3 extend = glm::vec3(
			component.patchSize * 0.5f,
			5.0f,
			component.patchSize * 0.5f
		);

		component.localPatchAABB.min = -extend * 1.2f;
		component.localPatchAABB.max = extend * 1.2f;
		return component.localPatchAABB;
	};

	void Init() override;
	void Update(float dt) override;
	unsigned int RenderToShadowmap(Camera& lightCamera);
	unsigned int RenderToGBuffer(Camera& viewCamera, Camera& lightCamera, std::shared_ptr<Texture> shadowmap);
};