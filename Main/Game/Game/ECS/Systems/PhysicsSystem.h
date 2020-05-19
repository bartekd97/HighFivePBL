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

	std::shared_ptr<ColliderCollectorSystem> colliderCollectorSystem;
};
