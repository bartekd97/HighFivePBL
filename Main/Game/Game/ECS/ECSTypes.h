#pragma once

#include <bitset>
#include <cstdint>

using GameObject = std::uint32_t;
const GameObject MAX_GAMEOBJECTS = 5000;
const GameObject NULL_GAMEOBJECT = 0xFFFFFFFF;
using ComponentType = std::uint8_t;
const ComponentType MAX_COMPONENTS = 32;
using Signature = std::bitset<MAX_COMPONENTS>;
const unsigned int MAX_GAMEOBJECT_NAME_LENGTH = 25;
using FrameCounter = std::uint64_t;
