#pragma once

#include "CacheNode.h"
#include "../HFEngine.h"

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

	bool DetectCollision(const glm::vec3& pos1, const CircleCollider& c1, const glm::vec3& pos2, const CircleCollider& c2, glm::vec3& sepVector);
	bool DetectCollision(const glm::vec3& pos1, const CircleCollider& c1, const glm::vec3& pos2, glm::quat& rotation, const BoxCollider& c2, glm::vec3& sepVector);
}
