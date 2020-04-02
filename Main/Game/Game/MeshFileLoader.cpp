#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Logger.h"
#include "MeshFileLoader.h"

MeshFileLoader::MeshFileLoader(std::string filepath)
{
	const unsigned int aiFlags = aiProcess_Triangulate |
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals |
		//aiProcess_PreTransformVertices |
		//aiProcess_RemoveRedundantMaterials |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |
		aiProcess_GenBoundingBoxes; // for later usage

	if (importer.ReadFile(filepath, aiFlags) == NULL)
	{
		LogError("MeshFileLoader::MeshFileLoader(): Cannot read '{}'. Reason: {}", filepath, importer.GetErrorString());
	}

	this->filepath = filepath;
}

bool MeshFileLoader::ReadMeshData(std::vector<Vertex>& vertices, std::vector<unsigned>& indices)
{
	const aiScene* scene = importer.GetScene();
	if (scene == NULL)
		return false;

	if (scene->mNumMeshes > 1)
	{
		// TODO: improve mesh loader
		LogWarning("MeshFileLoader::ReadMeshData(): More than 1 mesh in '{}', please take care of it", filepath);
	}
	aiMesh* mesh = scene->mMeshes[0];
	vertices.reserve(mesh->mNumVertices);
	indices.reserve(mesh->mNumFaces * 3);

	Vertex v;
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		v.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
		v.uv = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
		v.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
		v.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
		v.bitangent = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
		vertices.push_back(v);
	}

	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		indices.push_back(mesh->mFaces[i].mIndices[0]);
		indices.push_back(mesh->mFaces[i].mIndices[1]);
		indices.push_back(mesh->mFaces[i].mIndices[2]);
	}

	return true;
}
