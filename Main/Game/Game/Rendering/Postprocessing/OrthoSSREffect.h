#pragma once

#include <glm/glm.hpp>
#include "IPostprocessingEffect.h"

class Shader;

class OrthoSSREffect : public IPostprocessingEffect
{
private:
	std::shared_ptr<Shader> ssrShader;
	std::shared_ptr<Shader> ssrBlendShader;
	std::shared_ptr<FrameBuffer> ssrBuffer;

public:
	virtual void Init() override;
	virtual bool Process(
		std::shared_ptr<FrameBuffer> source,
		std::shared_ptr<FrameBuffer> destination,
		RenderPipeline::GBufferStruct& gbuffer) override;
};