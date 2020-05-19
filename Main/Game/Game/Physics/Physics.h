#pragma once

#include "CacheNode.h"
#include "../HFEngine.h"
#include "RaycastHit.h"

#define CLAMP(x,min,max) x > min ? (x < max ? x : max) : min
#define VECLEN(v) std::sqrtf(v.x*v.x + v.z*v.z);
#define VECLEN2D(v) std::sqrtf(v.x*v.x + v.y*v.y);

namespace Physics
{
	extern const float step;
	extern const int maxSteps;

	extern std::unordered_map<GameObject, CacheNode> cacheNodes;

	void ProcessGameObjects(const tsl::robin_set<GameObject>& gameObjects);
	void RemoveNode(GameObject gameObject);

	bool Raycast(glm::vec3& position, glm::vec3& direction, RaycastHit& out, GameObject ignoredGameObject = NULL_GAMEOBJECT, float maxDistance = std::numeric_limits<float>::max());
	bool Raycast(glm::vec3& start, glm::vec3& stop, RaycastHit& out, GameObject ignoredGameObject = NULL_GAMEOBJECT);

	bool DetectCollision(const glm::vec3& pos1, const CircleCollider& c1, const glm::vec3& pos2, const CircleCollider& c2, glm::vec3& sepVector);
	bool DetectCollision(const glm::vec3& pos1, const CircleCollider& c1, const glm::vec3& pos2, glm::quat& rotation, const BoxCollider& c2, glm::vec3& sepVector);
	bool DetectCollision(const glm::vec3& pos1, const glm::vec3& pos2, const CircleCollider& c2, glm::vec3& sepVector);
	bool DetectCollision(const glm::vec3& pos1, const glm::vec3& pos2, glm::quat& rotation, const BoxCollider& c2, glm::vec3& sepVector);
}
