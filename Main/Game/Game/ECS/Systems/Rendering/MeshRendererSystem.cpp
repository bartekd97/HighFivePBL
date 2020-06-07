#include <memory>
#include <glm/gtc/matrix_access.hpp>
#include "HFEngine.h"
#include "MeshRendererSystem.h"
#include "Resourcing/Shader.h"
#include "Resourcing/Material.h"
#include "Resourcing/Mesh.h"

#include "ECS/Components/Transform.h"
#include "ECS/Components/MeshRenderer.h"

void MeshRendererSystem::Init()
{
	toShadowmapShader = ShaderManager::GetShader("ToShadowmap");

	toGBufferShader = ShaderManager::GetShader("ToGBuffer");
	toGBufferShader->use();
	toGBufferShader->setInt("shadowmap", 0);
	MaterialBindingPoint::AssignToShader(toGBufferShader);

	forwardShader = ShaderManager::GetShader("ForwardRender");
	forwardShader->use();
	MaterialBindingPoint::AssignToShader(forwardShader);
}

unsigned int MeshRendererSystem::RenderToShadowmap(Camera& lightCamera)
{
	unsigned int rendered = 0;

	toShadowmapShader->use();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	bool lastDoubleSided = false;

	lightCamera.Use(toShadowmapShader);
	auto currentFrame = HFEngine::CURRENT_FRAME_NUMBER;
	auto renderers = HFEngine::ECS.GetAllComponents<MeshRenderer>();
	for (auto const& renderer : renderers)
	{
		if (renderer.cullingData.lastUpdate != currentFrame)
			continue;
		if (!renderer.cullingData.visibleByLightCamera || !renderer.castShadows)
			continue;

		if (renderer.doubleSided != lastDoubleSided) {
			lastDoubleSided = renderer.doubleSided;
			lastDoubleSided ? glDisable(GL_CULL_FACE) : glEnable(GL_CULL_FACE);
		}

		toShadowmapShader->setMat4("gModel", renderer.cullingData.worldTransform);

		renderer.mesh->bind();
		renderer.mesh->draw();
		rendered++;
	}
	Mesh::NoBind();
	glDisable(GL_CULL_FACE);

	return rendered;
}

unsigned int MeshRendererSystem::RenderToGBuffer(Camera& viewCamera, Camera& lightCamera, std::shared_ptr<Texture> shadowmap)
{
	unsigned int rendered = 0;

	toGBufferShader->use();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	bool lastDoubleSided = false;

	viewCamera.Use(toGBufferShader);
	glm::mat4 lightViewProjection = lightCamera.GetProjectionMatrix() * lightCamera.GetViewMatrix();
	toGBufferShader->setMat4("gLightViewProjection", lightViewProjection);
	shadowmap->bind(0);

	auto currentFrame = HFEngine::CURRENT_FRAME_NUMBER;
	delayedForward.clear();
	auto renderers = HFEngine::ECS.GetAllComponents<MeshRenderer>();
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

		if (renderer.doubleSided != lastDoubleSided) {
			lastDoubleSided = renderer.doubleSided;
			lastDoubleSided ? glDisable(GL_CULL_FACE) : glEnable(GL_CULL_FACE);
		}
		
		toGBufferShader->setMat4("gModel", renderer.cullingData.worldTransform);
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

unsigned int MeshRendererSystem::RenderForward(Camera& viewCamera, DirectionalLight& dirLight)
{
	unsigned int rendered = 0;
	if (delayedForward.empty()) return rendered;

	forwardShader->use();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	bool lastDoubleSided = false;

	viewCamera.Use(forwardShader);
	dirLight.Apply(forwardShader, viewCamera);
	do {
		MeshRenderer* renderer = delayedForward.back();

		if (renderer->doubleSided != lastDoubleSided) {
			lastDoubleSided = renderer->doubleSided;
			lastDoubleSided ? glDisable(GL_CULL_FACE) : glEnable(GL_CULL_FACE);
		}

		forwardShader->setMat4("gModel", renderer->cullingData.worldTransform);
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
