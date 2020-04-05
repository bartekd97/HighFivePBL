#pragma once

#include <array>
#include <memory>

#include "HierarchyNode.h"

class GameObjectHierarchy
{
public:
	void AddGameObject(GameObject child, GameObject parent);
	void RemoveGameObject(GameObject gameObject);
	std::vector<GameObject> GetChildren(GameObject parent);
	std::optional<GameObject> GetParent(GameObject child);
	std::vector<GameObject> GetRoot();
private:
	std::vector<GameObject> root;
	std::array<std::shared_ptr<HierarchyNode>, MAX_GAMEOBJECTS> pointers{};
};
