#include "Physics.h"

namespace Physics
{
	const float step = 0.15f;
	const int maxSteps = 20;

	std::unordered_map<GameObject, CacheNode> cacheNodes;

	void ProcessGameObjects(const tsl::robin_set<GameObject>& gameObjects)
	{
		for (const auto& gameObject : gameObjects)
		{
			auto it = cacheNodes.find(gameObject);
			if (it == cacheNodes.end())
			{
				auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
				auto const& collider = HFEngine::ECS.GetComponent<Collider>(gameObject);
				cacheNodes[gameObject] = CacheNode(transform, collider);
				if (collider.shape == Collider::ColliderShapes::BOX)
				{
					cacheNodes[gameObject].boxCollider = HFEngine::ECS.GetComponent<BoxCollider>(gameObject);
				}
				else if (collider.shape == Collider::ColliderShapes::CIRCLE)
				{
					cacheNodes[gameObject].circleCollider = HFEngine::ECS.GetComponent<CircleCollider>(gameObject);
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
		cacheNodes.erase(gameObject);
	}
}
