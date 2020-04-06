#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

#define M_PI 3.1415926f
#define rad2deg(x) ((x)/M_PI)*180.0f
#define deg2rad(x) ((x)/180.0f)*M_PI
#define nullableString(s) s ? s : ""


namespace Utility {
	bool ReadTextFile(std::string filename, std::string& content);
	std::vector<std::string> StringSplit(std::string& string, char separator);

	// 3 floats, coma separated
	bool TryConvertStringToVec3(std::string& string, glm::vec3& vec);
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
