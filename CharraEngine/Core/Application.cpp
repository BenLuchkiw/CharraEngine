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

	Application::Application(iVec2 screenPos, iVec2  screenSize, const std::string& appName)
	: m_eventHandler(),
	  m_dummy(appName, &m_eventHandler),
	  m_renderer(appName, screenSize, screenPos, &m_eventHandler),
	  m_appName(appName)
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
		const double timePerFrame = 16.6666f;
		double frameStart = Timer::getCurrentTime();
		double deltaTime = frameStart;

		ScaledTimer timer(1.0f);

		while(!Platform::shouldAppQuit())
		{
			m_renderer.draw();

			auto message = m_eventHandler.getTextBuffer();
			if(message.size() > 0)
			{
				CHARRA_LOG_INFO(true, message);
				m_eventHandler.resetTextBuffer();
			}

			deltaTime = timer.getTimeDiff(timePerFrame);

			std::ostringstream stream;
			stream.precision(2);
			stream << "Delta time: " << deltaTime << " fps: " << (1000.0f / deltaTime) << "\n";

			CHARRA_LOG_INFO(true, stream.str());

			Platform::pollEvents();
		}
	}
};