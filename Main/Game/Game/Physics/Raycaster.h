#pragma once

#include <chrono>
#include "Physics.h"

class Raycaster
{
public:
    Raycaster(float maxDistance = 50.0f, int maxChecks = 3, float maxDistanceBeforeRefresh = 1.0f, float maxTimeBeforeDynamicRefresh = 2.0f);

    RaycastHit& GetOut();
    bool Raycast(glm::vec3& position, glm::vec3& direction);
    void SetIgnoredGameObject(GameObject gameObject);

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
    int maxChecks;

	std::vector<std::pair<float, GameObject>> staticGameObjects;
    std::vector<std::pair<float, GameObject>> dynamicGameObjects;

    bool CircleCast(glm::vec2& position, glm::vec2& direction, GameObject gameObject, glm::vec2& intersection);
    bool BoxCast(glm::vec2& position, glm::vec2& direction, GameObject gameObject, glm::vec2& intersection);
    bool WallCast(glm::vec2& position, glm::vec2& direction, glm::vec2& wallA, glm::vec2& wallB, glm::vec2& intersectPoint);

    void ResetInitialized();

    bool CheckStaticRefresh(glm::vec3& position, glm::vec3& direction);
    bool CheckDynamicRefresh();
	
    void RefreshStaticGameObjects(glm::vec3& position, glm::vec3& direction);
    void RefreshDynamicGameObjects(glm::vec3& position, glm::vec3& direction);
};
