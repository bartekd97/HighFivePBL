#include <memory>
#include <random>
#include <glm/gtc/matrix_access.hpp>
#include "HFEngine.h"
#include "GrassPatchRendererSystem.h"
#include "Resourcing/Shader.h"
#include "Resourcing/Mesh.h"
#include "Resourcing/UniformBuffer.h"
#include "Resourcing/Model.h"
#include "Resourcing/Material.h"
#include "InputManager.h"

#include "ECS/Components/Transform.h"
#include "ECS/Components/GrassPatchRenderer.h"

namespace {
	float RandomFloat(float min = 0.0f, float max = 1.0f)
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_real_distribution<float> dis(0.0f, 1.0f);

		return glm::mix(min, max, dis(gen));
	}

	const GLuint BUFFER_BINDING = 3;
	const GLuint DENSITY_TEXTURE_BINDING = 6;
	const GLuint SIMULATION_TEXTURE_BINDING = 7;
	const GLuint WIND_NOISE_BINDING = 8;
}

void GrassPatchRendererSystem::Init()
{
	toShadowmapShader = ShaderManager::GetShader("ToShadowmapGrass");
	toShadowmapShader->use();
	toShadowmapShader->setInt("grassDensityTexture", DENSITY_TEXTURE_BINDING);
	toShadowmapShader->setInt("simulationTexture", SIMULATION_TEXTURE_BINDING);
	toShadowmapShader->setInt("windNoise", WIND_NOISE_BINDING);
	toShadowmapShader->bindUniformBlockPoint("gRandomBuffer", BUFFER_BINDING);
	MaterialBindingPoint::AssignToShader(toShadowmapShader);

	toGBufferShader = ShaderManager::GetShader("ToGBufferGrass");
	toGBufferShader->use();
	toGBufferShader->setInt("shadowmap", 0);
	toGBufferShader->setInt("grassDensityTexture", DENSITY_TEXTURE_BINDING);
	toGBufferShader->setInt("simulationTexture", SIMULATION_TEXTURE_BINDING);
	toGBufferShader->setInt("windNoise", WIND_NOISE_BINDING);
	toGBufferShader->bindUniformBlockPoint("gRandomBuffer", BUFFER_BINDING);
	MaterialBindingPoint::AssignToShader(toGBufferShader);


	const int randomCount = 1024;
	std::vector<glm::vec4> buffer(randomCount);
	buffer.resize(randomCount);
	for (int i = 0; i < randomCount; i++)
	{
		buffer[i] = glm::vec4(
			RandomFloat(-1.0f, 1.0f),
			RandomFloat(-1.0f, 1.0f),
			RandomFloat(-1.0f, 1.0f),
			RandomFloat(0.0f, glm::pi<float>())
		);
	}
	randomBuffer = UniformBuffer::Create(sizeof(glm::vec4) * randomCount);
	randomBuffer->uploadData((char*)buffer.data());


	grassModel = ModelManager::GetModel("MapGenerator/Grass", "Grass1");

	windNoise = TextureManager::GetTexture("MapGenerator/Grass", "WindNoise");
	windNoise->setEdges(GL_REPEAT, GL_REPEAT);
}

void GrassPatchRendererSystem::Update(float dt)
{
	time += dt;
}

