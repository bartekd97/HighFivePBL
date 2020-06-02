#pragma once

#include "../System.h"

class ScriptLateUpdateSystem : public System, public ISystemUpdate
{
public:
	void Update(float dt) override;
};
