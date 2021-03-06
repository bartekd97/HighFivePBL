#include <algorithm>

#include "GameObjectHierarchy.h"
#include  "../HFEngine.h"

#include "../Utility/Logger.h"

void GameObjectHierarchy::AddGameObject(GameObject child, GameObject parent)
{
	auto parentNode = pointers[parent];
	if (parentNode == nullptr)
	{
		root.push_back(parent);
		parentNode = std::make_shared<HierarchyNode>(parent);
		pointers[parent] = parentNode;
	}
	parentNode->children.push_back(child);
	pointers[child] = std::make_shared<HierarchyNode>(parentNode->children.back(), parent);
}

void GameObjectHierarchy::RemoveGameObject(GameObject gameObject)
{
	auto node = pointers[gameObject];
	if (node != nullptr)
	{
		for (int i = node->children.size() - 1; i >= 0; i--)
		{
			RemoveGameObject(node->children[i]);
		}
		if (!node->parent.has_value())
		{
			root.erase(std::remove(root.begin(), root.end(), gameObject), root.end());
		}
		else
		{
			auto parentNode = pointers[node->parent.value()];
			parentNode->children.erase(std::remove(parentNode->children.begin(), parentNode->children.end(), gameObject), parentNode->children.end());
		}
		pointers[gameObject] = nullptr;
	}
}

std::vector<GameObject>& GameObjectHierarchy::GetChildren(GameObject parent)
{
	auto node = pointers[parent];
	if (node != nullptr)
	{
		return node->children;
	}
	static std::vector<GameObject> _empty;
	return _empty;
}

std::vector<GameObject> GameObjectHierarchy::GetByNameInChildren(GameObject parent, std::string name)
{
	auto node = pointers[parent];
	if (node != nullptr)
	{
		std::vector<GameObject> result;
		this->GetByNameInChildren(parent, name, result);
		return result;
	}
	static std::vector<GameObject> _empty;
	return _empty;
}

void GameObjectHierarchy::GetByNameInChildren(GameObject parent, std::string name, std::vector<GameObject>& result)
{
	auto node = pointers[parent];
	if (node != nullptr)
	{
		if (name.compare(HFEngine::ECS.GetNameGameObject(parent)) == 0) result.push_back(parent);
		for (auto child : node->children)
		{
			GetByNameInChildren(child, name, result);
		}
	}
}

std::optional<GameObject> GameObjectHierarchy::GetParent(GameObject child)
{
	auto node = pointers[child];
	if (node != nullptr)
	{
		return node->parent;
	}
	return std::nullopt;
}

std::vector<GameObject>& GameObjectHierarchy::GetRoot()
{
	return root;
}

bool GameObjectHierarchy::IsPresent(GameObject gameObject)
{
	return pointers[gameObject] != nullptr;
}
