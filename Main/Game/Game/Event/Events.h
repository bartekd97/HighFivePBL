#pragma once

#include <bitset>

#include "Event.h"

constexpr std::uint32_t fnv1a_32(char const* s, std::size_t count)
{
	return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u;
}

constexpr std::uint32_t operator "" _hash(char const* s, std::size_t count)
{
	return fnv1a_32(s, count);
}

namespace Events::Test
{
	const EventId TICK = "Events::Test::TICK"_hash;
}
