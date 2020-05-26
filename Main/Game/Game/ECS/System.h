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