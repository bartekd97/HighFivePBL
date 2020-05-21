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
                    cacheNodes[gameObject].CalculateBoxRealPoints();
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
                    if (transform.LastFrameUpdate() != it->second.lastFrameUpdate)
                    {
                        it->second.position = transform.GetWorldPosition();
                        it->second.rotation = transform.GetWorldRotation();
                        cacheNodes[gameObject].CalculateBoxRealPoints();
                    }
				}
			}
		}
	}

	void RemoveNode(GameObject gameObject)
	{
		cacheNodes.erase(gameObject);
	}

    bool DetectCollision(const glm::vec3& pos1, const CircleCollider& c1, const glm::vec3& pos2, const CircleCollider& c2, glm::vec3& sepVector)
    {
        float r = c1.radius + c2.radius;
        glm::vec2 c1c2(pos1.x - pos2.x, pos1.z - pos2.z);

        float dist = VECLEN2D(c1c2);
        if (dist >= r) return false;

        sepVector.x = (c1c2.x / dist) * (r - dist);
        sepVector.z = (c1c2.y / dist) * (r - dist);

        return true;
    }

    bool DetectCollision(const glm::vec3& pos1, const CircleCollider& c1, GameObject boxGameObject, glm::vec3& sepVector)
    {
        /*glm::vec2 boxPoints[4];
        glm::vec2 boxPos(pos2.x, pos2.z);
        glm::vec3 boxBPoint;
        for (int i = 0; i < 4; i++)
        {
            boxBPoint = rotation2 * c2.bPoints[i];
            boxPoints[i].x = boxBPoint.x + boxPos.x;
            boxPoints[i].y = boxBPoint.z + boxPos.y;
        }*/
        auto& boxPoints = cacheNodes[boxGameObject].boxRealPoints;
        auto& pos2 = cacheNodes[boxGameObject].position;

        glm::vec2 circleCenter(pos1.x, pos1.z);
        glm::vec2 axes[5];
        int nearestPointIndex = 0;
        float minDist = std::numeric_limits<float>::max();
        float tmpDist;
        glm::vec2 temp;

        //Get box axes & nearest point
        for (int i = 0; i < 4; i++)
        {
            auto secondIndex = (i == 3) ? 0 : i + 1;
            axes[i] = glm::normalize(boxPoints[secondIndex] - boxPoints[i]);

            temp = boxPoints[i] - circleCenter;
            tmpDist = VECLEN2D(temp);
            if (tmpDist < minDist)
            {
                minDist = tmpDist;
                nearestPointIndex = i;
            }
        }

        temp = boxPoints[nearestPointIndex] - circleCenter;
        axes[4] = glm::normalize(temp);

        float overlap;
        float minCircle, maxCircle, minBox, maxBox;
        float tempProjection;
        int mtvAxisIndex = -1; //TODO: DEV -1
        float minOverlap = std::numeric_limits<float>::max();

        for (int i = 0; i < 5; i++)
        {
            tempProjection = glm::dot(circleCenter, axes[i]);
            minCircle = tempProjection - c1.radius;
            maxCircle = tempProjection + c1.radius;

            minBox = glm::dot(boxPoints[0], axes[i]);
            maxBox = minBox;

            for (int j = 1; j < 4; j++)
            {
                tempProjection = glm::dot(boxPoints[j], axes[i]);
                if (tempProjection < minBox) minBox = tempProjection;
                if (tempProjection > maxBox) maxBox = tempProjection;
            }

            auto min1 = std::min(maxCircle, maxBox);
            auto max1 = std::max(minCircle, minBox);
            overlap = min1 - max1;
            if (overlap <= 0.0f) return false;

            if (overlap < minOverlap)
            {
                minOverlap = overlap;
                mtvAxisIndex = i;
            }

        }

        if (mtvAxisIndex == -1) return false; //TODO: DEV

        auto p2top1 = glm::vec2(pos2.x - pos1.x, pos2.z - pos1.z);
        if (glm::dot(axes[mtvAxisIndex], p2top1) >= 0)
        {
            axes[mtvAxisIndex] *= -1.0f;
        }
        sepVector.x = axes[mtvAxisIndex].x * minOverlap;
        sepVector.z = axes[mtvAxisIndex].y * minOverlap;

        return true;
    }

    bool DetectCollision(const glm::vec3& pos1, const CircleCollider& c1, const glm::vec3& pos2, glm::quat& rotation2, const BoxCollider& c2, glm::vec3& sepVector)
    {
        glm::vec2 boxPoints[4];
        glm::vec2 boxPos(pos2.x, pos2.z);
        glm::vec3 boxBPoint;
        for (int i = 0; i < 4; i++)
        {
            boxBPoint = rotation2 * c2.bPoints[i];
            boxPoints[i].x = boxBPoint.x + boxPos.x;
            boxPoints[i].y = boxBPoint.z + boxPos.y;
        }

        glm::vec2 circleCenter(pos1.x, pos1.z);
        glm::vec2 axes[5];
        int nearestPointIndex = 0;
        float minDist = std::numeric_limits<float>::max();
        float tmpDist;
        glm::vec2 temp;

        //Get box axes & nearest point
        for (int i = 0; i < 4; i++)
        {
            auto secondIndex = (i == 3) ? 0 : i + 1;
            axes[i] = glm::normalize(boxPoints[secondIndex] - boxPoints[i]);

            temp = boxPoints[i] - circleCenter;
            tmpDist = VECLEN2D(temp);
            if (tmpDist < minDist)
            {
                minDist = tmpDist;
                nearestPointIndex = i;
            }
        }

        temp = boxPoints[nearestPointIndex] - circleCenter;
        axes[4] = glm::normalize(temp);

        float overlap;
        float minCircle, maxCircle, minBox, maxBox;
        float tempProjection;
        int mtvAxisIndex = -1; //TODO: DEV -1
        float minOverlap = std::numeric_limits<float>::max();

        for (int i = 0; i < 5; i++)
        {
            tempProjection = glm::dot(circleCenter, axes[i]);
            minCircle = tempProjection - c1.radius;
            maxCircle = tempProjection + c1.radius;

            minBox = glm::dot(boxPoints[0], axes[i]);
            maxBox = minBox;

            for (int j = 1; j < 4; j++)
            {
                tempProjection = glm::dot(boxPoints[j], axes[i]);
                if (tempProjection < minBox) minBox = tempProjection;
                if (tempProjection > maxBox) maxBox = tempProjection;
            }

            auto min1 = std::min(maxCircle, maxBox);
            auto max1 = std::max(minCircle, minBox);
            overlap = min1 - max1;
            if (overlap <= 0.0f) return false;

            if (overlap < minOverlap)
            {
                minOverlap = overlap;
                mtvAxisIndex = i;
            }

        }

        if (mtvAxisIndex == -1) return false; //TODO: DEV

        auto p2top1 = glm::vec2(pos2.x - pos1.x, pos2.z - pos1.z);
        if (glm::dot(axes[mtvAxisIndex], p2top1) >= 0)
        {
            axes[mtvAxisIndex] *= -1.0f;
        }
        sepVector.x = axes[mtvAxisIndex].x * minOverlap;
        sepVector.z = axes[mtvAxisIndex].y * minOverlap;

        return true;
    }

    float isLeft(glm::vec2& P0, glm::vec2& P1, glm::vec2& P2)
    {
        return ((P1.x - P0.x) * (P2.y - P0.y) - (P2.x - P0.x) * (P1.y - P0.y));
    }

    bool DetectCollision(const glm::vec3& pos1, const glm::vec3& pos2, glm::quat& rotation2, const BoxCollider& c2, glm::vec3& sepVector) //TODO: test, sepVector
    {
        glm::vec3 relativePointPos = pos1 - pos2;
        glm::vec2 boxPoints[4];
        glm::vec3 boxBPoint;
        for (int i = 0; i < 4; i++)
        {
            boxBPoint = rotation2 * c2.bPoints[i];
            boxPoints[i].x = boxBPoint.x;
            boxPoints[i].y = boxBPoint.z;
        }
        bool collided = false;
        glm::vec2 pointPos(relativePointPos.x, relativePointPos.z);

        return (isLeft(boxPoints[0], boxPoints[1], pointPos) > 0
            && isLeft(boxPoints[1], boxPoints[2], pointPos) > 0
            && isLeft(boxPoints[2], boxPoints[3], pointPos) > 0
            && isLeft(boxPoints[3], boxPoints[0], pointPos) > 0);
    }

    bool DetectCollision(const glm::vec3& pos1, const glm::vec3& pos2, const CircleCollider& c2, glm::vec3& sepVector) //TODO: test, sepVector
    {
        glm::vec3 subPos = pos1 - pos2;
        float dist = VECLEN(subPos);
        return (dist <= c2.radius);
    }

    bool Raycast(glm::vec3& position, const BoxCollider& boxCollider, RaycastHit& out)
    {
        glm::vec3 sepVector;
        for (auto& node : cacheNodes)
        {
            if (node.second.collider.shape == Collider::ColliderShapes::BOX)
            {
                //TODO: box-box SAT
                //if (DetectCollision())
            }
            else if (node.second.collider.shape == Collider::ColliderShapes::CIRCLE)
            {
                glm::quat rotation;
                if (DetectCollision(node.second.position, node.second.circleCollider, position, rotation, boxCollider, sepVector))
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool Raycast(glm::vec3& position, const CircleCollider& circleCollider, RaycastHit& out)
    {
        glm::vec3 sepVector;
        for (auto& node : cacheNodes)
        {
            if (node.second.collider.shape == Collider::ColliderShapes::BOX)
            {
                if (DetectCollision(position, circleCollider, node.second.position, node.second.rotation, node.second.boxCollider, sepVector))
                {

                    return true;
                }
            }
            else if (node.second.collider.shape == Collider::ColliderShapes::CIRCLE)
            {
                glm::quat rotation;
                if (DetectCollision(position, circleCollider, node.second.position, node.second.circleCollider, sepVector))
                {
                    return true;
                }
            }
        }

        return false;
    }
}
