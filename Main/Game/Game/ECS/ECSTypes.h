#pragma once

#include <bitset>
#include <cstdint>

using GameObject = std::uint32_t;
const GameObject MAX_GAMEOBJECTS = 5000;
using ComponentType = std::uint8_t;
const ComponentType MAX_COMPONENTS = 32;
using Signature = std::bitset<MAX_COMPONENTS>;
