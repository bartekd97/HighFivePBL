#pragma once

#include <bitset>

#include "Event.h"
#include "../Utility/Utility.h"

#define METHOD_LISTENER(EventType, Listener) EventType, std::bind(&Listener, this, std::placeholders::_1)
#define FUNCTION_LISTENER(EventType, Listener) EventType, std::bind(&Listener, std::placeholders::_1)

namespace Events::Test
{
	const EventId TICK = "Events::Test::TICK"_hash;
}

namespace Events::GameObject
{
	const EventId DESTROYED = "Events::GameObject::DESTROYED"_hash;
	namespace Script
	{
		const EventId ADDED = "Events::GameObject::Script::ADDED"_hash;
	}
}

namespace Events::GameObject
{
	const ParamId GameObject = "Events::GameObject::GameObject"_hash;
	namespace Script
	{
		const ParamId Index = "Events::GameObject::Script::Index"_hash;
	}
}
