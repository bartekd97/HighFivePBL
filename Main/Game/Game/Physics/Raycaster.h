#pragma once

#include <chrono>
#include "Physics.h"

class Raycaster
{
public:
	Raycaster()
	{
        ResetInitialized();
        maxTimeBeforeDynamicRefresh = 2.0f;
        maxDistanceSquaredBeforeRefresh = 10.0f * 10.0f;
        maxDistanceSquared = 500.0f * 500.0f;
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
        return false;
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
	float lastMaxDistance;
    std::chrono::steady_clock::time_point lastDynamicTime;

    float maxTimeBeforeDynamicRefresh;
	float maxDistanceSquaredBeforeRefresh;
	float maxDistanceSquared;

	std::vector<std::pair<float, GameObject>> staticGameObjects; // TODO: co jeœli usuniête? lub disable? tyczy obu
    std::vector<std::pair<float, GameObject>> dynamicGameObjects;

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

            tmpPos = node.second.position - position;
            tmpCheck = tmpPos * direction;
            if (tmpCheck.x >= 0 && tmpCheck.z >= 0)
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

            tmpPos = node.second.position - position;
            tmpCheck = tmpPos * direction;
            if (tmpCheck.x >= 0 && tmpCheck.z >= 0)
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
