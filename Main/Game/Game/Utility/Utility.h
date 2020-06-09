#pragma once

#include <string>
#include <sstream>
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

	// 2 floats, coma separated
	bool TryConvertStringToVec2(std::string& string, glm::vec2& vec);
	// 3 floats, coma separated
	bool TryConvertStringToVec3(std::string& string, glm::vec3& vec);
	// 4 floats, coma separated
	bool TryConvertStringToVec4(std::string& string, glm::vec4& vec);
	bool TryConvertStringToFloat(std::string& string, float& value);

	std::uint32_t HashString(char const* s, std::size_t count);

	// Math funkcions

	float GetDistanceBetweenPointAndSegment(glm::vec2& point, glm::vec2& s1, glm::vec2& s2);
	glm::vec2 GetSegmentsCommonPoint(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d);
}


#pragma region std::to_string extensions
namespace std {
	inline string to_string(string val) {
		return val;
	}
	inline string to_string(const char* val) {
		return val;
	}
	inline string to_string(glm::vec2 val) {
		return to_string(val.x) + "," + to_string(val.y);
	}
	inline string to_string(glm::vec3 val) {
		return to_string(val.x) + "," + to_string(val.y) + "," + to_string(val.z);
	}
	inline string to_string(glm::vec4 val) {
		return to_string(val.x) + "," + to_string(val.y) + "," + to_string(val.z) + "," + to_string(val.w);
	}

	template<typename T>
	inline string to_string(vector<T> vec) {
		stringstream ss;
		ss << "{";
		for (int i=0; i<vec.size(); i++)
		{
			if (i > 0) ss << ", ";
			ss << to_string(vec[i]);
		}
		ss << "}";
		return ss.str();
	}
}
#pragma endregion
