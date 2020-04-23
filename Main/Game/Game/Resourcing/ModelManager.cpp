#include <filesystem>
#include <tinyxml2.h>
#include <stb_image.h>
#include "Global.h"
#include "../Utility/Logger.h"
#include "../Utility/Utility.h"
#include "Material.h"
#include "Mesh.h"
#include "Model.h"
#include "MeshFileLoader.h"

using namespace tinyxml2;

// variables
namespace ModelManager {
	std::shared_ptr<ModelLibrary> GENERIC_LIBRARY;
	std::shared_ptr<Model> BLANK_MODEL;

	std::vector<std::shared_ptr<Model>> CacheHolder;

	bool Initialized = false;

	std::unordered_map<std::string, std::shared_ptr<ModelLibrary>> LibraryContainer;
}

// privates
namespace ModelManager {
	GLuint MakeAndSetupVBO(std::vector<Vertex>& vertices)
	{
		GLuint vbo;
		glGenBuffers(1, &vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(Vertex) * vertices.size(),
			&vertices[0],
			GL_STATIC_DRAW);

		// pos attrib
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		glEnableVertexAttribArray(0);
		// uv attrib
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
		glEnableVertexAttribArray(1);
		// normal attrib
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(2);
		// tangent attrib
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
		glEnableVertexAttribArray(3);
		// bitangent attrib
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
		glEnableVertexAttribArray(4);

		return vbo;
	}

	GLuint MakeAndSetupBoneVBO(std::vector<VertexBoneData>& boneData)
	{
		GLuint bvbo;
		glGenBuffers(1, &bvbo);

		glBindBuffer(GL_ARRAY_BUFFER, bvbo);
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(VertexBoneData) * boneData.size(),
			&boneData[0],
			GL_STATIC_DRAW);

		// pos attrib
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(VertexBoneData), (void*)offsetof(VertexBoneData, bones));
		glEnableVertexAttribArray(5);
		// uv attrib
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (void*)offsetof(VertexBoneData, weights));
		glEnableVertexAttribArray(6);

		return bvbo;
	}

	GLuint MakeAndSetupEBO(std::vector<unsigned>& indices)
	{
		GLuint ebo;
		glGenBuffers(1, &ebo);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			sizeof(unsigned) * indices.size(),
			&indices[0],
			GL_STATIC_DRAW);

		return ebo;
	}
}


// publics
void ModelManager::Initialize()
{
	if (Initialized)
	{
		LogWarning("ModelManager::Initialize(): Already initialized");
		return;
	}

	GENERIC_LIBRARY = std::shared_ptr<ModelLibrary>(new ModelLibrary());

	std::vector<Vertex> vertices0; vertices0.push_back(Vertex());
	std::vector<unsigned> indices0; indices0.push_back(0);
	std::shared_ptr<Mesh> mesh0 = CreateMesh(vertices0, indices0);
	BLANK_MODEL = std::shared_ptr<Model>(new Model(mesh0, MaterialManager::BLANK_MATERIAL));

	Initialized = true;

	LogInfo("ModelManager initialized.");
}



std::shared_ptr<Mesh> ModelManager::CreateMesh(std::vector<Vertex>& vertices, std::vector<unsigned>& indices)
{
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo = MakeAndSetupVBO(vertices);
	GLuint ebo = MakeAndSetupEBO(indices);

	glBindVertexArray(0);

	return std::shared_ptr<Mesh>(new Mesh(vao, vbo, 0, ebo, indices.size()));
}

std::shared_ptr<Mesh> ModelManager::CreateMesh(std::vector<Vertex>& vertices, std::vector<unsigned>& indices, std::vector<VertexBoneData>& boneData)
{
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo = MakeAndSetupVBO(vertices);
	GLuint bvbo = MakeAndSetupBoneVBO(boneData);
	GLuint ebo = MakeAndSetupEBO(indices);

	glBindVertexArray(0);

	return std::shared_ptr<Mesh>(new Mesh(vao, vbo, bvbo, ebo, indices.size()));
}

