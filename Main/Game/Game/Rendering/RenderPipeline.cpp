#include <glad/glad.h>
#include "RenderPipeline.h"
#include "ECS/Systems/Rendering/MeshRendererSystem.h"
#include "ECS/Systems/Rendering/SkinnedMeshRendererSystem.h"
#include "ECS/Systems/Rendering/PointLightRendererSystem.h"
#include "ECS/Systems/Rendering/ParticleRendererSystem.h"
#include "ECS/Systems/Rendering/BoxColliderRenderSystem.h"
#include "ECS/Systems/Rendering/CircleColliderRenderSystem.h"
#include "HFEngine.h"
#include "../Utility/Logger.h"
#include "../ECS/Components.h"
#include "PrimitiveRenderer.h"
#include "InputManager.h"

#include "Postprocessing/RiverFogEffect.h"


namespace {
	void CalculateLightCamera(Camera& viewCamera, Camera& lightCamera)
	{
		glm::vec3 mainCameraPosition = viewCamera.GetPosition();
		glm::vec3 mainCameraDirection = viewCamera.GetViewDiorection();

		float stepsToZero = -(mainCameraPosition.y / mainCameraDirection.y);
		glm::vec3 zeroPos = mainCameraPosition + (mainCameraDirection * stepsToZero);

		glm::vec3 lightDirection = glm::normalize(HFEngine::WorldLight.direction);
		glm::vec3 lightPosition = zeroPos - (lightDirection * stepsToZero);

		lightCamera.SetMode(Camera::ORTHOGRAPHIC);
		lightCamera.SetClipPlane(viewCamera.GetClipPlane());
		glm::vec2 camSize = viewCamera.GetSize();
		float camSizeMax = glm::max(camSize.x, camSize.y);
		lightCamera.SetSize(camSizeMax, camSizeMax);
		lightCamera.SetScale(viewCamera.GetScale() * 1.25f);
		lightCamera.SetView(lightPosition, lightPosition + lightDirection);
	}
}



