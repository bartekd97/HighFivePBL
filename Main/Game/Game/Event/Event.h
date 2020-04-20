#pragma once

#include <any>
#include <unordered_map>

using EventId = std::uint32_t;
using ParamId = std::uint32_t;

class Event
{
public:
	Event() = delete;

	explicit Event(EventId type)
		: type(type)
	{}

	template<typename T>
	void SetParam(EventId id, T value)
	{
		data[id] = value;
	}

	template<typename T>
	T GetParam(EventId id)
	{
		return std::any_cast<T>(data[id]);
	}

	EventId GetType() const
	{
		return type;
	}

private:
	EventId type{};
	std::unordered_map<EventId, std::any> data{};
};