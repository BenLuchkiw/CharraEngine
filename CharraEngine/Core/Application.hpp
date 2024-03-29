#pragma once

#include <string>

#include "Core/Events.hpp"
#include "Math/mathTypes.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/Window.hpp"

namespace Charra
{
	class Dummy // This is just so that initPlatformWin32 can be run before m_renderer constructor
	{
	public:
		Dummy(const std::string& appName, Events* eventHandlerRef);
	};

	class Application
	{
	public:
		// platform is passed by value so that this class can be platform agnostic
		Application(const std::string& appName);
		~Application();

		Events* getEventHandler() { return &m_eventHandler; }
		Renderer* getRendererRef() { return &m_renderer; }
		WindowManager* getWindowManager() { return &m_windowManager; }

		void run();

		bool shouldQuit();

	private: // Methods
	
	private: // Members
		Events m_eventHandler;
		Dummy m_dummy;
		Renderer m_renderer;
		WindowManager m_windowManager;

		std::string m_appName;

	};
};