#pragma once

#include <glm/glm.hpp>
#include "IPostprocessingEffect.h"

class Shader;

class FXAAEffect : public IPostprocessingEffect
{
private:
	std::shared_ptr<Shader> shader;

public:
	virtual void Init() override;
	virtual bool Process(
		std::shared_ptr<FrameBuffer> source,
		std::shared_ptr<FrameBuffer> destination,
		RenderPipeline::GBufferStruct& gbuffer) override;
};