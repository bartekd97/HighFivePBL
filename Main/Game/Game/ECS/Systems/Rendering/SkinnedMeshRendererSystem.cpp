#include "HFEngine.h"
#include "SkinnedMeshRendererSystem.h"
#include "Resourcing/Material.h"
#include "Resourcing/Mesh.h"

#include "ECS/Components/Transform.h"
#include "ECS/Components/SkinnedMeshRenderer.h"

void SkinnedMeshRendererSystem::Init()
{
	toShadowmapShader = ShaderManager::GetShader("ToShadowmapSkinned");
	toGBufferShader = ShaderManager::GetShader("ToGBufferSkinned");
	toGBufferShader->use();
	toGBufferShader->setInt("shadowmap", 0);
	MaterialBindingPoint::AssignToShader(toGBufferShader);
}

void SkinnedMeshRendererSystem::RenderToShadowmap(Camera& lightCamera)
{
	toShadowmapShader->use();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	lightCamera.Use(toShadowmapShader);
	auto currentFrame = HFEngine::CURRENT_FRAME_NUMBER;
	auto renderers = HFEngine::ECS.GetAllComponents<SkinnedMeshRenderer>();
	for (auto const& renderer : renderers)
	{
		if (renderer.cullingData.lastUpdate != currentFrame)
			continue;
		if (!renderer.cullingData.visibleByLightCamera)
			continue;

		toShadowmapShader->setMat4("gModel", renderer.cullingData.worldTransform);
		for (int i = 0; i < SkinnedMeshRenderer::MAX_BONES; i++)
		{
			auto mat = renderer.boneMatrices[i];
			toShadowmapShader->setMat4(("gBones[" + std::to_string(i) + "]").c_str(), mat);
		}

		renderer.mesh->bind();
		renderer.mesh->draw();
	}
	glDisable(GL_CULL_FACE);
}

void SkinnedMeshRendererSystem::RenderToGBuffer(Camera& viewCamera, Camera& lightCamera, std::shared_ptr<Texture> shadowmap)
{
	toGBufferShader->use();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	viewCamera.Use(toGBufferShader);
	glm::mat4 lightViewProjection = lightCamera.GetProjectionMatrix() * lightCamera.GetViewMatrix();
	toGBufferShader->setMat4("gLightViewProjection", lightViewProjection);
	shadowmap->bind(0);

	auto currentFrame = HFEngine::CURRENT_FRAME_NUMBER;
	auto renderers = HFEngine::ECS.GetAllComponents<SkinnedMeshRenderer>();
	for (auto const& renderer : renderers)
	{
		if (renderer.cullingData.lastUpdate != currentFrame)
			continue;
		if (!renderer.cullingData.visibleByViewCamera)
			continue;

		toGBufferShader->setMat4("gModel", renderer.cullingData.worldTransform);
		for (int i = 0; i < SkinnedMeshRenderer::MAX_BONES; i++)
		{
			auto mat = renderer.boneMatrices[i];
			toGBufferShader->setMat4(("gBones[" + std::to_string(i) + "]").c_str(), mat);
		}

		renderer.material->apply(toGBufferShader);
		renderer.mesh->bind();
		renderer.mesh->draw();
	}
	Mesh::NoBind();
	Material::NoApply(toGBufferShader);

	glDisable(GL_CULL_FACE);
}
