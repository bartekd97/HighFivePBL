#pragma once

#include <vector>

#include "ECSTypes.h"

class HierarchyNode
{
public:
	HierarchyNode(GameObject gameObject, std::shared_ptr<HierarchyNode> parent) : gameObject(gameObject), parent(parent) {}

	GameObject gameObject;
	std::shared_ptr<HierarchyNode> parent;
	std::vector<HierarchyNode> children;
};
