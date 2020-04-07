#include <filesystem>
#include <tinyxml2.h>
#include <stb_image.h>
#include "Global.h"
#include "../Utility/Logger.h"
#include "../Utility/Utility.h"
#include "Texture.h"
#include "Material.h"

using namespace tinyxml2;

// variables
namespace MaterialManager {
	std::shared_ptr<MaterialLibrary> GENERIC_LIBRARY;
	std::shared_ptr<Material> BLANK_MATERIAL;

	std::shared_ptr<Texture> BLANK_ALBEDO_MAP; // 4x4 white (255) srgb texture
	std::shared_ptr<Texture> BLANK_NORMAL_MAP; // 4x4 flat normal (127,127,255) rgb texture
	std::shared_ptr<Texture> BLANK_METALNESS_MAP; // 4x4 white (255) r texture
	std::shared_ptr<Texture> BLANK_ROUGHNESS_MAP; // 4x4 white (255) r texture
	std::shared_ptr<Texture> BLANK_EMISSIVE_MAP; // 4x4 white (255) srgb texture

#pragma region Blank Maps Data

	unsigned char* BLANK_ALBEDO_MAP_DATA = new unsigned char[4 * 4 * 3]{
		255,255,255, 255,255,255, 255,255,255, 255,255,255,
		255,255,255, 255,255,255, 255,255,255, 255,255,255,
		255,255,255, 255,255,255, 255,255,255, 255,255,255,
		255,255,255, 255,255,255, 255,255,255, 255,255,255,
	};
	unsigned char* BLANK_NORMAL_MAP_DATA = new unsigned char[4 * 4 * 3]{
		127,127,255, 127,127,255, 127,127,255, 127,127,255,
		127,127,255, 127,127,255, 127,127,255, 127,127,255,
		127,127,255, 127,127,255, 127,127,255, 127,127,255,
		127,127,255, 127,127,255, 127,127,255, 127,127,255,
	};
	unsigned char* BLANK_METALNESS_MAP_DATA = new unsigned char[4 * 4 * 1]{
		255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255
	};
	unsigned char* BLANK_ROUGHNESS_MAP_DATA = new unsigned char[4 * 4 * 1]{
		255,255,255,255, 255,255,255,255, 255,255,255,255, 255,255,255,255
	};
	unsigned char* BLANK_EMISSIVE_MAP_DATA = new unsigned char[4 * 4 * 3]{
		255,255,255, 255,255,255, 255,255,255, 255,255,255,
		255,255,255, 255,255,255, 255,255,255, 255,255,255,
		255,255,255, 255,255,255, 255,255,255, 255,255,255,
		255,255,255, 255,255,255, 255,255,255, 255,255,255,
	};

#pragma endregion

	bool Initialized = false;

	std::unordered_map<std::string, std::shared_ptr<MaterialLibrary>> LibraryContainer;
}


// publics
void MaterialManager::Initialize()
{
	if (Initialized)
	{
		LogWarning("MaterialManager::Initialize(): Already initialized");
		return;
	}

	GENERIC_LIBRARY = std::shared_ptr<MaterialLibrary>(new MaterialLibrary());

	TextureConfig blankMapConfig;
	blankMapConfig.format = GL_SRGB8;
	BLANK_ALBEDO_MAP = TextureManager::CreateTextureFromRawData(BLANK_ALBEDO_MAP_DATA, 4, 4, GL_RGB, blankMapConfig);
	blankMapConfig.format = GL_RGB;
	BLANK_NORMAL_MAP = TextureManager::CreateTextureFromRawData(BLANK_NORMAL_MAP_DATA, 4, 4, GL_RGB, blankMapConfig);
	blankMapConfig.format = GL_R8;
	BLANK_METALNESS_MAP = TextureManager::CreateTextureFromRawData(BLANK_METALNESS_MAP_DATA, 4, 4, GL_RED, blankMapConfig);
	blankMapConfig.format = GL_R8;
	BLANK_ROUGHNESS_MAP = TextureManager::CreateTextureFromRawData(BLANK_ROUGHNESS_MAP_DATA, 4, 4, GL_RED, blankMapConfig);
	blankMapConfig.format = GL_SRGB8;
	BLANK_EMISSIVE_MAP = TextureManager::CreateTextureFromRawData(BLANK_EMISSIVE_MAP_DATA, 4, 4, GL_RGB, blankMapConfig);

	BLANK_MATERIAL = CreateEmptyMaterial();

	Initialized = true;

	LogInfo("MaterialManager initialized.");
}

std::shared_ptr<Material> MaterialManager::CreateEmptyMaterial()
{
	return std::shared_ptr<Material>(new Material());
}

