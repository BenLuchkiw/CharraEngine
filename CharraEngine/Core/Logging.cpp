#include "Logging.hpp"

namespace Charra
{
	void logMessage(const std::string& message, Platform::ConsoleColours colour)
	{
		Platform::printMessage(message + "\n", colour);
	}

	void logMessage(const std::string& message, const std::string& file, const std::string& line, Platform::ConsoleColours colour)
	{
		std::string output;
		output += "File: " + file + " Line: " + line + "\n" + message + "\n";
		Platform::printMessage(output, colour);
	}
}