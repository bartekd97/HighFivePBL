#pragma once

#include <array>
#include <glm/glm.hpp>
#include "../../Resourcing/Mesh.h"
#include "../../Resourcing/Material.h"
#include "../../Resourcing/SkinningData.h"

struct SkinnedMeshRenderer
{
	static const int MAX_BONES = 64;

	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
	std::shared_ptr<SkinningData> skinningData;

	std::array<glm::mat4, MAX_BONES> boneMatrices{};

	bool enabled = true;
};