void RenderPipeline::InitGBuffer()
{
	const std::vector<FrameBuffer::ColorAttachement> gbufferComponents = {
		// internalFormat, dataFormat, dataType
		{GL_RGB16F, GL_RGB, GL_FLOAT},		// position
		{GL_RGB16F, GL_RGB, GL_FLOAT},		// normal
		{GL_RGB, GL_RGB, GL_UNSIGNED_BYTE},	// albedo
		{GL_RGB, GL_RGB, GL_UNSIGNED_BYTE},	// metalness roughness shadow
		{GL_RGB16F, GL_RGB, GL_FLOAT}	// emissive
	};

	GBuffer.frameBuffer = FrameBuffer::Create(
		HFEngine::RENDER_WIDTH, HFEngine::RENDER_HEIGHT,
		gbufferComponents,
		FrameBuffer::DepthAttachement::CREATE_TEXTURE
		);

	auto gbufferTextures = GBuffer.frameBuffer->getColorAttachements();
	GBuffer.position = gbufferTextures[0];
	GBuffer.normal = gbufferTextures[1];
	GBuffer.albedo = gbufferTextures[2];
	GBuffer.metalnessRoughnessShadow = gbufferTextures[3];
	GBuffer.emissive = gbufferTextures[4];
	GBuffer.depth = GBuffer.frameBuffer->getDepthAttachement();
}
void RenderPipeline::InitShadowmap()
{
	const std::vector<FrameBuffer::ColorAttachement> noColorAttachements = {};
	Shadowmap.frameBuffer = FrameBuffer::Create(
		HFEngine::SHADOWMAP_SIZE, HFEngine::SHADOWMAP_SIZE,
		noColorAttachements,
		FrameBuffer::DepthAttachement::CREATE_TEXTURE
		);
	Shadowmap.depthmap = Shadowmap.frameBuffer->getDepthAttachement();
	Shadowmap.depthmap->setFiltering(GL_NEAREST, GL_NEAREST);
	Shadowmap.depthmap->setCompare(GL_COMPARE_REF_TO_TEXTURE, GL_LEQUAL);
	Shadowmap.depthmap->setEdges(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
	Shadowmap.depthmap->setBorderColor(1.0f, 1.0f, 1.0f, 1.0f);
}
void RenderPipeline::InitRenderSystems()
{
	RenderSystems.meshRenderer = HFEngine::ECS.RegisterSystem<MeshRendererSystem>();
	{
		Signature signature;
		signature.set(HFEngine::ECS.GetComponentType<MeshRenderer>());
		HFEngine::ECS.SetSystemSignature<MeshRendererSystem>(signature);
	}
	RenderSystems.skinnedMeshRender = HFEngine::ECS.RegisterSystem<SkinnedMeshRendererSystem>();
	{
		Signature signature;
		signature.set(HFEngine::ECS.GetComponentType<SkinnedMeshRenderer>());
		HFEngine::ECS.SetSystemSignature<SkinnedMeshRendererSystem>(signature);
	}
	RenderSystems.pointLightRenderer = HFEngine::ECS.RegisterSystem<PointLightRendererSystem>();
	{
		Signature signature;
		signature.set(HFEngine::ECS.GetComponentType<PointLightRenderer>());
		HFEngine::ECS.SetSystemSignature<PointLightRendererSystem>(signature);
	}
	RenderSystems.particleRenderer = HFEngine::ECS.RegisterSystem<ParticleRendererSystem>();
	{
		Signature signature;
		signature.set(HFEngine::ECS.GetComponentType<ParticleContainer>());
		signature.set(HFEngine::ECS.GetComponentType<ParticleRenderer>());
		HFEngine::ECS.SetSystemSignature<ParticleRendererSystem>(signature);
	}

#ifdef _DEBUG
	RenderSystems.boxColliderRenderer = HFEngine::ECS.RegisterSystem<BoxColliderRenderSystem>();
	{
		Signature signature;
		signature.set(HFEngine::ECS.GetComponentType<BoxCollider>());
		HFEngine::ECS.SetSystemSignature<BoxColliderRenderSystem>(signature);
	}
	RenderSystems.circleColliderRenderer = HFEngine::ECS.RegisterSystem<CircleColliderRenderSystem>();
	{
		Signature signature;
		signature.set(HFEngine::ECS.GetComponentType<CircleCollider>());
		HFEngine::ECS.SetSystemSignature<CircleColliderRenderSystem>(signature);
	}
#endif //  _DEBUG
}

void RenderPipeline::InitPostprocessingEffects()
{
	// init swap buffers
	const std::vector<FrameBuffer::ColorAttachement> colorAttachement = {
		// internalFormat, dataFormat, dataType
		{GL_RGB, GL_RGB, GL_UNSIGNED_BYTE},	// albedo
	};

	PostprocessingSwapBuffers[0] = FrameBuffer::Create(
		HFEngine::RENDER_WIDTH, HFEngine::RENDER_HEIGHT,
		colorAttachement,
		FrameBuffer::DepthAttachement::DEFAULT
		);
	PostprocessingSwapBuffers[1] = FrameBuffer::Create(
		HFEngine::RENDER_WIDTH, HFEngine::RENDER_HEIGHT,
		colorAttachement,
		FrameBuffer::DepthAttachement::DEFAULT
		);

	// init effects
	postprocessingEffects.push_back(std::make_shared<RiverFogEffect>());
	
	for (auto fx : postprocessingEffects)
		fx->Init();
}

void RenderPipeline::Init()
{
	if (initialized)
	{
		LogWarning("RenderPipeline::Init(): Already initialized");
		return;
	}

	InitGBuffer();
	InitShadowmap();
	InitRenderSystems();
	InitPostprocessingEffects();

	combineGBufferShader = ShaderManager::GetShader("CombineGBuffer");
	combineGBufferShader->use();
	combineGBufferShader->setInt("gPosition", (int)GBufferBindingPoint::POSITION);
	combineGBufferShader->setInt("gNormal", (int)GBufferBindingPoint::NORMAL);
	combineGBufferShader->setInt("gAlbedo", (int)GBufferBindingPoint::ALBEDO);
	combineGBufferShader->setInt("gMetalnessRoughnessShadow", (int)GBufferBindingPoint::METALNESS_ROUGHNESS_SHADOW);
	combineGBufferShader->setInt("gEmissive", (int)GBufferBindingPoint::EMISSIVE);

	initialized = true;
	LogInfo("RenderPipeline initialized.");
}


void RenderPipeline::Render()
{
	// clear texture bound cache
	Texture::NoBindAll(true);

	// prepare cameras
	static Camera lightCamera;
	Camera& viewCamera = HFEngine::MainCamera;
	CalculateLightCamera(viewCamera, lightCamera);

	Frustum viewFrustum = viewCamera.GetFrustum();
	Frustum lightFrustum = lightCamera.GetFrustum();

	//auto start = std::chrono::high_resolution_clock::now();
	// schedule culling
	RenderSystems.meshRenderer->ScheduleCulling(viewFrustum, lightFrustum);
	RenderSystems.skinnedMeshRender->ScheduleCulling(viewFrustum, lightFrustum);

	// wait for culling
	RenderSystems.meshRenderer->FinishCulling();
	RenderSystems.skinnedMeshRender->FinishCulling();
	//auto elapsed = std::chrono::high_resolution_clock::now() - start;
	//long us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
	//LogInfo("RENDERING: Culling time: {} us", us);

	// draw to shadowmap
	Shadowmap.frameBuffer->bind();
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
	RenderSystems.meshRenderer->RenderToShadowmap(lightCamera);
	RenderSystems.skinnedMeshRender->RenderToShadowmap(lightCamera);

	// schedule particle culling
	RenderSystems.particleRenderer->ScheduleCulling(viewFrustum, lightFrustum);

	// draw to gbuffer
	GBuffer.frameBuffer->bind();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderSystems.meshRenderer->RenderToGBuffer(viewCamera, lightCamera, Shadowmap.depthmap);
	RenderSystems.skinnedMeshRender->RenderToGBuffer(viewCamera, lightCamera, Shadowmap.depthmap);
	//RenderSystems.cubeRenderer->Render();

	// synchronize light rendering with finished gbuffer, make  write to emissive safely
	glWaitSync(
		glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, GL_ZERO),
		GL_ZERO,
		GL_TIMEOUT_IGNORED
	); // glWaitSync doesn't block CPU
	// it makes sure all previous GPU commands will be completed until next scheduled commands

	// bind gbuffer textures
	GBuffer.position->bind((int)GBufferBindingPoint::POSITION);
	GBuffer.normal->bind((int)GBufferBindingPoint::NORMAL);
	GBuffer.albedo->bind((int)GBufferBindingPoint::ALBEDO);
	GBuffer.metalnessRoughnessShadow->bind((int)GBufferBindingPoint::METALNESS_ROUGHNESS_SHADOW);
	GBuffer.emissive->bind((int)GBufferBindingPoint::EMISSIVE);

	// draw point lights
	glm::vec2 viewportSize = { GBuffer.frameBuffer->width, GBuffer.frameBuffer->height };
	RenderSystems.pointLightRenderer->Render(viewCamera, viewportSize);

	// combine gbuffer
	//FrameBuffer::BindDefaultScreen();
	FrameBuffer::BlitDepth(GBuffer.frameBuffer, PostprocessingSwapBuffers[0]);
	FrameBuffer::BlitDepth(GBuffer.frameBuffer, PostprocessingSwapBuffers[1]);
	PostprocessingSwapBuffers[0]->bind();

	combineGBufferShader->use();
	HFEngine::MainCamera.Use(combineGBufferShader); // for gCameraPosition
	HFEngine::WorldLight.Apply(combineGBufferShader); // for gDirectionalLight struct
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	PrimitiveRenderer::DrawScreenQuad();

	// wait for particle culling
	RenderSystems.particleRenderer->FinishCulling();

	// draw forward
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE); // disable zbuffer writing
	RenderSystems.particleRenderer->Render(viewCamera);
	RenderSystems.meshRenderer->RenderForward(viewCamera, HFEngine::WorldLight);
	RenderSystems.skinnedMeshRender->RenderForward(viewCamera, HFEngine::WorldLight);
	glDepthMask(GL_TRUE);
	glDisable(GL_DEPTH_TEST);

	glDisable(GL_BLEND);

	// apply post processing
	glDepthMask(GL_FALSE);
	for (auto fx : postprocessingEffects)
	{
		bool swap = postprocessingEffects[0]->Process(
			PostprocessingSwapBuffers[0], PostprocessingSwapBuffers[1], GBuffer
			);
		if (swap)
			std::swap(PostprocessingSwapBuffers[0], PostprocessingSwapBuffers[1]);
	}
	glDepthMask(GL_TRUE);

	// show rendered
	FrameBuffer::BlitColor(PostprocessingSwapBuffers[0], nullptr, 0);

	// debug rendering
#ifdef _DEBUG
	static bool debugRendering = false;
	if (InputManager::GetKeyDown(GLFW_KEY_F1)) {
		debugRendering = !debugRendering;
		LogInfo("[DEBUG] Debug Rendering set to: {}", debugRendering);
	}

	if (debugRendering)
	{
		RenderSystems.boxColliderRenderer->Render();
		RenderSystems.circleColliderRenderer->Render();
		PrimitiveRenderer::DrawLines();
		PrimitiveRenderer::DrawStickyPoints();
	}
#endif //  _DEBUG
}
