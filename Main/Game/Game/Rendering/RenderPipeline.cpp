#include <glad/glad.h>
#include "RenderPipeline.h"
#include "HFEngine.h"
#include "../Utility/Logger.h"
#include "../ECS/Components.h"
#include "PrimitiveRenderer.h"

void RenderPipeline::InitGBuffer()
{
	const std::vector<FrameBuffer::ColorAttachement> gbufferComponents = {
		// internalFormat, dataFormat, dataType
		{GL_RGB16F, GL_RGB, GL_FLOAT},		// position
		{GL_RGB16F, GL_RGB, GL_FLOAT},		// normal
		{GL_RGB, GL_RGB, GL_UNSIGNED_BYTE},	// albedo
		{GL_RG, GL_RG, GL_UNSIGNED_BYTE},	// metalness roughness
		{GL_RGB, GL_RGB, GL_UNSIGNED_BYTE}	// emissive
	};

	GBuffer.frameBuffer = FrameBuffer::Create(
		HFEngine::RENDER_WIDTH, HFEngine::RENDER_HEIGHT,
		gbufferComponents,
		FrameBuffer::DepthAttachement::DEFAULT
		);

	auto gbufferTextures = GBuffer.frameBuffer->getColorAttachements();
	GBuffer.position = gbufferTextures[0];
	GBuffer.normal = gbufferTextures[1];
	GBuffer.albedo = gbufferTextures[2];
	GBuffer.metalnessRoughness = gbufferTextures[3];
	GBuffer.emissive = gbufferTextures[4];
}
void RenderPipeline::InitRenderSystems()
{
	RenderSystems.meshRenderer = HFEngine::ECS.RegisterSystem<MeshRendererSystem>();
	{
		Signature signature;
		signature.set(HFEngine::ECS.GetComponentType<Transform>());
		signature.set(HFEngine::ECS.GetComponentType<MeshRenderer>());
		HFEngine::ECS.SetSystemSignature<MeshRendererSystem>(signature);
	}
	RenderSystems.cubeRenderer = HFEngine::ECS.RegisterSystem<CubeRenderSystem>();
	{
		Signature signature;
		signature.set(HFEngine::ECS.GetComponentType<Transform>());
		signature.set(HFEngine::ECS.GetComponentType<CubeRenderer>());
		HFEngine::ECS.SetSystemSignature<CubeRenderSystem>(signature);
	}
	RenderSystems.boxColliderRenderer = HFEngine::ECS.RegisterSystem<BoxColliderRenderSystem>();
	{
		Signature signature;
		signature.set(HFEngine::ECS.GetComponentType<Transform>());
		signature.set(HFEngine::ECS.GetComponentType<BoxCollider>());
		HFEngine::ECS.SetSystemSignature<BoxColliderRenderSystem>(signature);
	}
	RenderSystems.circleColliderRenderer = HFEngine::ECS.RegisterSystem<CircleColliderRenderSystem>();
	{
		Signature signature;
		signature.set(HFEngine::ECS.GetComponentType<Transform>());
		signature.set(HFEngine::ECS.GetComponentType<CircleCollider>());
		HFEngine::ECS.SetSystemSignature<CircleColliderRenderSystem>(signature);
	}
}

void RenderPipeline::Init()
{
	if (initialized)
	{
		LogWarning("RenderPipeline::Init(): Already initialized");
		return;
	}

	InitGBuffer();
	InitRenderSystems();

	combineGBufferShader = ShaderManager::GetShader("CombineGBuffer");
	combineGBufferShader->use();
	combineGBufferShader->setInt("gPosition", (int)GBufferBindingPoint::POSITION);
	combineGBufferShader->setInt("gNormal", (int)GBufferBindingPoint::NORMAL);
	combineGBufferShader->setInt("gAlbedo", (int)GBufferBindingPoint::ALBEDO);
	combineGBufferShader->setInt("gMetalnessRoughness", (int)GBufferBindingPoint::METALNESS_ROUGHNESS);
	combineGBufferShader->setInt("gEmissive", (int)GBufferBindingPoint::EMISSIVE);

	initialized = true;
	LogInfo("RenderPipeline initialized.");
}


void RenderPipeline::Render()
{
	// draw to gbuffer
	GBuffer.frameBuffer->bind();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderSystems.meshRenderer->RenderToGBuffer();
	//RenderSystems.cubeRenderer->Render();

	// combine gbuffer
	FrameBuffer::BindDefaultScreen();
	combineGBufferShader->use();
	HFEngine::MainCamera.Use(combineGBufferShader); // for gCameraPosition
	HFEngine::WorldLight.Apply(combineGBufferShader); // for gDirectionalLight struct
	GBuffer.position->bind((int)GBufferBindingPoint::POSITION);
	GBuffer.normal->bind((int)GBufferBindingPoint::NORMAL);
	GBuffer.albedo->bind((int)GBufferBindingPoint::ALBEDO);
	GBuffer.metalnessRoughness->bind((int)GBufferBindingPoint::METALNESS_ROUGHNESS);
	GBuffer.emissive->bind((int)GBufferBindingPoint::EMISSIVE);
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	PrimitiveRenderer::DrawScreenQuad();

	RenderSystems.boxColliderRenderer->Render();
	RenderSystems.circleColliderRenderer->Render();
}
