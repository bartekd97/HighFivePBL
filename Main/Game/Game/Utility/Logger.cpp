#define _CRT_SECURE_NO_WARNINGS

#include <chrono>
#include <fstream>
#include <ctime>
#include <iostream>
#include <filesystem>
#include "Logger.h"

#ifdef HF_LOGGER

// variables
namespace Logger {

	const std::string LOGS_DIRECTORY = "logs";

	const std::string COMMON_FILE = "logs/all.log";

	const std::string INFO_FILE = "logs/info.log";
	const std::string WARNING_FILE = "logs/warning.log";
	const std::string ERROR_FILE = "logs/error.log";

	bool Initialized = false;
}

// privates
namespace Logger {
	// https://stackoverflow.com/questions/16077299/how-to-print-current-time-with-milliseconds-using-c-c11
	std::string GetTimeMark()
	{
		using std::chrono::system_clock;
		auto currentTime = std::chrono::system_clock::now();
		char buffer[80];

		auto transformed = currentTime.time_since_epoch().count() / 1000000;

		auto millis = transformed % 1000;

		std::time_t tt;
		tt = system_clock::to_time_t(currentTime);
		auto timeinfo = localtime(&tt);
		strftime(buffer, 80, "%F %H:%M:%S", timeinfo);
		sprintf(buffer, "%s:%03d", buffer, (int)millis);

		return "[" + std::string(buffer) + "]";
	}

	void LogFile(std::string filename, std::string message)
	{
		std::ofstream writer;
		writer.open(filename, std::ios_base::app | std::ios_base::out);
		if (!writer.is_open())
		{
			std::cout << "Cannot write log to " << filename << std::endl;
			return;
		}
		writer << message << std::endl;
		writer.close();
	}
}

// publics
void Logger::Initialize()
{
	if (Initialized)
	{
		LogWarning("Logger::Initialize(): Already initialized");
		return;
	}

	std::filesystem::create_directory(LOGS_DIRECTORY);
	std::ofstream(COMMON_FILE, std::ios_base::trunc);
	std::ofstream(INFO_FILE, std::ios_base::trunc);
	std::ofstream(WARNING_FILE, std::ios_base::trunc);
	std::ofstream(ERROR_FILE, std::ios_base::trunc);

	Initialized = true;
}

void Logger::Info(std::string message)
{
	std::cout << "INFO: " << message << std::endl;
	LogFile(INFO_FILE, GetTimeMark() + " " + message);
	LogFile(COMMON_FILE, GetTimeMark() + " [INFO] " + message);
}
void Logger::Warning(std::string message)
{
	std::cout << "WARNING: " << message << std::endl;
	LogFile(WARNING_FILE, GetTimeMark() + " " + message);
	LogFile(COMMON_FILE, GetTimeMark() + " [WARNING] " + message);
}
void Logger::Error(std::string message)
{
	std::cout << "ERROR: " << message << std::endl;
	LogFile(ERROR_FILE, GetTimeMark() + " " + message);
	LogFile(COMMON_FILE, GetTimeMark() + " [ERROR] " + message);
}

#endif