#pragma once

#include <glm/glm.hpp>

#include "../SystemUpdate.h"
#include "../Components.h"

class PhysicsSystem : public SystemUpdate
{
public:
	void Init() override;
	void Update(float dt) override;
private:
	bool DetectCollision(const glm::vec3& pos1, const CircleCollider& c1, const glm::vec3& pos2, const CircleCollider& c2, glm::vec3& sepVector);

	float step;
};
