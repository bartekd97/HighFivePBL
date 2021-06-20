#pragma once

#include "CulledRenderer.h"
#include "Rendering/FrameBuffer.h"

struct GrassPatchRenderer
{
	glm::vec4 worldBounds;
	float patchSize;
	float grassScale;
	int density;

	std::shared_ptr<Texture> densityTexture;
	std::shared_ptr<FrameBuffer> simulationTexture;

	AABBStruct localPatchAABB;
	CulledRenderer cullingData;
};