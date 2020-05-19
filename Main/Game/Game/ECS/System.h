#pragma once

#include <tsl/robin_set.h>

#include "ECSTypes.h"

class System
{
public:
	virtual ~System() {};
	virtual void Init() {};
	tsl::robin_set<GameObject> gameObjects;
};
