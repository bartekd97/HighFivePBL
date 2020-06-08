#include "BloomEffect.h"
#include "HFEngine.h"
#include "InputManager.h"

#include "Resourcing/Shader.h"
#include "Rendering/PrimitiveRenderer.h"

void BloomEffect::Init()
{
	const std::vector<FrameBuffer::ColorAttachement> colorAttachement = {
		{GL_RGB16F, GL_RGB, GL_FLOAT}
	};
	framebufferA = FrameBuffer::Create(
		HFEngine::RENDER_WIDTH, HFEngine::RENDER_HEIGHT,
		colorAttachement,
		FrameBuffer::DepthAttachement::DEFAULT
	);
	framebufferB = FrameBuffer::Create(
		HFEngine::RENDER_WIDTH, HFEngine::RENDER_HEIGHT,
		colorAttachement,
		FrameBuffer::DepthAttachement::DEFAULT
	);

	brightnessThreshold = 0.55;
	bloom = ShaderManager::GetShader("Bloom");
	bloom->use();
	bloom->setInt("diffuseTexture", 0);
	bloom->setFloat("BrightnessThreshold", brightnessThreshold);


	horizontalBlur = ShaderManager::GetShader("HorizontalBlur");
	horizontalBlur->use();
	horizontalBlur->setInt("image", 0);

	verticalBlur = ShaderManager::GetShader("VerticalBlur");
	verticalBlur->use();
	horizontalBlur->setInt("image", 0);

	resultBloom = ShaderManager::GetShader("BloomResult");
	resultBloom->use();
	resultBloom->setInt("scene", 0);
	resultBloom->setInt("bloomBlur", 1);
}

static bool bloomEffectEnabled = true;


bool BloomEffect::Process(
	std::shared_ptr<FrameBuffer> source,
	std::shared_ptr<FrameBuffer> destination,
	RenderPipeline::GBufferStruct& gbuffer)
{
	if (InputManager::GetKeyDown(GLFW_KEY_F4))
	{
		bloomEffectEnabled = !bloomEffectEnabled;
	}
	if (!bloomEffectEnabled)
	{
		return false;
	}

	
	int iterations = 10;

	bloom->use();
	source->getColorAttachement(0)->bind(0);
	framebufferA->bind();
	PrimitiveRenderer::DrawScreenQuad();

	for (int i = 0; i < iterations; i++)
	{
		horizontalBlur->use();
		framebufferA->getColorAttachement(0)->bind(0);
		framebufferB->bind();
		PrimitiveRenderer::DrawScreenQuad();

		std::swap(framebufferA, framebufferB);

		verticalBlur->use();
		framebufferA->getColorAttachement(0)->bind(0);
		framebufferB->bind();
		PrimitiveRenderer::DrawScreenQuad();

		std::swap(framebufferA, framebufferB);
	}

	resultBloom->use();
	source->getColorAttachement(0)->bind(0);
	framebufferB->getColorAttachement(0)->bind(1);


	destination->bind();
	PrimitiveRenderer::DrawScreenQuad();
	return true;
}