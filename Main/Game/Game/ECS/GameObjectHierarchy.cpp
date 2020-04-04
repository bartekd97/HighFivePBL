#include <algorithm>

#include "GameObjectHierarchy.h"

void GameObjectHierarchy::AddGameObject(GameObject child, GameObject parent)
{
	auto parentNode = pointers[parent];
	if (parentNode == nullptr)
	{
		nodes.push_back(HierarchyNode(parent, nullptr));
		parentNode = std::make_shared<HierarchyNode>(nodes.back());
		pointers[parent] = parentNode;
	}
	parentNode->children.push_back(HierarchyNode(child, parentNode));
	pointers[child] = std::make_shared<HierarchyNode>(parentNode->children.back());
}

void GameObjectHierarchy::RemoveGameObject(GameObject gameObject)
{
	auto node = pointers[gameObject];
	if (node != nullptr)
	{
		for (int i = node->children.size() - 1; i >= 0; i--)
		{
			RemoveGameObject(node->children[i].gameObject);
		}
		if (node->parent == nullptr)
		{
			nodes.erase(std::remove_if(nodes.begin(), nodes.end(), [gameObject](HierarchyNode node) { return node.gameObject == gameObject; }), nodes.end());
		}
		else
		{
			node->parent->children.erase(std::remove_if(nodes.begin(), nodes.end(), [gameObject](HierarchyNode node) { return node.gameObject == gameObject; }), nodes.end());
		}
		pointers[gameObject] = nullptr;
	}
}

std::vector<GameObject> GameObjectHierarchy::GetChildren(GameObject parent)
{
	std::vector<GameObject> children;
	auto node = pointers[parent];
	if (node != nullptr)
	{
		for (auto it = node->children.begin(); it != node->children.end(); it++)
		{
			children.push_back(it->gameObject);
		}
	}
	return children;
}
