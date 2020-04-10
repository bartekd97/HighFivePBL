#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

#define M_PI 3.1415926f
#define rad2deg(x) ((x)/M_PI)*180.0f
#define deg2rad(x) ((x)/180.0f)*M_PI
#define nullableString(s) s ? s : ""

constexpr std::uint32_t fnv1a_32(char const* s, std::size_t count)
{
	return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u;
}

constexpr std::uint32_t operator "" _hash(char const* s, std::size_t count)
{
	return fnv1a_32(s, count);
}

namespace Utility {
	bool ReadTextFile(std::string filename, std::string& content);
	std::vector<std::string> StringSplit(std::string& string, char separator);

	// 3 floats, coma separated
	bool TryConvertStringToVec3(std::string& string, glm::vec3& vec);
	bool TryConvertStringToFloat(std::string& string, float& value);

	std::uint32_t HashString(char const* s, std::size_t count);
}


#pragma region std::to_string extensions
namespace std {
	inline string to_string(string val) {
		return val;
	}
	inline string to_string(const char* val) {
		return val;
	}
}
#pragma endregion
