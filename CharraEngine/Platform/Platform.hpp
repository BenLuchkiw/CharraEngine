#pragma once

#include <memory>
#include <vector>
#include <string>

#include "Math/MathTypes.hpp"

#include "vulkan/vulkan.h"

namespace Charra
{
	class Events;

	namespace Platform
	{
		void initPlatformWin32(const char* applicationName, Events *eventHandler);
		void shutdownPlatformWin32();

		enum class ConsoleColours
		{
			RED,
			GREEN,
			BLUE,
			YELLOW,
			PURPLE,
			CYAN,
			WHITE,
			BRIGHT_RED,
			BRIGHT_GREEN,
			BRIGHT_BLUE,
			BRIGHT_YELLOW,
			BRIGHT_PURPLE,
			BRIGHT_CYAN,
			BRIGHT_WHITE
		};

		std::vector<const char*> getExtensions();
		// This retrieves the swapchain from the most recent window that was created
		VkSurfaceKHR getSurface(VkInstance instance);

		std::vector<char> readFile(const char* fileName, uint32_t* fileSize);
		
		// Returns window ID
		uint32_t createWindow(const std::string& windowName, iVec2 size, iVec2 position);
		
		// 0X0x80000000 is meant to replace WC_USEDEFAULT to avoid dirty casts
		inline void createWindow(const std::string& windowName, iVec2 size) { createWindow(windowName, size, {0x80000000, 0x80000000}); }
		inline void createWindow(const std::string& windowName) { createWindow(windowName, {0x80000000, 0x80000000}, {0x80000000, 0x80000000}); }

		void destroyWindow(uint32_t windowIndex);

		void fullscreen(int windowIndex, bool borderless);
		void pollEvents();
		bool shouldAppQuit();


		void printMessage(const std::string& message, ConsoleColours colour);
	};
}