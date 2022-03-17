#include "Application.hpp"

#include "Platform/Platform.hpp"
#include "Platform/Timer.hpp"
#include "Core/Logging.hpp"

#include <string>
#include <sstream>

// TODO this is temporary
#include "Renderer/Renderer.hpp"
#include "Vulkan/Pipeline.hpp"

namespace Charra
{
	Application::Application()
	: m_eventHandler()
	{
		
	}

	Application::~Application()
	{

	}

	void Application::run()
	{
		Renderer renderer("Charra Engine", &m_eventHandler);

		const double timePerFrame = 16.6666f;
		double frameStart = Timer::getCurrentTime();
		double deltaTime = frameStart;

		ScaledTimer timer(1.0f);

		while(!Platform::shouldAppQuit())
		{
			renderer.draw();

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