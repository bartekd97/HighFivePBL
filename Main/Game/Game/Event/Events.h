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
	const EventId FRAME_START = "Events::General::FRAME_START"_hash;
	const EventId UPDATE = "Events::General::UPDATE"_hash;
	const EventId POST_UPDATE = "Events::General::POST_UPDATE"_hash;
	const EventId WINDOW_RESIZE = "Events::General::WINDOWS_RESIZE"_hash;
}

namespace Events::Gameplay
{
	namespace Ghost
	{
		const EventId MOVEMENT_START = "Events::Gameplay::Ghost::MOVEMENT_START"_hash;
		const EventId MOVEMENT_STOP = "Events::Gameplay::Ghost::MOVEMENT_STOP"_hash;
		const EventId LINE_CREATED = "Events::Gameplay::Ghost::LINE_CREATED"_hash;
	}
	namespace MiniGhost
	{
		const EventId FADE_ME_OUT = "Events::Gameplay::MiniGhost::FADE_ME_OUT"_hash;
		const EventId ATTACK = "Events::Gameplay::MiniGhost::ATTACK"_hash;
	}
}

namespace Events::GameObject
{
	const EventId DESTROYED = "Events::GameObject::DESTROYED"_hash;
	const EventId NEXT_FRAME_DESTROY_REQUESTED = "Events::GameObject::NEXT_FRAME_DESTROY_REQUESTED"_hash;
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

namespace Events::Gameplay
{
	namespace Ghost
	{
		const ParamId GhostLine = "Events::Gameplay::Ghost::GhostLine"_hash; // std::shared_ptr<GhostLine>
	}
	namespace MiniGhost
	{
		const ParamId Direction = "Events::Gameplay::MiniGhost::Direction"_hash; // glm::vec3
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
