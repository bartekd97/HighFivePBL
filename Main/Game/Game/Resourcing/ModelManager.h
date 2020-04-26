#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <glad/glad.h>

class Material;
class MaterialLibrary;
struct Vertex;
struct VertexBoneData;
class Mesh;
class SkinningData;
class AnimationClip;
class Model;
class ModelLibrary;


namespace ModelManager {
	void Initialize();

	extern std::shared_ptr<ModelLibrary> GENERIC_LIBRARY;
	extern std::shared_ptr<Model> BLANK_MODEL; // 1 zero vertex mesh with blank material

	std::shared_ptr<Mesh> CreateMesh(std::vector<Vertex>& vertices, std::vector<unsigned>& indices);
	std::shared_ptr<Mesh> CreateMesh(std::vector<Vertex>& vertices, std::vector<unsigned>& indices, std::vector<VertexBoneData>& boneData);
	std::shared_ptr<ModelLibrary> GetLibrary(std::string name);
	std::shared_ptr<Model> GetModel(std::string libraryName, std::string modelName);

	void UnloadUnused(); // call only once per some time
}

class ModelLibrary {
	friend void ModelManager::Initialize();
	friend std::shared_ptr<ModelLibrary> ModelManager::GetLibrary(std::string name);

public:
	const std::string name;

private:
	struct AnimEntry {
		std::string name;
		std::string clip;
	};
	struct LibraryEntity {
		std::string meshFile;
		std::string materialName;
		bool skinned;
		std::vector<AnimEntry> animations;
		std::weak_ptr<Model> model;
	};
	std::unordered_map<std::string, LibraryEntity*> entities;

	std::shared_ptr<MaterialLibrary> materialLibrary;

	ModelLibrary();
	ModelLibrary(std::string name);
	std::shared_ptr<Model> LoadEntity(std::string& name, LibraryEntity* entity);
public:
	~ModelLibrary();
	std::shared_ptr<Model> GetModel(std::string name);
};