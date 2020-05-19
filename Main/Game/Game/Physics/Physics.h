#pragma once

#include "CacheNode.h"
#include "../HFEngine.h"

namespace Physics
{
	extern const float step;
	extern const int maxSteps;

	extern std::unordered_map<GameObject, CacheNode> cacheNodes;

	void ProcessGameObjects(const tsl::robin_set<GameObject>& gameObjects);
	void RemoveNode(GameObject gameObject);
}
