#pragma once

#include "FrameBuffer.h"

class RenderPipeline
{
private:
	struct GBufferStruct {
		std::shared_ptr<Texture> position;
		std::shared_ptr<Texture> normal;
		std::shared_ptr<Texture> albedo;
		std::shared_ptr<Texture> metalnessRoughness;
		std::shared_ptr<Texture> emissive;

		std::shared_ptr<FrameBuffer> frameBuffer;
	};

	GBufferStruct GBuffer;
	bool initialized = false;

public:
	void Init();
	void Render();
};

