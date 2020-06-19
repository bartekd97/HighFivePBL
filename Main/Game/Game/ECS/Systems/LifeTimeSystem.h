#pragma once

#include "../System.h"

//TEST SYSTEM
class LifeTimeSystem : public System, public ISystemUpdate
{
public:
	void Update(float dt) override;
};
