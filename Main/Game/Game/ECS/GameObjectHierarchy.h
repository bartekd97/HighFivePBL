#pragma once

#include <array>
#include <memory>

#include "HierarchyNode.h"

class GameObjectHierarchy
{
public:
	void AddGameObject(GameObject child, GameObject parent);
	void RemoveGameObject(GameObject gameObject);
	std::vector<GameObject>& GetChildren(GameObject parent);
	std::vector<GameObject> GetByNameInChildren(GameObject parent, std::string name);

	/*template<typename T>
	tsl::robin_set<GameObject>& GetChildrenWithComponent(GameObject parent, bool countParent = false)
	{

	}*/

	std::optional<GameObject> GetParent(GameObject child);
	std::vector<GameObject>& GetRoot();
	bool IsPresent(GameObject gameObject);
private:
	std::vector<GameObject> root;
	std::array<std::shared_ptr<HierarchyNode>, MAX_GAMEOBJECTS> pointers{};

	void GetByNameInChildren(GameObject parent, std::string name, std::vector<GameObject>& result);
};
