#pragma once

#include "System.h"

class SystemUpdate : public System
{
public:
	virtual void Update(float dt) = 0;
};
