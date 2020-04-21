#pragma once

#include "../../Resourcing/Mesh.h"
#include "../../Resourcing/Material.h"
#include "../../Resourcing/SkinningData.h"

struct SkinnedMeshRenderer
{
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
	std::shared_ptr<SkinningData> skinningData;

	bool enabled = true;
};
