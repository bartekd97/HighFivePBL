#pragma once

#include <memory>
#include "../RenderPipeline.h"


class IPostprocessingEffect
{
public:
	virtual void Init() {}
	// return true if actually destination was written (buffer swap needed)
	virtual bool Process(
		std::shared_ptr<FrameBuffer> source,
		std::shared_ptr<FrameBuffer> destination,
		RenderPipeline::GBufferStruct& gbuffer) = 0;
};