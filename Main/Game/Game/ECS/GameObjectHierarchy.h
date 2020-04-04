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
private:
	std::vector<HierarchyNode> nodes;
	std::array<std::shared_ptr<HierarchyNode>, MAX_GAMEOBJECTS> pointers{};
};
