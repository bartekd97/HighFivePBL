#include "EventManager.h"

namespace EventManager
{
	std::unordered_map<EventId, std::vector<std::function<void(Event&)>>> listeners;

	void AddListener(EventId eventId, std::function<void(Event&)> const& listener)
	{
		listeners[eventId].push_back(listener);
	}

	void FireEvent(Event& event)
	{
		uint32_t type = event.GetType();

		for (auto const& listener : listeners[type])
		{
			listener(event);
		}
	}

	void FireEvent(EventId eventId)
	{
		Event event(eventId);

		for (auto const& listener : listeners[eventId])
		{
			listener(event);
		}
	}
}
