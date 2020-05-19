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

    bool DetectCollision(const glm::vec3& pos1, const CircleCollider& c1, const glm::vec3& pos2, glm::quat& rotation2, const BoxCollider& c2, glm::vec3& sepVector)
    {
        // TODO: refaktor. maybe cache?
        float tbx1, tby1, tbx2, tby2;
        tbx1 = -c2.width / 2.0f;
        tby1 = -c2.height / 2.0f;
        tbx2 = c2.width / 2.0f;
        tby2 = c2.height / 2.0f;

        glm::vec3 boxBPoints[4] = {
            { tbx1, 0.0f, tby1 },
            { tbx2, 0.0f, tby1 },
            { tbx2, 0.0f, tby2 },
            { tbx1, 0.0f, tby2 }
        };
        glm::vec2 boxPoints[4];
        glm::vec2 boxPos(pos2.x, pos2.z);
        for (int i = 0; i < 4; i++)
        {
            //boxPoints[i] = glm::rotate(boxPoints[i], rotation2) + boxPos;
            boxBPoints[i] = rotation2 * boxBPoints[i];
            boxPoints[i].x = boxBPoints[i].x + boxPos.x;
            boxPoints[i].y = boxBPoints[i].z + boxPos.y;
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
}
