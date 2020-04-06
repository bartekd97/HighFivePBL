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
