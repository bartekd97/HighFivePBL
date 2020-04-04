#pragma once

#include <vector>
#include <optional>

#include "ECSTypes.h"

class HierarchyNode
{
public:
	HierarchyNode(GameObject gameObject, std::optional<GameObject> parent = std::nullopt) : gameObject(gameObject), parent(parent) {}

	GameObject gameObject;
	std::optional<GameObject> parent;
	std::vector<GameObject> children;
};
