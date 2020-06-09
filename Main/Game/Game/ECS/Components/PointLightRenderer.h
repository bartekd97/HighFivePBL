#pragma once

#include "Rendering/Lights.h"
#include "CulledRenderer.h"

struct PointLightRenderer
{
	PointLight light;

	CulledRenderer cullingData;
	AABBStruct localAABB;
};