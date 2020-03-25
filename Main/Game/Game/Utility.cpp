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
