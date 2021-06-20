#pragma once

#include <glm/glm.hpp>
#include "ECS/System.h"
#include "ECS/Components/GrassCircleSimulationPrimitive.h"
#include "ECS/Components/GrassSimulator.h"

class GrassPrimitiveCollectorSystem : public System, public ISystemUpdate
{
public:
	void Init() override;
	void Update(float dt) override;

private:
	bool IsInside(const GrassSimulator& simulator, glm::vec3 position, float radius);
};