#pragma once

#include "Logger.h"
#include "Material.h"

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
}

Material::Material() {
	albedoMap = MaterialManager::BLANK_ALBEDO_MAP;
	normalMap = MaterialManager::BLANK_NORMAL_MAP;
	metalnessMap = MaterialManager::BLANK_METALNESS_MAP;
	roughnessMap = MaterialManager::BLANK_ROUGHNESS_MAP;
	emissiveMap = MaterialManager::BLANK_EMISSIVE_MAP;

	albedoColor = {0.85f, 0.85f, 0.85f};
	metalnessValue = 0.5f;
	roughnessValue = 0.5f;
	emissiveColor = {0.0f, 0.0f, 0.0f};
}

void Material::SetLibraryProperties(std::map<std::string, std::string>& properties, std::shared_ptr<TextureLibrary> textureLibrary)
{
	std::map<std::string, std::string>::iterator val;

#ifdef _DEBUG
	std::map<std::string, std::string> debugProperties = properties;
#endif

	if ((val = properties.find("albedoMap")) != properties.end())
	{
		albedoMap = textureLibrary->GetTexture(val->second);
		albedoColor = { 1.0f, 1.0f, 1.0f };
		debugOnly(debugProperties.erase(val->first));
	}

	if ((val = properties.find("normalMap")) != properties.end())
	{
		normalMap = textureLibrary->GetTexture(val->second);
		debugOnly(debugProperties.erase(val->first));
	}

	if ((val = properties.find("metalnessMap")) != properties.end())
	{
		metalnessMap = textureLibrary->GetTexture(val->second);
		metalnessValue = 1.0f;
		debugOnly(debugProperties.erase(val->first));
	}

	if ((val = properties.find("roughnessMap")) != properties.end())
	{
		roughnessMap = textureLibrary->GetTexture(val->second);
		roughnessValue = 1.0f;
		debugOnly(debugProperties.erase(val->first));
	}

	if ((val = properties.find("emissiveMap")) != properties.end())
	{
		emissiveMap = textureLibrary->GetTexture(val->second);
		emissiveColor = { 1.0f, 1.0f, 1.0f };
		debugOnly(debugProperties.erase(val->first));
	}

	// TODO: More properties

#ifdef _DEBUG
	for (auto leftover : debugProperties)
	{
		LogWarning("Material::SetLibraryProperties(): Unknown property: '{}' = '{}'", leftover.first, leftover.second);
	}
#endif

}

void Material::apply()
{
	albedoMap->bind(MaterialBindingPoint::ALBEDO_MAP);
	normalMap->bind(MaterialBindingPoint::NORMAL_MAP);
	metalnessMap->bind(MaterialBindingPoint::METALNESS_MAP);
	roughnessMap->bind(MaterialBindingPoint::ROUGHNESS_MAP);
	emissiveMap->bind(MaterialBindingPoint::EMISSIVE_MAP);
}