#pragma once

#include <glm/glm.hpp>
#include "Texture.h"
#include "MaterialManager.h"
#include "Shader.h"
#include "Utility/PropertyReader.h"

namespace MaterialBindingPoint {
	extern const int ALBEDO_MAP;
	extern const int NORMAL_MAP;
	extern const int METALNESS_MAP;
	extern const int ROUGHNESS_MAP;
	extern const int EMISSIVE_MAP;

	void AssignToShader(std::shared_ptr<Shader> shader);
}

enum class MaterialType {
	DEFERRED,
	FORWARD
};
class Material {
	friend void MaterialManager::Initialize();
	friend std::shared_ptr<Material> MaterialManager::CreateEmptyMaterial(MaterialType type);
	friend std::shared_ptr<Material> MaterialManager::CloneMaterial(const std::shared_ptr<Material>& source);
	friend class MaterialLibrary;

public:

	std::shared_ptr<Texture> albedoMap;
	std::shared_ptr<Texture> normalMap;
	std::shared_ptr<Texture> metalnessMap; // only in deferred
	std::shared_ptr<Texture> roughnessMap; // only in deferred
	std::shared_ptr<Texture> emissiveMap;

	glm::vec3 albedoColor;
	float metalnessValue; // only in deferred
	float roughnessValue; // only in deferred
	glm::vec3 emissiveColor;

	float specularValue; // only in forward
	float opacityValue; // only in forward

	const MaterialType type;

private:
	void SetLibraryProperties(PropertyReader& properties, std::shared_ptr<TextureLibrary> textureLibrary);


	Material(MaterialType materialType);
	Material(const std::shared_ptr<Material>& base);

public:
	void apply(std::shared_ptr<Shader> shader);
	inline void forceApply(std::shared_ptr<Shader> shader) {
		NoApply(shader);
		apply(shader);
	}
	static void NoApply(std::shared_ptr<Shader> shader);
};