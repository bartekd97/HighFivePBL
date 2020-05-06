#pragma once

#include "../../Resourcing/Mesh.h"
#include "../../Resourcing/Material.h"
#include "ECS/ECSTypes.h"

struct MeshRenderer
{
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;

	AABBStruct worldAABB;
	glm::mat4 worldTransform;
	FrameCounter lastAABBUpdate = 0;

	bool visibleByMainCamera = true;
	bool visibleByLightCamera = true;
	bool enabled = true;
	bool castShadows = true;
};
