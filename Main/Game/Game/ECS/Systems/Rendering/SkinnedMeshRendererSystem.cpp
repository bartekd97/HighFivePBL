#include "HFEngine.h"
#include "SkinnedMeshRendererSystem.h"
#include "Resourcing/Material.h"
#include "Resourcing/Mesh.h"
#include "Resourcing/UniformBuffer.h"

#include "ECS/Components/Transform.h"
#include "ECS/Components/SkinnedMeshRenderer.h"

static const GLuint BONE_MATRICES_BUFFER_BINING_POINT = 1;

inline static void CheckBoneMatricesBuffer(SkinnedMeshRenderer& renderer)
{
	if (renderer.boneMatricesBuffer == nullptr)
	{
		renderer.boneMatricesBuffer = UniformBuffer::Create(sizeof(glm::mat4) * SkinnedMeshRenderer::MAX_BONES);
		if (!renderer.needMatricesBufferUpdate)
		{
			glm::mat4 identity(1.0f);
			renderer.boneMatrices.fill(identity);
			renderer.needMatricesBufferUpdate = true;
		}
	}
	if (renderer.needMatricesBufferUpdate)
	{
		renderer.boneMatricesBuffer->uploadData(
		(char*)renderer.boneMatrices.data(),
			sizeof(glm::mat4) * renderer.skinningData->NumBones()
			);
		renderer.needMatricesBufferUpdate = false;
	}
}

void SkinnedMeshRendererSystem::Init()
{
	toShadowmapShader = ShaderManager::GetShader("ToShadowmapSkinned");
	toShadowmapShader->bindUniformBlockPoint("gBonesBuffer", BONE_MATRICES_BUFFER_BINING_POINT);

	toGBufferShader = ShaderManager::GetShader("ToGBufferSkinned");
	toGBufferShader->use();
	toGBufferShader->setInt("shadowmap", 0);
	toGBufferShader->bindUniformBlockPoint("gBonesBuffer", BONE_MATRICES_BUFFER_BINING_POINT);

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
	for (auto& renderer : renderers)
	{
		if (renderer.cullingData.lastUpdate != currentFrame)
			continue;
		if (!renderer.cullingData.visibleByLightCamera)
			continue;

		toShadowmapShader->setMat4("gModel", renderer.cullingData.worldTransform);
		CheckBoneMatricesBuffer(renderer);
		renderer.boneMatricesBuffer->bind(BONE_MATRICES_BUFFER_BINING_POINT);
		/*
		for (int i = 0; i < SkinnedMeshRenderer::MAX_BONES; i++)
		{
			auto mat = renderer.boneMatrices[i];
			toShadowmapShader->setMat4(("gBones[" + std::to_string(i) + "]").c_str(), mat);
		}
		*/

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
	for (auto& renderer : renderers)
	{
		if (renderer.cullingData.lastUpdate != currentFrame)
			continue;
		if (!renderer.cullingData.visibleByViewCamera)
			continue;

		toGBufferShader->setMat4("gModel", renderer.cullingData.worldTransform);
		CheckBoneMatricesBuffer(renderer);
		renderer.boneMatricesBuffer->bind(BONE_MATRICES_BUFFER_BINING_POINT);
		/*
		for (int i = 0; i < SkinnedMeshRenderer::MAX_BONES; i++)
		{
			auto mat = renderer.boneMatrices[i];
			toGBufferShader->setMat4(("gBones[" + std::to_string(i) + "]").c_str(), mat);
		}
		*/

		renderer.material->apply(toGBufferShader);
		renderer.mesh->bind();
		renderer.mesh->draw();
	}
	Mesh::NoBind();
	Material::NoApply(toGBufferShader);

	glDisable(GL_CULL_FACE);
}
