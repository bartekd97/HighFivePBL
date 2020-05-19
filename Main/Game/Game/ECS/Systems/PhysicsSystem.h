#pragma once

#include <glm/glm.hpp>

#include "../SystemUpdate.h"
#include "../Components.h"
#include "ColliderCollectorSystem.h"

class PhysicsSystem : public SystemUpdate
{
public:
	void Init() override;
	void Update(float dt) override;
	void SetCollector(std::shared_ptr<ColliderCollectorSystem> colliderCollectorSystem);
private:
	bool DetectCollision(const glm::vec3& pos1, const CircleCollider& c1, const glm::vec3& pos2, const CircleCollider& c2, glm::vec3& sepVector);
	bool DetectCollision(const glm::vec3& pos1, const CircleCollider& c1, const glm::vec3& pos2, glm::quat& rotation, const BoxCollider& c2, glm::vec3& sepVector);
	//bool DetectCollision(const glm::vec3& pos1, const CircleCollider& c1, const glm::vec3& pos2, const float& rotation2, const BoxCollider& c2, glm::vec3& sepVector);

	std::shared_ptr<ColliderCollectorSystem> colliderCollectorSystem;
};