unsigned int GrassPatchRendererSystem::RenderToShadowmap(Camera& lightCamera)
{
	unsigned int rendered = 0;

	static bool enabled = true;
	if (InputManager::GetKeyDown(GLFW_KEY_F9))
		enabled = !enabled;
	if (!enabled)
		return false;

	toShadowmapShader->use();
	glDisable(GL_CULL_FACE);

	lightCamera.Use(toShadowmapShader);
	randomBuffer->bind(BUFFER_BINDING);

	grassModel->mesh->bind();
	grassModel->material->apply(toShadowmapShader);

	auto currentFrame = HFEngine::CURRENT_FRAME_NUMBER;
	auto renderers = HFEngine::ECS.GetAllComponents<GrassPatchRenderer>();
	for (auto& renderer : renderers)
	{
		if (renderer.cullingData.lastUpdate != currentFrame)
			continue;
		if (!renderer.cullingData.visibleByLightCamera)
			continue;

		auto& transform = HFEngine::ECS.GetComponent<Transform>(renderer.cullingData.targetGameObject);
		glm::vec3 worldPosition = transform.GetWorldPosition();

		float grassDistance = renderer.patchSize / (float)renderer.density;

		toShadowmapShader->setVector3F("worldPositionBase",
			worldPosition.x - renderer.patchSize * 0.5f,
			0.01f,
			worldPosition.z - renderer.patchSize * 0.5f
		);
		toShadowmapShader->setVector4F("worldBounds", renderer.worldBounds);
		toShadowmapShader->setFloat("grassDistance", grassDistance);
		toShadowmapShader->setFloat("grassScale", renderer.grassScale);
		toShadowmapShader->setInt("grassDensity", renderer.density);
		toShadowmapShader->setVector2F("randomFactor", grassDistance * 0.5f, 0.2f);
		toShadowmapShader->setInt("randomBufferOffset", glm::abs((int)(worldPosition.x * worldPosition.z)) % (1024 - (renderer.density * renderer.density)));

		renderer.densityTexture->bind(DENSITY_TEXTURE_BINDING);
		renderer.simulationTexture->getColorAttachement(0)->bind(SIMULATION_TEXTURE_BINDING);

		grassModel->mesh->drawInstanced(renderer.density * renderer.density);

		rendered++;
	}
	Mesh::NoBind();
	Material::NoApply(toShadowmapShader);
	Texture::NoBind(DENSITY_TEXTURE_BINDING, true);

	return rendered;
}

unsigned int GrassPatchRendererSystem::RenderToGBuffer(Camera& viewCamera, Camera& lightCamera, std::shared_ptr<Texture> shadowmap)
{
	unsigned int rendered = 0;

	static bool enabled = true;
	if (InputManager::GetKeyDown(GLFW_KEY_F9))
		enabled = !enabled;
	if (!enabled)
		return false;

	toGBufferShader->use();
	glDisable(GL_CULL_FACE);

	toGBufferShader->setFloat("time", time);

	viewCamera.Use(toGBufferShader);
	glm::mat4 lightViewProjection = lightCamera.GetProjectionMatrix() * lightCamera.GetViewMatrix();
	toGBufferShader->setMat4("gLightViewProjection", lightViewProjection);
	shadowmap->bind(0);
	randomBuffer->bind(BUFFER_BINDING);
	windNoise->bind(WIND_NOISE_BINDING);

	grassModel->mesh->bind();
	grassModel->material->apply(toGBufferShader);

	auto currentFrame = HFEngine::CURRENT_FRAME_NUMBER;
	auto renderers = HFEngine::ECS.GetAllComponents<GrassPatchRenderer>();
	for (auto& renderer : renderers)
	{
		if (renderer.cullingData.lastUpdate != currentFrame)
			continue;
		if (!renderer.cullingData.visibleByViewCamera)
			continue;

		auto& transform = HFEngine::ECS.GetComponent<Transform>(renderer.cullingData.targetGameObject);
		glm::vec3 worldPosition = transform.GetWorldPosition();

		float grassDistance = renderer.patchSize / (float)renderer.density;

		toGBufferShader->setVector3F("worldPositionBase",
			worldPosition.x - renderer.patchSize * 0.5f,
			0.01f,
			worldPosition.z - renderer.patchSize * 0.5f
		);
		toGBufferShader->setVector4F("worldBounds", renderer.worldBounds);
		toGBufferShader->setFloat("grassDistance", grassDistance);
		toGBufferShader->setFloat("grassScale", renderer.grassScale);
		toGBufferShader->setInt("grassDensity", renderer.density);
		toGBufferShader->setVector2F("randomFactor", grassDistance * 0.5f, 0.2f);
		toGBufferShader->setInt("randomBufferOffset", glm::abs((int)(worldPosition.x * worldPosition.z)) % (1024 - (renderer.density * renderer.density)));

		renderer.densityTexture->bind(DENSITY_TEXTURE_BINDING);
		renderer.simulationTexture->getColorAttachement(0)->bind(SIMULATION_TEXTURE_BINDING);

		grassModel->mesh->drawInstanced(renderer.density * renderer.density);

		rendered++;
	}
	Mesh::NoBind();
	Material::NoApply(toGBufferShader);
	Texture::NoBind(DENSITY_TEXTURE_BINDING, true);

	return rendered;
}