#pragma once

#include <string>
#include "Utility.h"

// Use macros defined below
// use {} to replace parameters
// example: LogWarning("This is {} world", "hello") -> "This is hello world"

#ifdef HF_LOGGER
	#define LogInfo(message, ...) Logger::Info(message, __VA_ARGS__)
	#define LogWarning(message, ...) Logger::Warning(message, __VA_ARGS__)
	#define LogError(message, ...) Logger::Error(message, __VA_ARGS__)
#else
	#define LogInfo(message, ...)
	#define LogWarning(message, ...)
	#define LogError(message, ...)
#endif

// use it only once in main
#ifdef HF_LOGGER
	#define LoggerInitialize() Logger::Initialize()
#else
	#define LoggerInitialize()
#endif

// DON'T USE Logger:: FUNCTIONS DIRECTLY !!!!!
#ifdef HF_LOGGER
namespace Logger {

	void Info(std::string message);
	template<typename T, typename... Targs>
	void Info(std::string message, T arg, Targs... args)
	{
		auto pos = message.find("{}");
		if (pos != std::string::npos)
			message = message.replace(pos, 2, std::to_string(arg));
		Info(message, args...);
	}


	void Warning(std::string message);
	template<typename T, typename... Targs>
	void Warning(std::string message, T arg, Targs... args)
	{
		auto pos = message.find("{}");
		if (pos != std::string::npos)
			message = message.replace(pos, 2, std::to_string(arg));
		Warning(message, args...);
	}


	void Error(std::string message);
	template<typename T, typename... Targs>
	void Error(std::string message, T arg, Targs... args)
	{
		auto pos = message.find("{}");
		if (pos != std::string::npos)
			message = message.replace(pos, 2, std::to_string(arg));
		Error(message, args...);
	}


	void Initialize();
}
#endif