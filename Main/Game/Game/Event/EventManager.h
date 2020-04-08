#pragma once

#include <functional>
#include <vector>
#include <unordered_map>

#include "Event.h"

namespace EventManager
{
	void AddListener(EventId eventId, std::function<void(Event&)> const& listener);

	void FireEvent(Event& event);

	void FireEvent(EventId eventId);
};
