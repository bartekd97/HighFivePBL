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

class ISystemUpdate
{
public:
	virtual void Update(float dt) = 0;
};

class ISystemPostUpdate
{
public:
	virtual void PostUpdate(float dt) = 0;
};