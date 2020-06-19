#include "EventManager.h"
#include "Events.h"

namespace EventManager
{
	std::unordered_map<EventId, std::vector<std::function<void(Event&)>>> listeners;
	std::unordered_map<EventId, std::unordered_map<GameObject, std::vector<std::function<void(Event&)>>>> scriptListeners;

	void OnGameObjectDestroyed(Event& ev)
	{
		GameObject gameObject = ev.GetParam<GameObject>(Events::GameObject::GameObject);
		for (auto& typeScriptListeners : scriptListeners)
		{
			typeScriptListeners.second[gameObject].clear();
		}
	}

	void Initialize()
	{
		// TODO: make it another way (performance + clarity)
		AddListener(Events::GameObject::DESTROYED, OnGameObjectDestroyed);
	}

	void AddListener(EventId eventId, std::function<void(Event&)> const& listener)
	{
		listeners[eventId].push_back(listener);
	}

	void AddScriptListener(EventId eventId, GameObject gameObject, std::function<void(Event&)> const& listener)
	{
		scriptListeners[eventId][gameObject].push_back(listener);
	}

	void FireEvent(Event& event)
	{
		uint32_t type = event.GetType();

		for (auto const& listener : listeners[type])
		{
			listener(event);
		}
		for (auto const& typeListeners : scriptListeners[type])
		{
			for (auto const& listener : typeListeners.second)
			{
				listener(event);
			}
		}
	}

	void FireEvent(EventId eventId)
	{
		Event event(eventId);

		for (auto const& listener : listeners[eventId])
		{
			listener(event);
		}
		for (auto const& typeListeners : scriptListeners[eventId])
		{
			for (auto const& listener : typeListeners.second)
			{
				listener(event);
			}
		}
	}


	void FireEventTo(GameObject gameObject, Event& event)
	{
		uint32_t type = event.GetType();
		for (auto const& listener : scriptListeners[type][gameObject])
		{
			listener(event);
		}
	}
	void FireEventTo(GameObject gameObject, EventId eventId)
	{
		Event event(eventId);
		for (auto const& listener : scriptListeners[eventId][gameObject])
		{
			listener(event);
		}
	}
}
