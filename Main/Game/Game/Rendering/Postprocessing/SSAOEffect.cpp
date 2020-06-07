#include <random>

#include "SSAOEffect.h"
#include "../PrimitiveRenderer.h"
#include "Resourcing/Shader.h"
#include "../../HFEngine.h"
#include "Rendering/RenderPipeline.h"

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

	const std::vector<FrameBuffer::ColorAttachement> colorAttachement = {
		{ GL_RGB, GL_RGB, GL_UNSIGNED_BYTE }
	};

	frameBuffer = FrameBuffer::Create(
		HFEngine::RENDER_WIDTH / 2, HFEngine::RENDER_HEIGHT / 2,
		colorAttachement,
		FrameBuffer::DepthAttachement::DEFAULT
	);

	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
	std::default_random_engine generator;
	std::vector<glm::vec3> ssaoKernel;
	for (unsigned int i = 0; i < 64; ++i)
	{
		glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / 64.0;

		// scale samples s.t. they're more aligned to center of kernel
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}
	for (unsigned int i = 0; i < 64; ++i)
		SSAOShader->setVector3F("samples[" + std::to_string(i) + "]", ssaoKernel[i]);

	std::vector<glm::vec3> ssaoNoise;
	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
		ssaoNoise.push_back(noise);
	}
	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	SSAOBlurShader = ShaderManager::GetShader("PPSSAOBlur");
	SSAOBlurShader->use();
	SSAOBlurShader->setInt("ssaoInput", 0);
	SSAOBlurShader->setInt("gTexture", 1);
}

bool SSAOEffect::Process(
	std::shared_ptr<FrameBuffer> source,
	std::shared_ptr<FrameBuffer> destination,
	RenderPipeline::GBufferStruct& gbuffer)
{
	SSAOShader->use();
	gbuffer.position->bind(0);
	gbuffer.normal->bind(1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);

	SSAOShader->setMat4("projectionView", HFEngine::MainCamera.GetProjectionMatrix() * HFEngine::MainCamera.GetViewMatrix());

	frameBuffer->bind();
	PrimitiveRenderer::DrawScreenQuad();

	SSAOBlurShader->use();
	frameBuffer->getColorAttachement(0)->bind(0);
	SSAOBlurShader->setVector3F("ambient", HFEngine::WorldLight.ambient);
#ifdef HF_DEBUG_RENDER
	SSAOBlurShader->setInt("debug", RenderPipeline::debugRendering ? 1 : 0);
#else
	SSAOBlurShader->setInt("debug", 0);
#endif
	source->getColorAttachement(0)->bind(1);

	destination->bind();
	PrimitiveRenderer::DrawScreenQuad();

	return true;
}
