#pragma once

#include "../../Resourcing/Mesh.h"
#include "../../Resourcing/Material.h"

struct MeshRenderer
{
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;

	bool enabled = true;
};
