#pragma once

#include <functional>
#include "../ECSTypes.h"

#define TriggerMethodPointer(method) std::bind(&method, this, std::placeholders::_1, std::placeholders::_2)

struct Collider
{
	enum class ColliderTypes
	{
		DYNAMIC,
		STATIC,
		TRIGGER
	};

	enum class ColliderShapes
	{
		CIRCLE,
		BOX,
		GRAVITY // xD
	};

	ColliderTypes type;
	ColliderShapes shape;
	bool frozen = false;

	std::function<void(GameObject, GameObject)> OnTriggerEnter;
	std::function<void(GameObject, GameObject)> OnTriggerExit;
};
