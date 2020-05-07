#pragma once

#include "../../Resourcing/Material.h"
#include "CulledRenderer.h"

struct MeshRenderer
{
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;

	bool castShadows = true;

	CulledRenderer cullingData;
};