std::shared_ptr<MaterialLibrary> MaterialManager::GetLibrary(std::string name)
{
#ifdef _DEBUG
	for (auto lib : LibraryContainer)
	{
		const char* key = lib.first.c_str();
		if (_stricmp(key, name.c_str()) == 0 &&
			strcmp(key, name.c_str()) != 0)
		{
			LogWarning("MaterialLibrary::GetLibrary(): Trying to get duplciate of '{}' library but with different letter size. It causes dupliacate and increased memory usage!!!", name);
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
			LibraryContainer[name] = std::shared_ptr<MaterialLibrary>(new MaterialLibrary(name));
			return LibraryContainer[name];
		}
		else
		{
			LogWarning("MaterialManager::GetLibrary(): Cannot find library '{}'", name);
			return GENERIC_LIBRARY;
		}
	}
}

std::shared_ptr<Material> MaterialManager::GetMaterial(std::string libraryName, std::string textureName)
{
	return MaterialManager::GetLibrary(libraryName)->GetMaterial(textureName);
}


// classes

MaterialLibrary::MaterialLibrary() : name("GENERIC")
{}
MaterialLibrary::MaterialLibrary(std::string name) : name(name)
{
	const std::string libraryFolder = Global::ASSETS_DIRECTORY + name + "/";

	XMLDocument doc;
	auto loaded = doc.LoadFile((libraryFolder + "materials.xml").c_str());
	if (loaded != XML_SUCCESS)
	{
		LogError("MaterialLibrary::MaterialLibrary(): Cannot load materials.xml for '{}'. Code: {}", name, loaded);
		return;
	}

	auto root = doc.FirstChildElement();
	if (root == nullptr)
	{
		LogError("MaterialLibrary::MaterialLibrary(): Cannot find root element in materials.xml for '{}'", name);
		return;
	}
	
	std::string textureLibraryName = nullableString(root->Attribute("textureLibrary"));
	if (textureLibraryName.size() > 0)
	{
		if (textureLibraryName == "this")
			textureLibraryName = name;
		textureLibrary = TextureManager::GetLibrary(textureLibraryName);
		if (textureLibrary->name == "GENERIC")
		{
			LogWarning("MaterialLibrary::MaterialLibrary(): Cannot find '{}' texture library for '{}' material library", textureLibraryName, name);
		}
	}
	else
	{
		textureLibrary = TextureManager::GENERIC_LIBRARY;
	}
	

	int i = 0;
	for (XMLElement* node = root->FirstChildElement("material");
		node != nullptr;
		node = node->NextSiblingElement("material"), i++)
	{
		const char* materialName = node->Attribute("name");

		if (materialName == nullptr)
		{
			LogWarning("MaterialLibrary::MaterialLibrary(): Invalid material node at #{}", i);
			continue;
		}

		int j = 0;
		LibraryEntity* entity = new LibraryEntity();
		for (XMLElement* property = node->FirstChildElement("property");
			property != nullptr;
			property = property->NextSiblingElement("property"), j++)
		{
			const char* propertyName = property->Attribute("name");
			const char* propertyValue = property->Attribute("value");

			if (propertyName == nullptr || propertyValue == nullptr)
			{
				LogWarning("MaterialLibrary::MaterialLibrary(): Invalid property node at #{} for '{}'", j, materialName);
				continue;
			}

			entity->properties[propertyName] = propertyValue;
		}

		entities[materialName] = entity;
	}
	LogInfo("MaterialLibrary::MaterialLibrary(): Initialized '{}' library with {} entities", name, entities.size());
}

MaterialLibrary::~MaterialLibrary()
{
	for (auto e : entities)
		delete e.second;
}

std::shared_ptr<Material> MaterialLibrary::LoadEntity(std::string& name, LibraryEntity* entity)
{
	auto ptr = MaterialManager::CreateEmptyMaterial();
	ptr->SetLibraryProperties(entity->properties, textureLibrary);
	entity->material = ptr;
	LogInfo("MaterialLibrary::LoadEntity(): Loaded '{}' in '{}'", name, this->name);
	return ptr;
}

std::shared_ptr<Material> MaterialLibrary::GetMaterial(std::string name)
{
	if (this->name == "GENERIC")
		return MaterialManager::BLANK_MATERIAL;
	try
	{
		LibraryEntity* entity = entities.at(name);
		if (entity->material.expired())
			return LoadEntity(name, entity);
		else
			return entity->material.lock();
	}
	catch (std::out_of_range ex)
	{
		LogWarning("MaterialLibrary::GetMaterial(): Cannot find material '{}' in '{}' library", name, this->name);
		return MaterialManager::BLANK_MATERIAL;
	}
}