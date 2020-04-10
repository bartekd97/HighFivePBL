#pragma once

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

void Material::SetLibraryProperties(std::unordered_map<std::string, std::string>& properties, std::shared_ptr<TextureLibrary> textureLibrary)
{
	std::unordered_map<std::string, std::string>::iterator val;

#ifdef _DEBUG
	std::unordered_map<std::string, std::string> debugProperties = properties;
#endif

	if ((val = properties.find("albedoMap")) != properties.end())
	{
		albedoMap = textureLibrary->GetTexture(val->second);
		albedoColor = { 1.0f, 1.0f, 1.0f };
		debugOnly(debugProperties.erase(val->first));
	}
	if ((val = properties.find("albedoColor")) != properties.end())
	{
		if (!Utility::TryConvertStringToVec3((val->second), albedoColor))
		{
			LogWarning("Material::SetLibraryProperties(): Cannot parse albedoColor: '{}'", val->second);
		}
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
	if ((val = properties.find("metalnessValue")) != properties.end())
	{
		if (!Utility::TryConvertStringToFloat((val->second), metalnessValue))
		{
			LogWarning("Material::SetLibraryProperties(): Cannot parse metalnessValue: '{}'", val->second);
		}
		debugOnly(debugProperties.erase(val->first));
	}

	if ((val = properties.find("roughnessMap")) != properties.end())
	{
		roughnessMap = textureLibrary->GetTexture(val->second);
		roughnessValue = 1.0f;
		debugOnly(debugProperties.erase(val->first));
	}
	if ((val = properties.find("roughnessValue")) != properties.end())
	{
		if (!Utility::TryConvertStringToFloat((val->second), roughnessValue))
		{
			LogWarning("Material::SetLibraryProperties(): Cannot parse roughnessValue: '{}'", val->second);
		}
		debugOnly(debugProperties.erase(val->first));
	}

	if ((val = properties.find("emissiveMap")) != properties.end())
	{
		emissiveMap = textureLibrary->GetTexture(val->second);
		emissiveColor = { 1.0f, 1.0f, 1.0f };
		debugOnly(debugProperties.erase(val->first));
	}
	if ((val = properties.find("emissiveColor")) != properties.end())
	{
		if (!Utility::TryConvertStringToVec3((val->second), emissiveColor))
		{
			LogWarning("Material::SetLibraryProperties(): Cannot parse emissiveColor: '{}'", val->second);
		}
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

void Material::apply(std::shared_ptr<Shader> shader)
{
	// bind maps
	albedoMap->bind(MaterialBindingPoint::ALBEDO_MAP);
	normalMap->bind(MaterialBindingPoint::NORMAL_MAP);
	metalnessMap->bind(MaterialBindingPoint::METALNESS_MAP);
	roughnessMap->bind(MaterialBindingPoint::ROUGHNESS_MAP);
	emissiveMap->bind(MaterialBindingPoint::EMISSIVE_MAP);

	// assign values
	shader->setVector3F("albedoColor", albedoColor);
	shader->setFloat("roughnessValue", roughnessValue);
	shader->setFloat("metalnessValue", metalnessValue);
	shader->setVector3F("emissiveColor", emissiveColor);
}