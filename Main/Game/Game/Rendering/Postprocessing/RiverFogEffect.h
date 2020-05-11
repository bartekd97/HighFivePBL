#pragma once

#include <glm/glm.hpp>
#include "IPostprocessingEffect.h"

class Shader;
class Mesh;

class RiverFogEffect : public IPostprocessingEffect
{
public:
	const float FogHeightLevel = -0.5f;

private:
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Mesh> fogPlane;
	std::shared_ptr<Texture> noise;

public:
	glm::vec2 noiseOffsets[3];

	virtual void Init() override;
	virtual bool Process(
		std::shared_ptr<FrameBuffer> source,
		std::shared_ptr<FrameBuffer> destination,
		RenderPipeline::GBufferStruct& gbuffer) override;
};