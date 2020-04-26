#pragma once

#include "CacheNode.h"
#include "../HFEngine.h"

namespace PhysicsCache
{
	std::unordered_map<GameObject, CacheNode> nodes;

	void ProcessGameObjects(const std::set<GameObject>& gameObjects)
	{
		// TODO: jakoœ ¿eby nie liczy³o za ka¿dym, tylko gdy trzeba
		nodes.clear();
		for (const auto& gameObject : gameObjects)
		{
			auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
			auto const& collider = HFEngine::ECS.GetComponent<Collider>(gameObject);
			nodes[gameObject] = CacheNode(transform, collider);
			if (collider.type == Collider::ColliderShapes::BOX)
			{
				nodes[gameObject].boxCollider = HFEngine::ECS.GetComponent<BoxCollider>(gameObject);
			}
			else
			{
				nodes[gameObject].circleCollider = HFEngine::ECS.GetComponent<CircleCollider>(gameObject);
			}
		}
	}
}
