#pragma once

#include <chrono>
#include "Physics.h"

//xD
#include "../Rendering/PrimitiveRenderer.h"

class Raycaster
{
public:
	Raycaster()
	{
        ResetInitialized();
        maxTimeBeforeDynamicRefresh = 2.0f;
        maxDistanceSquaredBeforeRefresh = 1.0f;
        maxDistance = 50.0f;
        maxDistanceSquared = maxDistance * maxDistance;
	}

    RaycastHit& GetOut()
    {
        return out;
    }

	bool Raycast(glm::vec3& position, glm::vec3& direction)
	{
        if (CheckStaticRefresh(position, direction))
        {
            RefreshStaticGameObjects(position, direction);
        }
        if (CheckDynamicRefresh())
        {
            RefreshDynamicGameObjects(position, direction);
        }
        static glm::vec2 startPos, endPos, tmpPos;
        static glm::vec2 dynamicIntersection, staticIntersection;
        static const int maxChecks = 3; // TODO: move somewhere
        static int dynamicChecks;
        static std::vector<RaycastHit> hits;
        static RaycastHit raycastHitTmp;

        dynamicChecks = 0;
        hits.clear();

        startPos.x = position.x;
        startPos.y = position.z;
        endPos.x = position.x + (direction.x * maxDistance);
        endPos.y = position.z + (direction.z * maxDistance);
        static glm::vec2 dir;
        dir.x = direction.x;
        dir.y = direction.z;

        for (auto& gameObjectPair : dynamicGameObjects)
        {
            if (dynamicChecks >= maxChecks) break;
            auto node = Physics::cacheNodes[gameObjectPair.second];

            if (node.collider.shape == Collider::ColliderShapes::BOX)
            {
                if (BoxCast(startPos, dir, gameObjectPair.second, dynamicIntersection))
                {
                    tmpPos = dynamicIntersection - startPos;
                    raycastHitTmp.distance = VECLEN2D(tmpPos);
                    raycastHitTmp.hittedObject = gameObjectPair.second;
                    raycastHitTmp.hitPosition = glm::vec3(dynamicIntersection.x, 0.0f, dynamicIntersection.y);
                    hits.push_back(raycastHitTmp);
                    dynamicChecks += 1;
                }
            }
            else if (node.collider.shape == Collider::ColliderShapes::CIRCLE)
            {
                // ????
            }
        }

        // TODO: powtarzalnoœæ
        for (auto& gameObjectPair : staticGameObjects)
        {
            if ((hits.size() - dynamicChecks) >= maxChecks) break;
            //LogInfo("xD {}", gameObjectPair.first);
            auto node = Physics::cacheNodes[gameObjectPair.second];

            if (node.collider.shape == Collider::ColliderShapes::BOX)
            {
                if (BoxCast(startPos, dir, gameObjectPair.second, staticIntersection))
                {
                    tmpPos = staticIntersection - startPos;
                    raycastHitTmp.distance = VECLEN2D(tmpPos);
                    raycastHitTmp.hittedObject = gameObjectPair.second;
                    raycastHitTmp.hitPosition = glm::vec3(staticIntersection.x, 0.0f, staticIntersection.y);
                    hits.push_back(raycastHitTmp);
                }
            }
            else if (node.collider.shape == Collider::ColliderShapes::CIRCLE)
            {
                // ????
            }
        }
        if (hits.size() == 0) return false;
        static float minDist;
        static int minIndex;
        minDist = std::numeric_limits<float>::max();
        for (int i = 0; i < hits.size(); i++)
        {
            if (hits[i].distance < minDist)
            {
                minDist = hits[i].distance;
                minIndex = i;
            }
        }
        out = hits[minIndex];

        return true;
	}

    void SetIgnoredGameObject(GameObject gameObject)
    {
        this->ignoredGameObject = gameObject;
        ResetInitialized();
    }

	
private:
    bool initializedStatic, initializedDynamic;
	GameObject ignoredGameObject;
    RaycastHit out;

	glm::vec3 lastPosition;
	glm::vec3 lastDirection;
    std::chrono::steady_clock::time_point lastDynamicTime;

    float maxTimeBeforeDynamicRefresh;
	float maxDistanceSquaredBeforeRefresh;
    float maxDistance;
	float maxDistanceSquared;

	std::vector<std::pair<float, GameObject>> staticGameObjects; // TODO: co jeœli usuniête? lub disable? tyczy obu
    std::vector<std::pair<float, GameObject>> dynamicGameObjects;

    bool BoxCast(glm::vec2& position, glm::vec2& direction, GameObject gameObject, glm::vec2& intersection)
    {
        static int intersectionIndex;
        intersectionIndex = -1;
        static float distFinal, distTmp;
        distFinal = std::numeric_limits<float>::max();
        static glm::vec2 intersectionTmp, subTmp;

        auto node = Physics::cacheNodes[gameObject];

        for (int i = 0; i < 4; i++)
        {
            if (WallCast(position, direction, node.boxRealPoints[i], node.boxRealPoints[(i == 3) ? 0 : (i + 1)], intersectionTmp))
            {
                subTmp = position - intersectionTmp;
                distTmp = VECLEN2D(subTmp);
                if (distTmp < distFinal)
                {
                    intersectionIndex = i;
                    distFinal = distTmp;
                    intersection = intersectionTmp;
                }
            }
        }

        if (intersectionIndex == -1)
        {
            return false;
        }
        return true;
    }

