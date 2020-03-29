#pragma once

#include <set>

#include "ECSTypes.h"

class System
{
public:
	std::set<GameObject> gameObjects;
	virtual void Update(float dt) = 0;
};
