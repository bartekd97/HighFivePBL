#include "OrthoSSREffect.h"
#include "Rendering/PrimitiveRenderer.h"
#include "Resourcing/Shader.h"
#include "HFEngine.h"

void OrthoSSREffect::Init()
{
	ssrShader = ShaderManager::GetShader("PPOrthoSSR");
	ssrShader->use();
	ssrShader->setInt("gDepth", 0);
	ssrShader->setInt("gPosition", 1);
	ssrShader->setInt("gNormal", 2);
	ssrShader->setInt("gMetalnessRoughness", 3);
	ssrShader->setInt("gScreenTexture", 4);

	ssrBlendShader = ShaderManager::GetShader("PPOrthoSSRBlend");
	ssrBlendShader->use();
	ssrBlendShader->setInt("gPosition", 1);
	ssrBlendShader->setInt("gMetalnessRoughness", 2);
	ssrBlendShader->setInt("gSSRTexture", 3);

	const std::vector<FrameBuffer::ColorAttachement> colorAttachement = {
		// internalFormat, dataFormat, dataType
		{GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE},
	};
	ssrBuffer = FrameBuffer::Create(
		HFEngine::RENDER_WIDTH / 2, HFEngine::RENDER_HEIGHT / 2,
		colorAttachement,
		FrameBuffer::DepthAttachement::DEFAULT
		);
	ssrBuffer->getColorAttachement(0)->generateMipmaps();
	ssrBuffer->getColorAttachement(0)->setFiltering(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
}

bool OrthoSSREffect::Process(
	std::shared_ptr<FrameBuffer> source,
	std::shared_ptr<FrameBuffer> destination,
	RenderPipeline::GBufferStruct& gbuffer
	)
{
	Camera& viewCamera = HFEngine::MainCamera;

	ssrBuffer->bind();

	ssrShader->use();
	ssrShader->setVector3F("gCameraDirection", viewCamera.GetViewDiorection());
	ssrShader->setMat4("gViewProjection",
		viewCamera.GetProjectionMatrix() * viewCamera.GetViewMatrix()
	);
	gbuffer.depth->bind(0);
	gbuffer.position->bind(1);
	gbuffer.normal->bind(2);
	gbuffer.metalnessRoughnessShadow->bind(3);
	source->getColorAttachement(0)->bind(4);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	PrimitiveRenderer::DrawScreenQuad();


	source->bind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	ssrBlendShader->use();
	gbuffer.position->bind(1);
	gbuffer.metalnessRoughnessShadow->bind(2);
	ssrBuffer->getColorAttachement(0)->generateMipmaps();
	ssrBuffer->getColorAttachement(0)->bind(3);
	PrimitiveRenderer::DrawScreenQuad();
	glDisable(GL_BLEND);

	return false;
}
