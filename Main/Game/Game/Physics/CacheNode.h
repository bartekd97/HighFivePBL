#pragma once

#include <tsl/robin_set.h>

#include "../ECS/Components.h"

struct CacheNode
{
	CacheNode() {}
	CacheNode(Transform& transform, const Collider& collider) : position(transform.GetWorldPosition()), rotation(transform.GetWorldRotation()), collider(collider), lastFrameUpdate(transform.LastFrameUpdate())
	{
	}

	bool HasTrigger(GameObject gameObject)
	{
		return triggers.find(gameObject) != triggers.end();
	}

	void CalculateBoxRealPoints()
	{
		glm::vec3 boxBPoint;
		for (int i = 0; i < 4; i++)
		{
			boxBPoint = rotation * boxCollider.bPoints[i];
			boxRealPoints[i].x = boxBPoint.x + position.x;
			boxRealPoints[i].y = boxBPoint.z + position.y;
		}
	}

	glm::vec3 position;
	glm::quat rotation;
	Collider collider;

	BoxCollider boxCollider;
	glm::vec2 boxRealPoints[4];

	CircleCollider circleCollider;
	tsl::robin_set<GameObject> triggers;

	float lastFrameUpdate;
};
