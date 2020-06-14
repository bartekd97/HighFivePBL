#pragma once

#include <glm/glm.hpp>

#include "../System.h"
#include "../Components.h"
#include "ColliderCollectorSystem.h"

class PhysicsSystem : public System, public ISystemUpdate
{
public:
	void Init() override;
	void Update(float dt) override;
	void SetCollector(std::shared_ptr<ColliderCollectorSystem> colliderCollectorSystem);
private:
	float minimalMovement;
	std::shared_ptr<ColliderCollectorSystem> colliderCollectorSystem;
};
