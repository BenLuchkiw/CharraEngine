#pragma once

#include <string>
#include <iostream>
#include <cstdint>
#include "Platform/Platform.hpp"\

namespace Charra
{
	void logMessage(const std::string& message, Platform::ConsoleColours colour);
	void logMessage(const std::string& message, const std::string& file, const std::string& line, Charra::Platform::ConsoleColours colour);
}


#define CHARRA_LOG_INFO(condition, message) 						\
if(condition)														\
{ 																	\
	Charra::logMessage(message, Platform::ConsoleColours::WHITE); 	\
}

#define CHARRA_LOG_WARNING(condition, message) 																			\
if(condition)																											\
{																														\
	Charra::logMessage(message, std::string(__FILE__), std::to_string(__LINE__), Platform::ConsoleColours::YELLOW);		\
}

#define CHARRA_LOG_ERROR(condition, message) 																			\
if(condition)																											\
{																														\
	Charra::logMessage(message, std::string(__FILE__), std::to_string(__LINE__), Platform::ConsoleColours::RED);		\
}