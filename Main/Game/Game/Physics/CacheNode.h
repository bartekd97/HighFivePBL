#pragma once

#include <tsl/robin_set.h>

#include "../ECS/Components.h"

struct CacheNode
{
	const float GRID_SIZE = 25.0f;
	enum class STATE
	{
		ACTIVE,
		INACTIVE,
		REMOVED
	};

	CacheNode()
	{
	}

	bool HasTrigger(GameObject gameObject)
	{
		return triggers.find(gameObject) != triggers.end();
	}

	void CalculateBoxPoints()
	{
		glm::vec3 boxBPoint;
		for (int i = 0; i < 4; i++)
		{
			boxBPoint = rotation * boxCollider.bPoints[i];
			boxRealPoints[i].x = boxBPoint.x + position.x;
			boxRealPoints[i].y = boxBPoint.z + position.z;
		}
		glm::vec2 min(std::numeric_limits<float>::max()), max(std::numeric_limits<float>::min());
		for (int i = 0; i < 4; i++)
		{
			if (boxRealPoints[i].x < min.x) min.x = boxRealPoints[i].x;
			else if (boxRealPoints[i].x > max.x) max.x = boxRealPoints[i].x;
			if (boxRealPoints[i].y < min.y) min.y = boxRealPoints[i].y;
			else if (boxRealPoints[i].y > max.y) max.y = boxRealPoints[i].y;
		}
		boxMinMax[0] = min;
		boxMinMax[1] = max;
		maxSide = std::max(boxCollider.width, boxCollider.height);
	}

	void CalculateGrid()
	{
		if (collider.shape == Collider::ColliderShapes::BOX)
		{
			gridMinX = boxMinMax[0].x / GRID_SIZE;
			gridMinY = boxMinMax[0].y / GRID_SIZE;
			gridMaxX = std::ceil(boxMinMax[1].x / GRID_SIZE);
			gridMaxY = std::ceil(boxMinMax[1].y / GRID_SIZE);
		}
		else if (collider.shape == Collider::ColliderShapes::CIRCLE)
		{
			gridMinX = (position.x - circleCollider.radius) / GRID_SIZE;
			gridMinY = (position.z - circleCollider.radius) / GRID_SIZE;
			gridMaxX = (position.x + circleCollider.radius) / GRID_SIZE;
			gridMaxY = (position.z + circleCollider.radius) / GRID_SIZE;
		}
	}

	glm::vec3 position;
	glm::quat rotation;
	Collider collider;

	BoxCollider boxCollider;
	glm::vec2 boxRealPoints[4];
	glm::vec2 boxMinMax[2];
	float maxSide;

	CircleCollider circleCollider;
	tsl::robin_set<GameObject> triggers;

	bool hasRigidBody = false;
	STATE state = STATE::REMOVED;

	float lastFrameUpdate;
	int gridMinX;
	int gridMinY;
	int gridMaxX;
	int gridMaxY;
};
