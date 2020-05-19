#pragma once

#include "CacheNode.h"
#include "../HFEngine.h"

namespace PhysicsCache
{
	extern std::unordered_map<GameObject, CacheNode> nodes;

	void ProcessGameObjects(const tsl::robin_set<GameObject>& gameObjects);
	void RemoveNode(GameObject gameObject);
}
