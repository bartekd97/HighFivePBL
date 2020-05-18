#pragma once

#include <array>
#include <glm/glm.hpp>
#include "../../Resourcing/Material.h"
#include "../../Resourcing/SkinningData.h"
#include "CulledRenderer.h"

class UniformBuffer;
struct SkinnedMeshRenderer
{
	static const int MAX_BONES = 64;

	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
	std::shared_ptr<SkinningData> skinningData;

	std::array<glm::mat4, MAX_BONES> boneMatrices{};
	std::shared_ptr<UniformBuffer> boneMatricesBuffer;
	AABBStruct extendedLocalAABB;
	bool needMatricesBufferUpdate = false;

	bool castShadows = true;

	CulledRenderer cullingData;
};
