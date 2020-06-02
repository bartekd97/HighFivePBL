#pragma once
#include <typeinfo>
#include "PrefabManager.h"

class Prefab
{
	friend void PrefabManager::Initialize();
	friend std::shared_ptr<Prefab> PrefabManager::GetPrefab(std::string name);

public:
	typedef std::vector<std::shared_ptr<IPrefabComponentLoader>> PrefabComponents;
	struct PrefabChild
	{
		std::string name;
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
		PrefabComponents components;
		std::vector<PrefabChild> children;
	};

public:
	const std::string name;

private:
	glm::vec3 defaultScale;
	PrefabComponents components;
	std::vector<PrefabChild> children;

	PropertyReader properties;

	Prefab(
		std::string& name,
		glm::vec3& defaultScale,
		PrefabComponents& components,
		std::vector<PrefabChild>& children,
		PropertyReader& properties
		) :
		name(name),
		defaultScale(defaultScale),
		components(components),
		children(children),
		properties(properties)
	{}

public:
	GameObject Instantiate(glm::vec3 position = { 0,0,0 }, glm::vec3 rotation = { 0,0,0 }, glm::vec3 scale = { 1,1,1 });
	GameObject Instantiate(GameObject parent, glm::vec3 position = { 0,0,0 }, glm::vec3 rotation = { 0,0,0 }, glm::vec3 scale = { 1,1,1 });


public:
	inline PropertyReader& Properties() { return properties; }
};