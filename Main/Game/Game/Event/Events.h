#pragma once

#include <bitset>

#include "EventManager.h"
#include "../Utility/Utility.h"

#define METHOD_LISTENER(EventType, Listener) EventType, std::bind(&Listener, this, std::placeholders::_1)
#define SCRIPT_LISTENER(EventType, Listener) EventType, GetGameObject(), std::bind(&Listener, this, std::placeholders::_1)
#define FUNCTION_LISTENER(EventType, Listener) EventType, std::bind(&Listener, std::placeholders::_1)

// EVENTS

namespace Events::Test
{
	const EventId TICK = "Events::Test::TICK"_hash;
}

namespace Events::General
{
	const EventId UPDATE = "Events::General::UPDATE"_hash;
	const EventId LATE_UPDATE = "Events::General::LATE_UPDATE"_hash;
}

namespace Events::Gameplay
{
	namespace Ghost
	{
		const EventId MOVEMENT_START = "Events::Gameplay::Ghost::MOVEMENT_START"_hash;
		const EventId MOVEMENT_STOP = "Events::Gameplay::Ghost::MOVEMENT_STOP"_hash;
	}
}

namespace Events::GameObject
{
	const EventId DESTROYED = "Events::GameObject::DESTROYED"_hash;
	namespace Script
	{
		const EventId ADDED = "Events::GameObject::Script::ADDED"_hash;
	}
}

// PARAMS

namespace Events::General
{
	const ParamId DELTA_TIME = "Events::General::DELTA_TIME"_hash;
}

namespace Events::GameObject
{
	const ParamId GameObject = "Events::GameObject::GameObject"_hash;
	namespace Script
	{
		const ParamId Index = "Events::GameObject::Script::Index"_hash;
	}
}
