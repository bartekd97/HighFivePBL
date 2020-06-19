#pragma once

#include <glm/glm.hpp>

#include "../System.h"
#include "../Components.h"
#include "TriggerColliderCollectorSystem.h"

class PhysicsSpawnTriggerSystem : public System, public ISystemUpdate
{
public:
	void Update(float dt) override;
	void SetCollector(std::shared_ptr<TriggerColliderCollectorSystem> triggerColliderCollectorSystem);
private:
	float minimalMovement;
	std::shared_ptr<TriggerColliderCollectorSystem> triggerColliderCollectorSystem;
};
