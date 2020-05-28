#include "Physics.h"

namespace Physics
{
	const float step = 0.15f;
	const int maxSteps = 20;

	std::unordered_map<GameObject, CacheNode> cacheNodes;
    std::shared_ptr<System> rigidBodyCollector;


    void SetRigidBodyCollector(std::shared_ptr<System> rigidBodyCollector)
    {
        Physics::rigidBodyCollector = rigidBodyCollector;
    }

	void ProcessGameObjects(const tsl::robin_set<GameObject>& gameObjects, bool disableOthers)
	{
        if (disableOthers)
        {
            for (auto& node : cacheNodes)
            {
                node.second.active = false;
            }
        }

		for (const auto& gameObject : gameObjects)
		{
			auto it = cacheNodes.find(gameObject);
			if (it == cacheNodes.end())
			{
				auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
				auto const& collider = HFEngine::ECS.GetComponent<Collider>(gameObject);
				cacheNodes[gameObject] = CacheNode(transform, collider);
                if (HFEngine::ECS.SearchComponent<RigidBody>(gameObject))
                {
                    cacheNodes[gameObject].hasRigidBody = true;
                }
				if (collider.shape == Collider::ColliderShapes::BOX)
				{
					cacheNodes[gameObject].boxCollider = HFEngine::ECS.GetComponent<BoxCollider>(gameObject);
                    cacheNodes[gameObject].CalculateBoxPoints();
				}
				else if (collider.shape == Collider::ColliderShapes::CIRCLE)
				{
					cacheNodes[gameObject].circleCollider = HFEngine::ECS.GetComponent<CircleCollider>(gameObject);
				}
			}
			else
			{
                it->second.active = true;
				if (!it->second.collider.frozen)
				{
					auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
                    if (transform.LastFrameUpdate() != it->second.lastFrameUpdate)
                    {
                        it->second.position = transform.GetWorldPosition();
                        it->second.rotation = transform.GetWorldRotation();
                        cacheNodes[gameObject].CalculateBoxPoints();
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

    bool DetectCollision(const glm::vec3& pos1, glm::quat& rotation1, const BoxCollider& c1, GameObject box2GameObject)
    {
        glm::vec2 boxPoints[4];
        glm::vec3 boxBPoint;
        for (int i = 0; i < 4; i++)
        {
            boxBPoint = rotation1 * c1.bPoints[i];
            boxPoints[i].x = boxBPoint.x + pos1.x;
            boxPoints[i].y = boxBPoint.z + pos1.z;
        }
        auto& box2Points = cacheNodes[box2GameObject].boxRealPoints;
        auto& pos2 = cacheNodes[box2GameObject].position;

        for (int i = 0; i < 4; i++)
        {
            auto current = boxPoints[i];
            auto next = boxPoints[(i + 1) % 4];
            glm::vec2 axis = glm::normalize(next - current);

            auto aMaxProj = -std::numeric_limits<float>::max();
            auto aMinProj = std::numeric_limits<float>::max();
            auto bMaxProj = -std::numeric_limits<float>::max();
            auto bMinProj = std::numeric_limits<float>::max();
            for (int j = 0; j < 4; j++)
            {
                auto proj = glm::dot(boxPoints[j], axis);
                if (proj < aMinProj) aMinProj = proj;
                if (proj > aMaxProj) aMaxProj = proj;

                proj = glm::dot(box2Points[j], axis);
                if (proj < bMinProj) bMinProj = proj;
                if (proj > bMaxProj) bMaxProj = proj;
            }

            if (aMaxProj < bMinProj || aMinProj > bMaxProj)
            {
                return false;
            }

        }

        return true;
    }

    bool DetectCollision(const glm::vec3& pos1, const CircleCollider& c1, const glm::vec3& pos2, glm::quat& rotation2, const BoxCollider& c2, glm::vec3& sepVector) // TODO: poczyœciæ z sepVector
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

        /*auto p2top1 = glm::vec2(pos2.x - pos1.x, pos2.z - pos1.z);
        if (glm::dot(axes[mtvAxisIndex], p2top1) >= 0)
        {
            axes[mtvAxisIndex] *= -1.0f;
        }
        sepVector.x = axes[mtvAxisIndex].x * minOverlap;
        sepVector.z = axes[mtvAxisIndex].y * minOverlap;*/

        return true;
    }

    /*
    float isLeft(glm::vec2& P0, glm::vec2& P1, glm::vec2& P2)
    {
        return ((P1.x - P0.x) * (P2.y - P0.y) - (P2.x - P0.x) * (P1.y - P0.y));
    }

    bool DetectCollision(const glm::vec3& pos1, const glm::vec3& pos2, glm::quat& rotation2, const BoxCollider& c2, glm::vec3& sepVector) //TODO: test, sepVector
    {
        // TODO: zamiana na policzone ju¿
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
    }*/

    bool Raycast(glm::vec3& position, glm::quat& rotation, const BoxCollider& boxCollider, RaycastHit& out, GameObject ignoredGameObject)
    {
        glm::vec3 sepVector;
        for (auto& node : cacheNodes)
        {
            if (node.first == ignoredGameObject || !node.second.active) continue;
            if (node.second.collider.shape == Collider::ColliderShapes::BOX)
            {
                if (DetectCollision(position, rotation, boxCollider, node.first))
                {
                    out.hittedObject = node.first;
                    return true;
                }
            }
            else if (node.second.collider.shape == Collider::ColliderShapes::CIRCLE)
            {
                if (DetectCollision(node.second.position, node.second.circleCollider, position, rotation, boxCollider, sepVector))
                {
                    out.hittedObject = node.first;
                    return true;
                }
            }
        }

        return false;
    }

    bool Raycast(glm::vec3& position, const CircleCollider& circleCollider, RaycastHit& out, GameObject ignoredGameObject) // TODO: przy jednym odpaleniu ³apa³o ca³y czas jakiœ gameObject koñcowy
    {
        glm::vec3 sepVector;
        for (auto& node : cacheNodes)
        {
            if (node.first == ignoredGameObject || !node.second.active) continue;
            if (node.second.collider.shape == Collider::ColliderShapes::BOX)
            {
                if (DetectCollision(position, circleCollider, node.second.position, node.second.rotation, node.second.boxCollider, sepVector))
                {
                    out.hittedObject = node.first;
                    return true;
                }
            }
            else if (node.second.collider.shape == Collider::ColliderShapes::CIRCLE)
            {
                if (DetectCollision(position, circleCollider, node.second.position, node.second.circleCollider, sepVector))
                {
                    out.hittedObject = node.first;
                    return true;
                }
            }
        }

        return false;
    }

    /*bool Raycast(glm::vec3& position, glm::vec2& direction, RaycastHit& out, GameObject ignoredGameObject, float maxDistance)
    {
        glm::vec2 invDirection = 1.0f / direction;
        int sign[2];
        sign[0] = (invDirection.x < 0);
        sign[1] = (invDirection.y < 0);

        float tmin, tmax, tymin, tymax;
        glm::vec3 distVec;

        for (auto& node : cacheNodes)
        {
            if (node.first == ignoredGameObject) continue;

            distVec = position - node.second.position;
            if (VECLEN(distVec) > maxDistance) continue;

            if (node.second.collider.shape == Collider::ColliderShapes::BOX)
            {
                tmin = (node.second.boxMinMax[sign[0]].x - position.x) * invDirection.x;
                tmax = (node.second.boxMinMax[1 - sign[0]].x - position.x) * invDirection.x;
                tymin = (node.second.boxMinMax[sign[1]].y - position.z) * invDirection.y;
                tymax = (node.second.boxMinMax[1 - sign[1]].y - position.z) * invDirection.y;

                if ((tmin > tymax) || (tymin > tmax))
                    continue;
                if (tymin > tmin)
                    tmin = tymin;
                if (tymax < tmax)
                    tmax = tymax;

                out.hitPosition = glm::vec3(tymin, 0.0f, tymax);
                distVec = out.hitPosition - position;
                out.distance = VECLEN(distVec);
                out.hittedObject = node.first;

                return true;
            }
            else if (node.second.collider.shape == Collider::ColliderShapes::CIRCLE)
            {

            }
        }
        return false;
    }*/

    /*bool Raycast(glm::vec3& position, glm::vec3& direction, RaycastHit& out, GameObject ignoredGameObject, float maxDistance)
    {
        glm::vec3 currentPosition = position;
        glm::vec3 diff = currentPosition - position;
        float distance = VECLEN(diff);
        glm::vec3 sepVector;
        while (distance <= maxDistance)
        {
            for (auto& node : cacheNodes)
            {
                if (node.first == ignoredGameObject) continue;

                if (node.second.collider.shape == Collider::ColliderShapes::BOX)
                {
                    if (DetectCollision(currentPosition, node.second.position, node.second.rotation, node.second.boxCollider, sepVector))
                    {
                        out.hitPosition = currentPosition;
                        out.hittedObject = node.first;
                        out.distance = distance;
                        return true;
                    }
                }
            }

            currentPosition += direction * step;
            diff = currentPosition - position;
            distance = VECLEN(diff);
        }

        return false;
    }*/

    bool Raycast(glm::vec3& position, glm::vec3& direction, RaycastHit& out, GameObject ignoredGameObject, float maxDistance)
    {
        std::vector<std::pair<float, GameObject>> objects;
        glm::vec3 tmpPos;
        glm::vec3 tmpCheck;
        float tmpDistance;
        float maxDistanceSquared = maxDistance * maxDistance;
        for (auto& node : cacheNodes)
        {
            if (node.first == ignoredGameObject) continue;

            tmpPos = node.second.position - position;
            tmpCheck = tmpPos * direction;
            if (tmpCheck.x >= 0 && tmpCheck.z >= 0)
            {
                tmpDistance = (tmpPos.x * tmpPos.x) + (tmpPos.z * tmpPos.z);
                if (tmpDistance <= maxDistanceSquared)
                {
                    objects.push_back(std::make_pair(tmpDistance, node.first));
                }
            }
        }
        std::sort(objects.begin(), objects.end(), [](const std::pair<float, GameObject>& a, const std::pair<float, GameObject>& b) {
            return a.first < b.first;
        });
        return false;
    }
}
