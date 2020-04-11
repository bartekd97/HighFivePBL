#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <glad/glad.h>

class Material;
class MaterialLibrary;
struct Vertex;
class Mesh;
class Model;
class ModelLibrary;


namespace ModelManager {
	void Initialize();

	extern std::shared_ptr<ModelLibrary> GENERIC_LIBRARY;
	extern std::shared_ptr<Model> BLANK_MODEL; // 1 zero vertex mesh with blank material

	std::shared_ptr<Mesh> CreateMesh(std::vector<Vertex>& vertices, std::vector<unsigned>& indices);
	std::shared_ptr<ModelLibrary> GetLibrary(std::string name);
	std::shared_ptr<Model> GetModel(std::string libraryName, std::string modelName);
}

class ModelLibrary {
	friend void ModelManager::Initialize();
	friend std::shared_ptr<ModelLibrary> ModelManager::GetLibrary(std::string name);

public:
	const std::string name;

private:
	struct LibraryEntity {
		std::string meshFile;
		std::string materialName;
		std::weak_ptr<Model> model;
		std::weak_ptr<Mesh> meshCache;
		std::weak_ptr<Material> materialCache;
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