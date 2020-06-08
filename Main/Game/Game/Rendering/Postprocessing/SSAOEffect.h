#pragma once

#include <glm/glm.hpp>
#include "IPostprocessingEffect.h"

class SSAOEffect : public IPostprocessingEffect
{
private:
	std::shared_ptr<Shader> SSAOShader;
	std::shared_ptr<Shader> SSAOBlurShader;
	std::shared_ptr<FrameBuffer> frameBuffer;
	std::shared_ptr<Texture> noiseTexture;

public:
	virtual void Init() override;
	virtual bool Process(
		std::shared_ptr<FrameBuffer> source,
		std::shared_ptr<FrameBuffer> destination,
		RenderPipeline::GBufferStruct& gbuffer) override;
};
