#pragma once

#include <set>

#include "ECSTypes.h"

class System
{
public:
	virtual ~System() {};
	virtual void Init() {};
	std::set<GameObject> gameObjects;
};