std::shared_ptr<ModelLibrary> ModelManager::GetLibrary(std::string name)
{
#ifdef _DEBUG
	for (auto lib : LibraryContainer)
	{
		const char* key = lib.first.c_str();
		if (_stricmp(key, name.c_str()) == 0 &&
			strcmp(key, name.c_str()) != 0)
		{
			LogWarning("ModelLibrary::GetLibrary(): Trying to get duplciate of '{}' library but with different letter size. It causes dupliacate and increased memory usage!!!", name);
		}
	}
#endif
	try
	{
		return LibraryContainer.at(name);
	}
	catch (std::out_of_range ex)
	{
		if (std::filesystem::is_directory(Global::ASSETS_DIRECTORY + name))
		{
			LibraryContainer[name] = std::shared_ptr<ModelLibrary>(new ModelLibrary(name));
			return LibraryContainer[name];
		}
		else
		{
			LogWarning("ModelManager::GetLibrary(): Cannot find library '{}'", name);
			return GENERIC_LIBRARY;
		}
	}
}

std::shared_ptr<Model> ModelManager::GetModel(std::string libraryName, std::string modelName)
{
	return ModelManager::GetLibrary(libraryName)->GetModel(modelName);
}



void ModelManager::UnloadUnused()
{
	int i = 0;
	for (; i < CacheHolder.size(); i++)
	{
		const auto& model = CacheHolder.at(i);

		if (model.use_count() > 1)
			continue;

		if (model->material.use_count() > 1 ||
			model->mesh.use_count() > 1 ||
			model->skinningData.use_count() > 1)
			continue;

		CacheHolder.erase(CacheHolder.begin() + i);
		i--;
	}
}



// classes

ModelLibrary::ModelLibrary() : name("GENERIC")
{}
ModelLibrary::ModelLibrary(std::string name) : name(name)
{
	const std::string libraryFolder = Global::ASSETS_DIRECTORY + name + "/";

	XMLDocument doc;
	auto loaded = doc.LoadFile((libraryFolder + "models.xml").c_str());
	if (loaded != XML_SUCCESS)
	{
		LogError("ModelLibrary::ModelLibrary(): Cannot load models.xml for '{}'. Code: {}", name, loaded);
		return;
	}

	auto root = doc.FirstChildElement();
	if (root == nullptr)
	{
		LogError("ModelLibrary::ModelLibrary(): Cannot find root element in models.xml for '{}'", name);
		return;
	}

	std::string materialLibraryName = nullableString(root->Attribute("materialLibrary"));
	if (materialLibraryName.size() > 0)
	{
		if (materialLibraryName == "this")
			materialLibraryName = name;
		materialLibrary = MaterialManager::GetLibrary(materialLibraryName);
		if (materialLibrary->name == "GENERIC")
		{
			LogWarning("ModelLibrary::ModelLibrary(): Cannot find '{}' material library for '{}' model library", materialLibraryName, name);
		}
	}
	else
	{
		materialLibrary = MaterialManager::GENERIC_LIBRARY;
	}


	int i = 0;
	for (XMLElement* node = root->FirstChildElement("model");
		node != nullptr;
		node = node->NextSiblingElement("model"), i++)
	{
		const char* modelName = node->Attribute("name");
		const char* modelMeshFile = node->Attribute("mesh");

		if (modelName == nullptr || modelMeshFile == nullptr)
		{
			LogWarning("ModelLibrary::ModelLibrary(): Invalid model node at #{}", i);
			continue;
		}

		std::string meshFilepath = libraryFolder + modelMeshFile;
		if (!std::filesystem::exists(meshFilepath))
		{
			LogWarning("ModelLibrary::ModelLibrary(): No mesh file found for model '{}'", modelName);
			continue;
		}

		LibraryEntity* entity = new LibraryEntity();
		entity->meshFile = meshFilepath;
		entity->materialName = nullableString(node->Attribute("material"));
		entity->skinned = node->Attribute("skinned", "true") != NULL;

		int j = 0;
		for (tinyxml2::XMLElement* animation = node->FirstChildElement("animation");
			animation != nullptr;
			animation = animation->NextSiblingElement("animation"), j++)
		{
			const char* animationName = animation->Attribute("name");
			const char* animationClip = animation->Attribute("clip");

			if (animationName == nullptr || animationClip == nullptr)
			{
				LogWarning("ModelLibrary::ModelLibrary(): Invalid animation node at #{}/{}", i, j);
				continue;
			}

			entity->animations.push_back({ std::string(animationName), std::string(libraryFolder + animationClip) });
		}

		entities[modelName] = entity;
	}
	LogInfo("ModelLibrary::ModelLibrary(): Initialized '{}' library with {} entities", name, entities.size());
}

