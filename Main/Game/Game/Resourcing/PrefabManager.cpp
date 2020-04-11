#include <filesystem>
#include <unordered_map>
#include <tinyxml2.h>
#include "../Global.h"
#include "../Utility/Logger.h"
#include "Prefab.h"
#include "PrefabComponentLoader/PrefabComponentLoader.h"

using namespace tinyxml2;

// variables
namespace PrefabManager {
	std::string PREFABS_PATH = "Data/Prefabs/";

	std::shared_ptr<Prefab> GENERIC_PREFAB;

	std::unordered_map<std::string, std::shared_ptr<Prefab>> PrefabContainer;
	std::unordered_map<std::string, std::function<std::shared_ptr<IPrefabComponentLoader>()>> ComponentLoaderProviderContainer;

	bool Initialized = false;
}

// privates
namespace PrefabManager {
	struct LoadedPrefabInfo
	{
		glm::vec3 defaultScale;
		Prefab::PrefabComponents components;
		std::vector<Prefab::PrefabChild> children;
		PropertyReader properties;
	};


	void ReadPrefabComponents(XMLElement* root, Prefab::PrefabComponents& components)
	{
		int i = 0;
		for (XMLElement* node = root->FirstChildElement("component");
			node != nullptr;
			node = node->NextSiblingElement("component"), i++)
		{
			const char* componentName = node->Attribute("name");

			if (componentName == nullptr)
			{
				LogWarning("PrefabManager::ReadPrefabComponents(): Invalid component node at #{}", i);
				continue;
			}

			auto provider = ComponentLoaderProviderContainer[componentName];
			if (!provider)
			{
				LogWarning("PrefabManager::ReadPrefabComponents(): Unregistered loader for '{}' component", componentName);
				continue;
			}

			auto loader = provider();

			PropertyReader properties(node);

			loader->Preprocess(properties);
			components.push_back(loader);
		}
	}
	void ReadPrefabChildren(XMLElement* root, std::vector<Prefab::PrefabChild>& children)
	{
		XMLElement* childRoot = root->FirstChildElement("children");
		if (childRoot == nullptr) return;

		int i = 0;
		for (XMLElement* node = childRoot->FirstChildElement("child");
			node != nullptr;
			node = node->NextSiblingElement("child"), i++)
		{

			const char* childName = node->Attribute("name");
			if (childName == nullptr)
			{
				LogWarning("PrefabManager::ReadPrefabChildren(): Child #{} name not specified. Using 'GameObject' instead", i);
				childName = "GameObject";
			}

			std::string position = nullableString(node->Attribute("position"));
			std::string rotation = nullableString(node->Attribute("rotation"));
			std::string scale = nullableString(node->Attribute("scale"));

			Prefab::PrefabChild child;
			child.name = childName;

			if (!Utility::TryConvertStringToVec3(position, child.position))
			{
				LogWarning("PrefabManager::ReadPrefabChildren(): Child #{}: Cannot parse position. Using '0,0,0' instead", i);
				child.position = { 0.0f, 0.0f, 0.0f };
			}
			if (!Utility::TryConvertStringToVec3(rotation, child.rotation))
			{
				LogWarning("PrefabManager::ReadPrefabChildren(): Child #{}: Cannot parse rotation. Using '0,0,0' instead", i);
				child.rotation = { 0.0f, 0.0f, 0.0f };
			}
			if (!Utility::TryConvertStringToVec3(scale, child.scale))
			{
				LogWarning("PrefabManager::ReadPrefabChildren(): Child #{}: Cannot parse scale. Using '1,1,1' instead", i);
				child.scale = { 1.0f, 1.0f, 1.0f };
			}

			ReadPrefabComponents(node, child.components);
			ReadPrefabChildren(node, child.children);

			children.push_back(child);
		}
	}

	bool TryLoadPrefab(std::string name, LoadedPrefabInfo& info)
	{
		XMLDocument doc;
		auto loaded = doc.LoadFile((PREFABS_PATH + name + ".xml").c_str());
		if (loaded != XML_SUCCESS)
		{
			LogError("PrefabManager::TryLoadPrefab(): Cannot load xml for prefab file. Code: {}", loaded);
			return false;
		}

		auto root = doc.FirstChildElement();
		if (root == nullptr)
		{
			LogError("PrefabManager::TryLoadPrefab(): Cannot find root element in XML file.");
			return false;
		}

		std::string defaultScale = nullableString(root->Attribute("defaultScale"));
		if (defaultScale == "")
			info.defaultScale = { 1.0f, 1.0f, 1.0f };
		else if (!Utility::TryConvertStringToVec3(defaultScale, info.defaultScale))
		{
			LogWarning("PrefabManager::TryLoadPrefab(): Cannot parse default scale for '{}'. Using '1,1,1' instead.", name);
			info.defaultScale = { 1.0f, 1.0f, 1.0f };
		}

		info.properties = PropertyReader(root);
		ReadPrefabComponents(root, info.components);
		ReadPrefabChildren(root, info.children);

		return true;
	}
}

// publics
void PrefabManager::Initialize()
{
	if (Initialized)
	{
		LogWarning("PrefabManager::Initialize(): Already initialized");
		return;
	}

	std::string dName = "GENERIC";
	glm::vec3 dScale = { 1.0f, 1.0f, 1.0f };
	Prefab::PrefabComponents dComponents;
	std::vector<Prefab::PrefabChild> dChildren;
	PropertyReader properties;
	GENERIC_PREFAB = std::shared_ptr<Prefab>(new Prefab(dName,dScale,dComponents,dChildren,properties));

	PrefabComponentLoader::RegisterLoaders();

	Initialized = true;

	LogInfo("PrefabManager initialized.");
}

std::shared_ptr<Prefab> PrefabManager::GetPrefab(std::string name)
{
	try
	{
		return PrefabContainer.at(name);
	}
	catch (std::out_of_range ex)
	{
		LoadedPrefabInfo info;
		std::shared_ptr<Prefab> ptr;
		if (TryLoadPrefab(name, info))
		{
			auto pos = name.find_last_of("/");
			auto prefabName = name;
			if (pos != std::string::npos)
				prefabName = name.substr(pos+1);
			ptr = std::shared_ptr<Prefab>(new Prefab(prefabName, info.defaultScale, info.components, info.children, info.properties));
			LogInfo("PrefabManager::GetPrefab(): Loaded '{}'", name);
		}
		else
		{
			ptr = GENERIC_PREFAB;
			LogError("PrefabManager::GetPrefab(): Failed loading '{}'", name);
		}
		PrefabContainer[name] = ptr;
		return ptr;
	}
}

void PrefabManager::RegisterComponentLoader(std::string name, std::function<std::shared_ptr<IPrefabComponentLoader>()> provider)
{
	if (ComponentLoaderProviderContainer.contains(name))
	{
		LogWarning("PrefabManager::RegisterComponentLoader(): Loader for '{}' already registered.", name);
		return;
	}
	ComponentLoaderProviderContainer[name] = provider;
}

