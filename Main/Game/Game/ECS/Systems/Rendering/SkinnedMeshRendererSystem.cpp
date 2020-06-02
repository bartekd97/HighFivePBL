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
	renderer.business.Wait();

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

	forwardShader = ShaderManager::GetShader("ForwardRenderSkinned");
	forwardShader->use();
	forwardShader->bindUniformBlockPoint("gBonesBuffer", BONE_MATRICES_BUFFER_BINING_POINT);
	MaterialBindingPoint::AssignToShader(forwardShader);
}

unsigned int SkinnedMeshRendererSystem::RenderToShadowmap(Camera& lightCamera)
{
	unsigned int rendered = 0;

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
		if (!renderer.cullingData.visibleByLightCamera || !renderer.castShadows)
			continue;

		toShadowmapShader->setMat4("gModel", renderer.cullingData.worldTransform);
		CheckBoneMatricesBuffer(renderer);
		renderer.boneMatricesBuffer->bind(BONE_MATRICES_BUFFER_BINING_POINT);

		renderer.mesh->bind();
		renderer.mesh->draw();
		rendered++;
	}
	glDisable(GL_CULL_FACE);

	return rendered;
}

unsigned int SkinnedMeshRendererSystem::RenderToGBuffer(Camera& viewCamera, Camera& lightCamera, std::shared_ptr<Texture> shadowmap)
{
	unsigned int rendered = 0;

	toGBufferShader->use();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	viewCamera.Use(toGBufferShader);
	glm::mat4 lightViewProjection = lightCamera.GetProjectionMatrix() * lightCamera.GetViewMatrix();
	toGBufferShader->setMat4("gLightViewProjection", lightViewProjection);
	shadowmap->bind(0);

	delayedForward.clear();
	auto currentFrame = HFEngine::CURRENT_FRAME_NUMBER;
	auto renderers = HFEngine::ECS.GetAllComponents<SkinnedMeshRenderer>();
	for (auto& renderer : renderers)
	{
		if (renderer.cullingData.lastUpdate != currentFrame)
			continue;
		if (!renderer.cullingData.visibleByViewCamera)
			continue;

		if (renderer.material->type == MaterialType::FORWARD) {
			delayedForward.emplace_back(&renderer);
			continue;
		}

		toGBufferShader->setMat4("gModel", renderer.cullingData.worldTransform);
		CheckBoneMatricesBuffer(renderer);
		renderer.boneMatricesBuffer->bind(BONE_MATRICES_BUFFER_BINING_POINT);

		renderer.material->apply(toGBufferShader);
		renderer.mesh->bind();
		renderer.mesh->draw();
		rendered++;
	}
	Mesh::NoBind();
	Material::NoApply(toGBufferShader);

	glDisable(GL_CULL_FACE);

	return rendered;
}

unsigned int SkinnedMeshRendererSystem::RenderForward(Camera& viewCamera, DirectionalLight& dirLight)
{
	unsigned int rendered = 0;

	if (delayedForward.empty()) return rendered;

	forwardShader->use();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	viewCamera.Use(forwardShader);
	dirLight.Apply(forwardShader);
	do {
		SkinnedMeshRenderer* renderer = delayedForward.back();

		forwardShader->setMat4("gModel", renderer->cullingData.worldTransform);
		CheckBoneMatricesBuffer(*renderer);
		renderer->boneMatricesBuffer->bind(BONE_MATRICES_BUFFER_BINING_POINT);

		renderer->material->apply(forwardShader);
		renderer->mesh->bind();
		renderer->mesh->draw();
		rendered++;

		delayedForward.pop_back();
	} while (!delayedForward.empty());
	Mesh::NoBind();
	Material::NoApply(forwardShader);

	glDisable(GL_CULL_FACE);

	return rendered;
}
