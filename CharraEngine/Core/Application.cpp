#include "Application.hpp"

#include "Platform/Platform.hpp"
#include "Platform/Timer.hpp"
#include "Core/Logging.hpp"

#include <string>
#include <sstream>

#include "Renderer/Renderer.hpp"
#include "Vulkan/Pipeline.hpp"

namespace Charra
{
	Dummy::Dummy(const std::string& appName, Events* eventHandlerRef)
	{
		// TODO platform specific, may be able to only have one initPlatform function...
		Platform::initPlatformWin32(appName.data(), eventHandlerRef);
	}


	// m_renderer is created with a pointer to an uninitialized WindowManager, this is fine
	// because m_renderer does not use window manager on creation, only on main loop
	Application::Application(const std::string& appName)
	: m_eventHandler(),
	  m_dummy(appName, &m_eventHandler),
	  m_renderer(&m_eventHandler, &m_windowManager),
	  m_appName(appName),
	  m_windowManager(&m_renderer)
	{
		Timer::initTimer();
	}

	Application::~Application()
	{
		// TODO platform specific, may be able to only have one shutdownPlatform function...
		Platform::shutdownPlatformWin32();
	}

	void Application::run()
	{
		auto message = m_eventHandler.getTextBuffer();
		if(message.size() > 0)
		{
			CHARRA_LOG_INFO(true, message);
			m_eventHandler.resetTextBuffer();
		}

		Platform::pollEvents();
	}

	bool Application::shouldQuit()
	{
		return Platform::shouldAppQuit();
	}
};