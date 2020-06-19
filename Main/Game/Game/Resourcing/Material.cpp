#pragma once

#include <tsl/robin_map.h>
#include "../Utility/Logger.h"
#include "Material.h"
#include "Shader.h"
#include "Utility/Utility.h"

#ifdef _DEBUG
#define debugOnly(expression) expression
#else
#define debugOnly(expression)
#endif

namespace MaterialBindingPoint {
	const int ALBEDO_MAP = 1;
	const int NORMAL_MAP = 2;
	const int METALNESS_MAP = 3;
	const int ROUGHNESS_MAP = 4;
	const int EMISSIVE_MAP = 5;

	void AssignToShader(std::shared_ptr<Shader> shader)
	{
		shader->setInt("albedoMap", ALBEDO_MAP);
		shader->setInt("normalMap", NORMAL_MAP);
		shader->setInt("metalnessMap", METALNESS_MAP);
		shader->setInt("roughnessMap", ROUGHNESS_MAP);
		shader->setInt("emissiveMap", EMISSIVE_MAP);
	}
}

Material::Material(MaterialType type) : type(type) {
	albedoMap = MaterialManager::BLANK_ALBEDO_MAP;
	normalMap = MaterialManager::BLANK_NORMAL_MAP;
	metalnessMap = MaterialManager::BLANK_METALNESS_MAP;
	roughnessMap = MaterialManager::BLANK_ROUGHNESS_MAP;
	emissiveMap = MaterialManager::BLANK_EMISSIVE_MAP;

	albedoColor = {0.85f, 0.85f, 0.85f};
	metalnessValue = 0.5f;
	roughnessValue = 0.5f;
	emissiveColor = {0.0f, 0.0f, 0.0f};

	specularValue = 0.5f;
	opacityValue = 1.0f;
}

Material::Material(const std::shared_ptr<Material>& base) : type(base->type) {
	albedoMap = base->albedoMap;
	normalMap = base->normalMap;
	metalnessMap = base->metalnessMap;
	roughnessMap = base->roughnessMap;
	emissiveMap = base->emissiveMap;

	albedoColor = base->albedoColor;
	metalnessValue = base->metalnessValue;
	roughnessValue = base->roughnessValue;
	emissiveColor = base->emissiveColor;

	specularValue = base->specularValue;
	opacityValue = base->opacityValue;
}

void Material::SetLibraryProperties(PropertyReader& properties, std::shared_ptr<TextureLibrary> textureLibrary)
{
	static std::string albedoMapTmp;
	if (properties.GetString("albedoMap", albedoMapTmp, ""))
	{
		albedoMap = textureLibrary->GetTexture(albedoMapTmp);
		albedoColor = { 1.0f, 1.0f, 1.0f };
	}
	properties.GetVec3("albedoColor", albedoColor, albedoColor);

	static std::string normalMapTmp;
	if (properties.GetString("normalMap", normalMapTmp, ""))
		normalMap = textureLibrary->GetTexture(normalMapTmp);

	static std::string metalnessMapTmp;
	if (properties.GetString("metalnessMap", metalnessMapTmp, ""))
	{
		metalnessMap = textureLibrary->GetTexture(metalnessMapTmp);
		metalnessValue = 1.0f;
	}
	properties.GetFloat("metalnessValue", metalnessValue, metalnessValue);

	static std::string roughnessMapTmp;
	if (properties.GetString("roughnessMap", roughnessMapTmp, ""))
	{
		roughnessMap = textureLibrary->GetTexture(roughnessMapTmp);
		roughnessValue = 1.0f;
	}
	properties.GetFloat("roughnessValue", roughnessValue, roughnessValue);

	static std::string emissiveMapTmp;
	if (properties.GetString("emissiveMap", emissiveMapTmp, ""))
	{
		emissiveMap = textureLibrary->GetTexture(emissiveMapTmp);
		emissiveColor = { 1.0f, 1.0f, 1.0f };
	}
	properties.GetVec3("emissiveColor", emissiveColor, emissiveColor);


	properties.GetFloat("specularValue", specularValue, specularValue);
	properties.GetFloat("opacityValue", opacityValue, opacityValue);


#ifdef _DEBUG
	auto debugProperties = properties.GetRawCopy();
	debugProperties.erase("albedoMap");
	debugProperties.erase("albedoColor");
	debugProperties.erase("normalMap");
	debugProperties.erase("metalnessMap");
	debugProperties.erase("metalnessValue");
	debugProperties.erase("roughnessMap");
	debugProperties.erase("roughnessValue");
	debugProperties.erase("emissiveMap");
	debugProperties.erase("emissiveColor");
	debugProperties.erase("specularValue");
	debugProperties.erase("opacityValue");
	for (auto leftover : debugProperties)
	{
		LogWarning("Material::SetLibraryProperties(): Unknown property: '{}' = '{}'", leftover.first, leftover.second);
	}
#endif
}

static tsl::robin_map<Shader*, Material*> LastBoundMaterials;
void Material::apply(std::shared_ptr<Shader> shader)
{
	if (LastBoundMaterials[shader.get()] == this) return;
	LastBoundMaterials[shader.get()] = this;

	// bind maps
	albedoMap->bind(MaterialBindingPoint::ALBEDO_MAP);
	normalMap->bind(MaterialBindingPoint::NORMAL_MAP);
	if (type == MaterialType::DEFERRED)
	{
		metalnessMap->bind(MaterialBindingPoint::METALNESS_MAP);
		roughnessMap->bind(MaterialBindingPoint::ROUGHNESS_MAP);
	}
	emissiveMap->bind(MaterialBindingPoint::EMISSIVE_MAP);

	// assign values
	shader->setVector3F("albedoColor", albedoColor);
	if (type == MaterialType::DEFERRED)
	{
		shader->setFloat("roughnessValue", roughnessValue);
		shader->setFloat("metalnessValue", metalnessValue);
	}
	shader->setVector3F("emissiveColor", emissiveColor);

	if (type == MaterialType::FORWARD)
	{
		shader->setFloat("specularValue", specularValue);
		shader->setFloat("opacityValue", opacityValue);
	}
}

void Material::NoApply(std::shared_ptr<Shader> shader)
{
	LastBoundMaterials[shader.get()] = NULL;
}
