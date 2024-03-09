#include "CharraEngine/Core/Application.hpp"
#include "CharraEngine/Renderer/Renderer.hpp"
#include "CharraEngine/Math/MathTypes.hpp"
#include "CharraEngine/Math/RendererTypes.hpp"
#include "CharraEngine/Renderer/Window.hpp"
#include "CharraEngine/Renderer/Canvas.hpp"
#include "CharraEngine/Draw2D/Quad.hpp"
#include "CharraEngine/Platform/Timer.hpp"

#include <vector>

bool resize(Charra::EventType type, Charra::InputCode code, uint64_t data, void* privateData)
{
	//Object objRef = static_cast<Object*>(privateData);



	return true;
}

bool window1Closed = false;
bool window2Closed = false;

bool close(Charra::EventType type, Charra::InputCode code, uint64_t data, void* privateData)
{
	if(reinterpret_cast<uint64_t>(privateData) == 1)
	{
		window1Closed = true;
	}
	if(reinterpret_cast<uint64_t>(privateData) == 2)
	{
		window2Closed = true;
	}

	return true;
}

int main()
{
	Charra::Application application("Charra Test");

	Charra::Renderer* renderer = application.getRendererRef();
	Charra::Events* events = application.getEventHandler();
	Charra::WindowManager* windowManager = application.getWindowManager();

	Charra::WindowID window1 = windowManager->createWindow(Charra::iVec2(500, 500), Charra::iVec2(0,0), "Charra Test");
	Charra::WindowID window2 = windowManager->createWindow(Charra::iVec2(500, 500), Charra::iVec2(500,0), "Charra Test2");

	Charra::Canvas* canvas1 = windowManager->createCanvas();
	Charra::Canvas* canvas2 = windowManager->createCanvas();

	windowManager->assignCanvasToWindow(*canvas1, window1);
	windowManager->assignCanvasToWindow(*canvas2, window2);

	events->registerEventCallback(0, Charra::EventType::WINDOW_CLOSE, Charra::InputCode::NO_EVENT, &close, (void*)1);
	events->registerEventCallback(1, Charra::EventType::WINDOW_CLOSE, Charra::InputCode::NO_EVENT, &close, (void*)2);

	Charra::Quad quad1(Charra::fVec3(400.0f, 400.0f, 20.0f), Charra::fVec2(100.0f, 100.0f), Charra::fVec4(1.0f, 1.0f, 0.0f, 0.9f));
	Charra::Quad quad2(Charra::fVec3(200.0f, 400.0f, 20.0f), Charra::fVec2(100.0f, 100.0f), Charra::fVec4(1.0f, 1.0f, 0.5f, 0.9f));

	double time;
	while (!application.shouldQuit())
	{
		time = Charra::Timer::getCurrentTime();

		if(!window1Closed)
		{
			Charra::Mat4X4 mat = windowManager->getOrthoMatrix(window1);
			quad1.updateVertices(mat);
			canvas1->drawQuad(quad1.getVertices());
		}
		else
		{
			windowManager->destroyWindow(window1);
		}
		if(!window2Closed)
		{
			Charra::Mat4X4 mat = windowManager->getOrthoMatrix(window2);
			quad2.updateVertices(mat);
			canvas2->drawQuad(quad2.getVertices());
		}
		else
		{
			windowManager->destroyWindow(window2);
		}
		if(window2Closed || window1Closed)
		{
			quad1.getCoords().x += 0.001;
			quad2.getCoords().y += 0.001;
		}

		renderer->draw();
		application.run();


		// 60 fps
		time = Charra::Timer::getCurrentTime() - time;
		time = 1000.0 / 60.0 - time;
		Charra::Timer::wait(1000.0 / 60.0 - time);
	}
}