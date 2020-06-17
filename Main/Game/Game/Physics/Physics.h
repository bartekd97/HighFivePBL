#pragma once

#include <set>

#include "CacheNode.h"
#include "../HFEngine.h"
#include "RaycastHit.h"

#define CLAMP(x,min,max) x > min ? (x < max ? x : max) : min
#define VECLEN(v) std::sqrtf(v.x*v.x + v.z*v.z)
#define VECLEN2D(v) std::sqrtf(v.x*v.x + v.y*v.y)

namespace Physics
{
	extern const float step;
	extern const float maxDelta;
	extern const int maxSteps;

	extern std::array<CacheNode, MAX_GAMEOBJECTS> cacheNodes;
	extern int maxGameObject;
	extern std::shared_ptr<System> rigidBodyCollector;

	void SetRigidBodyCollector(std::shared_ptr<System> rigidBodyCollector);

	void ProcessGameObjects(const tsl::robin_set<GameObject>& gameObjects, bool disableOthers = false);
	void ClearGameObjects();
	void RemoveNode(GameObject gameObject);

	bool Raycast(glm::vec3& position, glm::quat& rotation, const BoxCollider& boxCollider, RaycastHit& out, GameObject ignoredGameObject = NULL_GAMEOBJECT);
	bool Raycast(glm::vec3& position, const CircleCollider& circleCollider, RaycastHit& out, GameObject ignoredGameObject = NULL_GAMEOBJECT);

	bool DetectCollision(const glm::vec3& pos1, const CircleCollider& c1, const glm::vec3& pos2, const CircleCollider& c2, glm::vec3& sepVector);
	bool DetectCollision(const glm::vec3& pos1, const CircleCollider& c1, const glm::vec3& pos2, glm::quat& rotation2, const BoxCollider& c2, glm::vec3& sepVector);
	bool DetectCollision(const glm::vec3& pos1, const CircleCollider& c1, GameObject boxGameObject, glm::vec3& sepVector);

	//bool DetectCollision(const glm::vec3& pos1, const glm::vec3& pos2, const CircleCollider& c2, glm::vec3& sepVector);
	//bool DetectCollision(const glm::vec3& pos1, const glm::vec3& pos2, glm::quat& rotation2, const BoxCollider& c2, glm::vec3& sepVector);
}
