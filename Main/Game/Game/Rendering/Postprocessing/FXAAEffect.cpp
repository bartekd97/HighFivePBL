#include "FXAAEffect.h"
#include "Resourcing/Shader.h"
#include "Rendering/PrimitiveRenderer.h"
#include "../../HFEngine.h"

void FXAAEffect::Init()
{
	shader = ShaderManager::GetShader("FXAA");
	shader->use();
	shader->setInt("gTexture", 0);
	shader->setVector3F("inverseFilterTextureSize", 1.0f / HFEngine::RENDER_WIDTH, 1.0f / HFEngine::RENDER_HEIGHT, 0.0f);
	shader->setFloat("SpanMax", 8.0);
	shader->setFloat("ReduceMin", 1.0 / 128.0);
	shader->setFloat("ReduceMul", 1.0 / 2.0);
}

bool FXAAEffect::Process(
	std::shared_ptr<FrameBuffer> source,
	std::shared_ptr<FrameBuffer> destination,
	RenderPipeline::GBufferStruct& gbuffer
)
{
	shader->use();
	source->getColorAttachement(0)->bind(0);
	destination->bind();
	PrimitiveRenderer::DrawScreenQuad();
	return true;
}