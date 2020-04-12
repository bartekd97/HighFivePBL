#pragma once

#include "../SystemUpdate.h"

class PhysicsSystem : public SystemUpdate
{
public:
	void Init() override;
	void Update(float dt) override;
private:
	float step;
};
