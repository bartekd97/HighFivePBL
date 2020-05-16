#pragma once

#include "../SystemUpdate.h"

class ScriptLateUpdateSystem : public SystemUpdate
{
public:
	void Update(float dt) override;
};
