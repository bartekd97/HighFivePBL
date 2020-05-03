#pragma once

#include <functional>
#include "../ECSTypes.h"

#define TriggerMethodPointer(method) std::bind(&method, this, std::placeholders::_1)

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
		BOX
	};

	ColliderTypes type;
	ColliderShapes shape;
	bool frozen = false;

	std::function<void(GameObject)> OnTriggerEnter;
	std::function<void(GameObject)> OnTriggerExit;
};
