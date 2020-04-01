#include <glad/glad.h>
#include "RenderPipeline.h"
#include "HFEngine.h"
#include "Logger.h"

void RenderPipeline::Init()
{
	if (initialized)
	{
		LogWarning("RenderPipeline::Init(): Already initialized");
		return;
	}

	std::vector<FrameBuffer::ColorAttachement> gbufferComponents = {
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

	initialized = true;
	LogInfo("RenderPipeline initialized.");
}


void RenderPipeline::Render()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
