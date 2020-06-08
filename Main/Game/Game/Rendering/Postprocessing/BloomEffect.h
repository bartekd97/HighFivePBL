#pragma once

#include <glm/glm.hpp>
#include "IPostprocessingEffect.h"

class Shader;

class BloomEffect : public IPostprocessingEffect
{
private:
	std::shared_ptr<Shader> bloom;
	std::shared_ptr<Shader> horizontalBlur;
	std::shared_ptr<Shader> verticalBlur;
	std::shared_ptr<Shader> resultBloom;

	std::shared_ptr<FrameBuffer> framebufferA;
	std::shared_ptr<FrameBuffer> framebufferB;
	float brightnessThreshold;

public:
	virtual void Init() override;
	virtual bool Process(
		std::shared_ptr<FrameBuffer> source,
		std::shared_ptr<FrameBuffer> destination,
		RenderPipeline::GBufferStruct& gbuffer) override;
};