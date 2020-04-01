#pragma once

#include "Mesh.h"
#include "Material.h"

struct MeshRenderer
{
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;

	bool enabled = true;
};