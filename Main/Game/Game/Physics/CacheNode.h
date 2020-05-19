#pragma once

#include <tsl/robin_set.h>

#include "../ECS/Components.h"

struct CacheNode
{
	CacheNode() {}
	CacheNode(Transform& transform, const Collider& collider) : position(transform.GetWorldPosition()), rotation(transform.GetWorldRotation()), collider(collider)
	{
	}

	bool HasTrigger(GameObject gameObject)
	{
		return triggers.find(gameObject) != triggers.end();
	}

	glm::vec3 position;
	glm::quat rotation;
	Collider collider;
	BoxCollider boxCollider;
	CircleCollider circleCollider;
	tsl::robin_set<GameObject> triggers;
};
