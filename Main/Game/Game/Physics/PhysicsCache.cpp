#include "PhysicsCache.h"

namespace PhysicsCache
{
	std::unordered_map<GameObject, CacheNode> nodes;

	void ProcessGameObjects(const tsl::robin_set<GameObject>& gameObjects)
	{
		for (const auto& gameObject : gameObjects)
		{
			auto it = nodes.find(gameObject);
			if (it == nodes.end())
			{
				auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
				auto const& collider = HFEngine::ECS.GetComponent<Collider>(gameObject);
				nodes[gameObject] = CacheNode(transform, collider);
				if (collider.shape == Collider::ColliderShapes::BOX)
				{
					nodes[gameObject].boxCollider = HFEngine::ECS.GetComponent<BoxCollider>(gameObject);
				}
				else if (collider.shape == Collider::ColliderShapes::CIRCLE)
				{
					nodes[gameObject].circleCollider = HFEngine::ECS.GetComponent<CircleCollider>(gameObject);
				}
			}
			else
			{
				if (!it->second.collider.frozen)
				{
					auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
					it->second.position = transform.GetWorldPosition();
					it->second.rotation = transform.GetWorldRotation();
				}
			}
		}
	}

	void RemoveNode(GameObject gameObject)
	{
		nodes.erase(gameObject);
	}
}
