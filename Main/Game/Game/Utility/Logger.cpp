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

	const std::string COMMON_FILENAME = "logs/all.log";
	const std::string INFO_FILENAME = "logs/info.log";
	const std::string WARNING_FILENAME = "logs/warning.log";
	const std::string ERROR_FILENAME = "logs/error.log";

	std::ofstream COMMON_FILE;
	std::ofstream INFO_FILE;
	std::ofstream WARNING_FILE;
	std::ofstream ERROR_FILE;

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

	void LogFile(std::ofstream& filewriter, std::string message)
	{
		filewriter << message << std::endl;
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
	COMMON_FILE.open(COMMON_FILENAME, std::ios_base::trunc);
	INFO_FILE.open(INFO_FILENAME, std::ios_base::trunc);
	WARNING_FILE.open(WARNING_FILENAME, std::ios_base::trunc);
	ERROR_FILE.open(ERROR_FILENAME, std::ios_base::trunc);


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