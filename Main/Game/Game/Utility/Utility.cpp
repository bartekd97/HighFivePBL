#include <fstream>
#include <sstream>
#include "Utility.h"
#include "Logger.h"

bool Utility::ReadTextFile(std::string filename, std::string& content)
{
	static char linebuff[512];
	std::ifstream file(filename);
	if (file.good())
	{
		std::stringstream ss;
		while (!file.eof())
		{
			file.getline(linebuff, 512);
			ss << linebuff << '\n';
		}
		file.close();
		content = ss.str();
		return true;
	}
	else
	{
		LogWarning("Utility::ReadTextFile(): Cannot read file: {}", filename);
		return false;
	}
}

std::vector<std::string> Utility::StringSplit(std::string& string, char separator)
{
	std::vector<std::string> strings;
	std::istringstream f(string);
	std::string s;
	while (getline(f, s, separator)) {
		strings.push_back(s);
	}
	return strings;
}

bool Utility::TryConvertStringToVec2(std::string& string, glm::vec2& vec)
{
	auto floats = StringSplit(string, ',');
	if (floats.size() != 2) return false;
	try
	{
		vec.x = std::stof(floats[0].c_str(), NULL);
		vec.y = std::stof(floats[1].c_str(), NULL);
	}
	catch (std::invalid_argument ex)
	{
		return false;
	}
	return true;
}

bool Utility::TryConvertStringToVec3(std::string& string, glm::vec3& vec)
{
	auto floats = StringSplit(string, ',');
	if (floats.size() != 3) return false;
	try
	{
		vec.x = std::stof(floats[0].c_str(), NULL);
		vec.y = std::stof(floats[1].c_str(), NULL);
		vec.z = std::stof(floats[2].c_str(), NULL);
	}
	catch (std::invalid_argument ex)
	{
		return false;
	}
	return true;
}

bool Utility::TryConvertStringToVec4(std::string& string, glm::vec4& vec)
{
	auto floats = StringSplit(string, ',');
	if (floats.size() != 4) return false;
	try
	{
		vec.x = std::stof(floats[0].c_str(), NULL);
		vec.y = std::stof(floats[1].c_str(), NULL);
		vec.z = std::stof(floats[2].c_str(), NULL);
		vec.w = std::stof(floats[3].c_str(), NULL);
	}
	catch (std::invalid_argument ex)
	{
		return false;
	}
	return true;
}

bool Utility::TryConvertStringToFloat(std::string& string, float& value)
{
	try { value = std::stof(string.c_str(), NULL);}
	catch (std::invalid_argument ex) { return false; }
	return true;
}

std::uint32_t Utility::HashString(char const* s, std::size_t count)
{
	return fnv1a_32(s, count);
}

float Utility::GetDistanceBetweenPointAndSegment(glm::vec2& point, glm::vec2& s1, glm::vec2& s2)
{
	float A = point.x - s1.x;
	float B = point.y - s1.y;
	float C = s2.x - s1.x;
	float D = s2.y - s1.y;

	float p = A * C + B * D;
	float lenSquare = C * C + D * D;
	float parameter = p / lenSquare;

	float shortestX;
	float shortestY;

	if (parameter < 0)
	{
		shortestX = s1.x;
		shortestY = s1.y;
	}
	else if (parameter > 1)
	{
		shortestX = s2.x;
		shortestY = s2.y;
	}
	else
	{
		shortestX = s1.x + parameter * C;
		shortestY = s1.y + parameter * D;
	}

	float distance = glm::sqrt((point.x - shortestX) * (point.x - shortestX) + (point.y - shortestY) * (point.y - shortestY));
	return distance;
}

glm::vec2 Utility::GetSegmentsCommonPoint(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d)
{
	float
		dXab = b.x - a.x,
		dYab = b.y - a.y,

		dXac = c.x - a.x,
		dYac = c.y - a.y,

		dXcd = d.x - c.x,
		dYcd = d.y - c.y;

	float t1 = (dXac * dYcd - dYac * dXcd) / (dXab * dYcd - dYab * dXcd);
	float t2 = (dXac * dYab - dYac * dXab) / (dXab * dYcd - dYab * dXcd);

	if (t1 < 0.0f || t1 > 1.0f || t2 < 0.0f || t2 > 1.0f)
		return { 0.0f, 0.0f };

	return glm::mix(a, b, t1);
}
