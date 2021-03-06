#pragma once

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include "Mesh.h"
#include "SkinningData.h"
#include "AnimationClip.h"

class MeshFileLoader
{
private:
	Assimp::Importer importer;
	std::string filepath;

public:
	MeshFileLoader(std::string filepath);

	bool ReadMeshData(std::vector<Vertex>& vertices, std::vector<unsigned>& indices, AABBStruct& AABB);
	bool ReadBoneData(std::vector<VertexBoneData>& data, std::shared_ptr<SkinningData>& skinning);
	bool ReadAnimation(std::shared_ptr<AnimationClip>& animation);
};

