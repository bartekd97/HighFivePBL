#pragma once

#include "../../Resourcing/Mesh.h"
#include "ECS/ECSTypes.h"

struct CulledRenderer
{
	AABBStruct worldAABB;
	glm::mat4 worldTransform;
	FrameCounter lastUpdate = 0;

	bool visibleByMainCamera = true;
	bool visibleByLightCamera = true;
};