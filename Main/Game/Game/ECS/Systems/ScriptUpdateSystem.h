#pragma once

#include "../System.h"

class ScriptUpdateSystem : public System, public ISystemUpdate
{
public:
	void Update(float dt) override;
};
