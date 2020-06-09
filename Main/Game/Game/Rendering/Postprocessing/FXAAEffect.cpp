#include "FXAAEffect.h"
#include "Resourcing/Shader.h"
#include "Rendering/PrimitiveRenderer.h"
#include "../../HFEngine.h"
#include "../../InputManager.h"

void FXAAEffect::Init()
{
	shader = ShaderManager::GetShader("FXAA");
	shader->use();
	shader->setInt("gTexture", 0);
	shader->setVector3F("inverseFilterTextureSize", 1.0f / HFEngine::RENDER_WIDTH, 1.0f / HFEngine::RENDER_HEIGHT, 0.0f);
	shader->setFloat("SpanMax", 8.0);
	shader->setFloat("ReduceMin", 1.0 / 128.0);
	shader->setFloat("ReduceMul", 1.0 / 8.0);
}

bool FXAAEffect::Process(
	std::shared_ptr<FrameBuffer> source,
	std::shared_ptr<FrameBuffer> destination,
	RenderPipeline::GBufferStruct& gbuffer
)
{
	static bool enabled = true;
	if (InputManager::GetKeyDown(GLFW_KEY_F3))
	{
		enabled = !enabled;
	}
	if (!enabled)
	{
		return false;
	}

	shader->use();
	source->getColorAttachement(0)->bind(0);
	destination->bind();
	PrimitiveRenderer::DrawScreenQuad();
	return true;
}