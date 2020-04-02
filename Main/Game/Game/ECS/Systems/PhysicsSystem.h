#pragma once

#include "../SystemUpdate.h"

class PhysicsSystem : public SystemUpdate
{
public:
	void Update(float dt) override;
};
