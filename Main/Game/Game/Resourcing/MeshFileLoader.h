#pragma once

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include "Mesh.h"

class MeshFileLoader
{
private:
	Assimp::Importer importer;
	std::string filepath;

public:
	MeshFileLoader(std::string filepath);

	bool ReadMeshData(std::vector<Vertex>& vertices, std::vector<unsigned>& indices);
};

