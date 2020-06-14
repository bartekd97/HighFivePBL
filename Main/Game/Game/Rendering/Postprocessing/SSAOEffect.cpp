#include <random>

#include "SSAOEffect.h"
#include "../PrimitiveRenderer.h"
#include "Resourcing/Shader.h"
#include "../../HFEngine.h"
#include "../../InputManager.h"

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

void SSAOEffect::Init()
{
	SSAOShader = ShaderManager::GetShader("PPSSAO");
	SSAOShader->use();
	SSAOShader->setInt("gPosition", 0);
	SSAOShader->setInt("gNormal", 1);
	SSAOShader->setInt("texNoise", 2);
	SSAOShader->setVector2F("noiseScale", { HFEngine::RENDER_WIDTH / 4, HFEngine::RENDER_HEIGHT / 4 });

	const std::vector<FrameBuffer::ColorAttachement> colorAttachement = {
		{ GL_RGB, GL_RGB, GL_UNSIGNED_BYTE }
	};

	frameBuffer = FrameBuffer::Create(
		HFEngine::RENDER_WIDTH, HFEngine::RENDER_HEIGHT,
		colorAttachement,
		FrameBuffer::DepthAttachement::DEFAULT
	);

	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
	std::default_random_engine generator;
	std::vector<glm::vec3> ssaoKernel;
	for (unsigned int i = 0; i < 24; i++)
	{
		glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / 24.0;

		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}
	for (unsigned int i = 0; i < 24; i++)
	{
		SSAOShader->setVector3F("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
	}

	std::vector<unsigned char> ssaoNoise;
	for (unsigned int i = 0; i < 16; i++)
	{
		ssaoNoise.push_back(static_cast<unsigned char>((randomFloats(generator) * 2.0f - 1.0f) * 255.0f));
		ssaoNoise.push_back(static_cast<unsigned char>((randomFloats(generator) * 2.0f - 1.0f) * 255.0f));
		ssaoNoise.push_back(static_cast<unsigned char>(0.0f));
	}

	TextureConfig nConfig;
	nConfig.format = GL_RGBA32F;
	nConfig.filteringMin = GL_NEAREST;
	nConfig.filteringMag = GL_NEAREST;
	nConfig.repeat = true;
	noiseTexture = TextureManager::CreateTextureFromRawData(ssaoNoise.data(), 4, 4, GL_RGB, nConfig);

	SSAOBlurShader = ShaderManager::GetShader("PPSSAOBlur");
	SSAOBlurShader->use();
	SSAOBlurShader->setInt("ssaoInput", 0);
	SSAOBlurShader->setInt("gTexture", 1);
	SSAOBlurShader->setInt("gAlbedoFade", 2);
}

bool SSAOEffect::PreForwardProcess(
	std::shared_ptr<FrameBuffer> source,
	std::shared_ptr<FrameBuffer> destination,
	RenderPipeline::GBufferStruct& gbuffer)
{
	static bool enabled = true;
	if (InputManager::GetKeyDown(GLFW_KEY_F7))
		enabled = !enabled;
	if (!enabled)
		return false;

	static bool debug = false;
	if (InputManager::GetKeyDown(GLFW_KEY_F6))
	{
		debug = !debug;
	}

	SSAOShader->use();
	gbuffer.position->bind(0);
	gbuffer.normal->bind(1);
	noiseTexture->bind(2);

	SSAOShader->setMat4("gProjection", HFEngine::MainCamera.GetProjectionMatrix());

	frameBuffer->bind();
	PrimitiveRenderer::DrawScreenQuad();

	SSAOBlurShader->use();
	frameBuffer->getColorAttachement(0)->bind(0);
	SSAOBlurShader->setVector3F("ambient", HFEngine::WorldLight.ambient);
	SSAOBlurShader->setInt("debug", (int)debug);
	source->getColorAttachement(0)->bind(1);
	gbuffer.albedoFade->bind(2);

	destination->bind();
	PrimitiveRenderer::DrawScreenQuad();

	return true;
}