    bool WallCast(glm::vec2& position, glm::vec2& direction, glm::vec2& wallA, glm::vec2& wallB, glm::vec2& intersectPoint)
    {
        // https://github.com/CodingTrain/website/blob/master/CodingChallenges/CC_145_Ray_Casting/P5/ray.js
        auto x1 = wallA.x;
        auto y1 = wallA.y;
        auto x2 = wallB.x;
        auto y2 = wallB.y;

        auto x3 = position.x;
        auto y3 = position.y;
        auto x4 = position.x + direction.x;
        auto y4 = position.y + direction.y;

        /*float denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
        if (denominator == 0) // TODO: eps?
        {
            return false;
        }

        float xNominator = (x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4);
        float yNominator = (x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4);

        intersectPoint.x = xNominator / denominator;
        intersectPoint.y = yNominator / denominator;

        return true;*/

        float denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
        if (denominator == 0) // TODO: eps?
        {
            return false;
        }

        float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denominator;
        float u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denominator;
        if (t > 0 && t < 1 && u > 0)
        {
            intersectPoint.x = x1 + t * (x2 - x1);
            intersectPoint.y = y1 + t * (y2 - y1);
            return true;
        }
        return false;
    }

    void ResetInitialized()
    {
        initializedStatic = initializedDynamic = false;
    }

    bool CheckStaticRefresh(glm::vec3& position, glm::vec3& direction)
    {
        if (!initializedStatic)
        {
            initializedStatic = true;
            return true;
        }
        else
        {
            glm::vec3 dirCheck = lastDirection * direction;
            if (dirCheck.x < 0 || dirCheck.z < 0)
            {
                return true;
            }

            glm::vec3 tmpPos = position - lastPosition;
            if ((tmpPos.x * tmpPos.x + tmpPos.z * tmpPos.z) > maxDistanceSquaredBeforeRefresh)
            {
                return true;
            }
        }
        return false;
    }

    bool CheckDynamicRefresh()
    {
        if (!initializedDynamic)
        {
            initializedDynamic = true;
            return true;
        }
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<float, std::chrono::seconds::period>(now - lastDynamicTime).count() >= maxTimeBeforeDynamicRefresh;
    }
	
    //Odœwie¿a wszystkie gameObjecty
	void RefreshStaticGameObjects(glm::vec3& position, glm::vec3& direction)
	{
        staticGameObjects.clear();
        lastPosition = position;
        lastDirection = direction;

        glm::vec3 tmpPos, tmpCheck;
        float tmpDistance;
        for (auto& node : Physics::cacheNodes)
        {
            if (node.first == ignoredGameObject) continue;
            if (node.second.collider.type == Collider::ColliderTypes::TRIGGER || node.second.collider.shape == Collider::ColliderShapes::GRAVITY) continue;

            tmpPos = node.second.position - position;
            tmpCheck = tmpPos * direction;
            if (tmpCheck.x >= 0 || tmpCheck.z >= 0) // TODO: ?
            {
                tmpDistance = (tmpPos.x * tmpPos.x) + (tmpPos.z * tmpPos.z);
                if (tmpDistance <= maxDistanceSquared)
                {
                    staticGameObjects.push_back(std::make_pair(tmpDistance, node.first));
                }
            }
        }
        std::sort(staticGameObjects.begin(), staticGameObjects.end(), [](const std::pair<float, GameObject>& a, const std::pair<float, GameObject>& b) {
            return a.first < b.first;
        });
	}

    void RefreshDynamicGameObjects(glm::vec3& position, glm::vec3& direction)
    {
        dynamicGameObjects.clear();
        lastDynamicTime = std::chrono::high_resolution_clock::now();
        glm::vec3 tmpPos, tmpCheck;
        float tmpDistance;
        for (auto& gameObject : Physics::rigidBodyCollector->gameObjects)
        {
            if (gameObject == ignoredGameObject) continue;
            if (!HFEngine::ECS.SearchComponent<Collider>(gameObject)) continue;

            auto it = Physics::cacheNodes.find(gameObject);
            if (it == Physics::cacheNodes.end()) continue; // TODO: mo¿e jednak raw informacje zamiast cache? ma³o ich w sumie
            auto node = *it;

            if (node.second.collider.type == Collider::ColliderTypes::TRIGGER || node.second.collider.shape == Collider::ColliderShapes::GRAVITY) continue;

            tmpPos = node.second.position - position;
            tmpCheck = tmpPos * direction;
            if (tmpCheck.x >= 0 || tmpCheck.z >= 0) // TODO: ?
            {
                tmpDistance = (tmpPos.x * tmpPos.x) + (tmpPos.z * tmpPos.z);
                if (tmpDistance <= maxDistanceSquared)
                {
                    dynamicGameObjects.push_back(std::make_pair(tmpDistance, gameObject));
                }
            }
        }
        std::sort(dynamicGameObjects.begin(), dynamicGameObjects.end(), [](const std::pair<float, GameObject>& a, const std::pair<float, GameObject>& b) {
            return a.first < b.first;
        });
    }
};
