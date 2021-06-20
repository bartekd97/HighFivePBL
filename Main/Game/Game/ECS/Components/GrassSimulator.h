#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "../ECSTypes.h"
#include "Rendering/FrameBuffer.h"

struct GrassSimulator
{
	glm::vec4 worldBounds;

	std::shared_ptr<FrameBuffer> simulationTexture;
	std::shared_ptr<FrameBuffer> simulationTextureBack;
	std::shared_ptr<FrameBuffer> renderTexture;

	std::vector<GameObject> collectedPrimitives;
	float inactivityTime = 0.0f;
};