ModelLibrary::~ModelLibrary()
{
	for (auto e : entities)
		delete e.second;
}

std::shared_ptr<Model> ModelLibrary::LoadEntity(std::string& name, LibraryEntity* entity)
{
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
	std::shared_ptr<SkinningData> skinningData;
	Model::Animations animations;

	MeshFileLoader loader(entity->meshFile);
	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;
	if (loader.ReadMeshData(vertices, indices))
	{
		if (entity->skinned)
		{
			std::vector<VertexBoneData> boneData;
			if (loader.ReadBoneData(boneData, skinningData))
			{
				mesh = ModelManager::CreateMesh(vertices, indices, boneData);
				LogInfo("ModelLibrary::LoadEntity(): Loaded '{}' in '{}'", name, this->name);
			}
			else
			{
				mesh = ModelManager::BLANK_MODEL->mesh;
				LogError("ModelLibrary::LoadEntity(): Failed loading mesh for '{}' in '{}'", name, this->name);
			}
		}
		else
		{
			mesh = ModelManager::CreateMesh(vertices, indices);
			LogInfo("ModelLibrary::LoadEntity(): Loaded '{}' in '{}'", name, this->name);
		}
	}
	else
	{
		mesh = ModelManager::BLANK_MODEL->mesh;
		LogError("ModelLibrary::LoadEntity(): Failed loading mesh for '{}' in '{}'", name, this->name);
	}
	
	material = entity->materialName == "" ? MaterialManager::BLANK_MATERIAL : materialLibrary->GetMaterial(entity->materialName);

	for (auto& ae : entity->animations)
	{
		std::shared_ptr<AnimationClip> animation;
		MeshFileLoader animFile(ae.clip);
		if (animFile.ReadAnimation(animation))
		{
			LogInfo("ModelLibrary::LoadEntity(): Loaded '{}' animation for '{}' in '{}'", ae.name, name, this->name);
		}
		else
		{
			animation = AnimationClip::Create(1.0f, 1.0f);
			LogError("ModelLibrary::LoadEntity(): Failed loading animation '{}' for '{}' in '{}'", ae.name, name, this->name);
		}

		animations[ae.name] = animation;
	}

	std::shared_ptr<Model> ptr(new Model(mesh,material,skinningData, animations));

	entity->model = ptr;
	ModelManager::CacheHolder.push_back(ptr);
	return ptr;
}

std::shared_ptr<Model> ModelLibrary::GetModel(std::string name)
{
	if (this->name == "GENERIC")
		return ModelManager::BLANK_MODEL;
	try
	{
		LibraryEntity* entity = entities.at(name);
		if (entity->model.expired())
			return LoadEntity(name, entity);
		else
			return entity->model.lock();
	}
	catch (std::out_of_range ex)
	{
		LogWarning("ModelLibrary::GetModel(): Cannot find model '{}' in '{}' library", name, this->name);
		return ModelManager::BLANK_MODEL;
	}
}