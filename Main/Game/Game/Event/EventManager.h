#pragma once

#include <functional>
#include <vector>
#include <unordered_map>

#include "../ECS/ECSTypes.h"
#include "Event.h"

namespace EventManager
{
	void Initialize();

	void AddListener(EventId eventId, std::function<void(Event&)> const& listener);

	void AddScriptListener(EventId eventId, GameObject gameObject, std::function<void(Event&)> const& listener);

	void FireEvent(Event& event);

	void FireEvent(EventId eventId);
};
