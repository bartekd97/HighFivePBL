#pragma once

#include <set>

#include "ECSTypes.h"

class System
{
public:
	virtual void Init() {};
	std::set<GameObject> gameObjects;
};
