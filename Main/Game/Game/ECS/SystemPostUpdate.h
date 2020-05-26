#pragma once

#include "System.h"

class SystemPostUpdate : public System
{
public:
	virtual void PostUpdate(float dt) = 0;
};
