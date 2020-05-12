#pragma once

#include <glm/glm.hpp>
#include "IPostprocessingEffect.h"

class Shader;
class Mesh;
class Event;

class RiverFogEffect : public IPostprocessingEffect
{
public:
	const float FogHeightLevel = -0.5f;

private:
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Mesh> fogPlane;
	std::shared_ptr<Texture> noise;

	glm::vec2 noiseOffsets[3];

	void LateUpdateNoise(Event& event);

public:
	glm::vec2 noiseSpeeds[3] = {
		glm::vec2(0.015f, 0.0f),
		glm::vec2(0.075f, 0.015f),
		glm::vec2(0.0f, -0.015f)
	};

	virtual void Init() override;
	virtual bool Process(
		std::shared_ptr<FrameBuffer> source,
		std::shared_ptr<FrameBuffer> destination,
		RenderPipeline::GBufferStruct& gbuffer) override;
};