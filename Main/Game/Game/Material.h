#pragma once

#include <glm/glm.hpp>
#include "Texture.h"
#include "MaterialManager.h"

namespace MaterialBindingPoint {
	extern const int ALBEDO_MAP;
	extern const int NORMAL_MAP;
	extern const int METALNESS_MAP;
	extern const int ROUGHNESS_MAP;
	extern const int EMISSIVE_MAP;
}

class Material {
	friend void MaterialManager::Initialize();
	friend std::shared_ptr<Material> MaterialManager::CreateEmptyMaterial();
	friend class MaterialLibrary;

public:
	std::shared_ptr<Texture> albedoMap;
	std::shared_ptr<Texture> normalMap;
	std::shared_ptr<Texture> metalnessMap;
	std::shared_ptr<Texture> roughnessMap;
	std::shared_ptr<Texture> emissiveMap;

	glm::vec3 albedoColor;
	float metalnessValue;
	float roughnessValue;
	glm::vec3 emissiveColor;

private:
	void SetLibraryProperties(std::map<std::string, std::string>& properties, std::shared_ptr<TextureLibrary> textureLibrary);

	Material();

public:
	void apply();

};