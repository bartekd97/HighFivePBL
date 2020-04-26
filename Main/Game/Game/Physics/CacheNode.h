#pragma once

#include "../ECS/Components.h"

struct CacheNode
{
	CacheNode() {}
	CacheNode(Transform& transform, const Collider& collider) : position(transform.GetWorldPosition()), rotation(transform.GetRotation()), collider(collider) {}

	glm::vec3 position;
	glm::quat rotation;
	Collider collider;
	BoxCollider boxCollider;
	CircleCollider circleCollider;
};
