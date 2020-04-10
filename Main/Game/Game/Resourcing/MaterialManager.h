#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <glad/glad.h>
#include "Utility/PropertyReader.h"

class Material;
class MaterialLibrary;
class TextureLibrary;


namespace MaterialManager {
	void Initialize();

	extern std::shared_ptr<MaterialLibrary> GENERIC_LIBRARY;
	extern std::shared_ptr<Material> BLANK_MATERIAL;

	extern std::shared_ptr<Texture> BLANK_ALBEDO_MAP; // 4x4 white (255) srgb texture
	extern std::shared_ptr<Texture> BLANK_NORMAL_MAP; // 4x4 flat normal (127,127,255) rgb texture
	extern std::shared_ptr<Texture> BLANK_METALNESS_MAP; // 4x4 gray (127) r texture
	extern std::shared_ptr<Texture> BLANK_ROUGHNESS_MAP; // 4x4 gray (127) r texture
	extern std::shared_ptr<Texture> BLANK_EMISSIVE_MAP; // 4x4 black (0) srgb texture

	std::shared_ptr<Material> CreateEmptyMaterial();
	std::shared_ptr<MaterialLibrary> GetLibrary(std::string name);
	std::shared_ptr<Material> GetMaterial(std::string libraryName, std::string materialName);
}


class MaterialLibrary {
	friend void MaterialManager::Initialize();
	friend std::shared_ptr<MaterialLibrary> MaterialManager::GetLibrary(std::string name);

public:
	const std::string name;

private:
	struct LibraryEntity {
		std::weak_ptr<Material> material;
		//std::unordered_map<std::string, std::string> properties;
		PropertyReader properties;
	};
	std::unordered_map<std::string, LibraryEntity*> entities;

	std::shared_ptr<TextureLibrary> textureLibrary;

	MaterialLibrary();
	MaterialLibrary(std::string name);
	std::shared_ptr<Material> LoadEntity(std::string& name, LibraryEntity* entity);
public:
	~MaterialLibrary();
	std::shared_ptr<Material> GetMaterial(std::string name);
};