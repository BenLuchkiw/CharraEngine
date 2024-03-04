#include "CharraEngine/Core/Application.hpp"
#include "CharraEngine/Renderer/Renderer.hpp"
#include "CharraEngine/Math/MathTypes.hpp"
#include "CharraEngine/Renderer/Window.hpp"
#include "CharraEngine/Renderer/Canvas.hpp"

#include <vector>

class Object
{
	public:

	Object(Charra::fVec3 pos, Charra::fVec2 size, Charra::fVec4 colour, Charra::Canvas* canvasRef)
	: m_canvasRef(canvasRef),
	  m_pos(pos),
	  m_size(size),
	  m_colour(colour)
	{

	}

	void draw()
	{
		m_canvasRef->drawQuad(m_pos, m_size, m_colour);
	}

		Charra::Canvas* m_canvasRef;
		Charra::fVec3 m_pos;
		Charra::fVec2 m_size;
		Charra::fVec4 m_colour;
};

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

	Object obj1(Charra::fVec3(400.0f, 400.0f, 20.0f), Charra::fVec2(100.0f, 100.0f), Charra::fVec4(1.0f, 1.0f, 0.0f, 0.9f), canvas1);
	Object obj2(Charra::fVec3(200.0f, 400.0f, 20.0f), Charra::fVec2(100.0f, 100.0f), Charra::fVec4(1.0f, 1.0f, 0.5f, 0.9f), canvas1);

	events->registerEventCallback(0, Charra::EventType::WINDOW_CLOSE, Charra::InputCode::NO_EVENT, &close, (void*)1);
	events->registerEventCallback(1, Charra::EventType::WINDOW_CLOSE, Charra::InputCode::NO_EVENT, &close, (void*)2);

	while (!application.shouldQuit())
	{
		if(!window1Closed)
		{
			obj1.draw();
		}
		if(!window2Closed)
		{
			obj2.draw();
		}
		if(window2Closed || window1Closed)
		{
			obj1.m_colour.r -= 0.001;
			obj2.m_colour.x += 0.001;
		}

		renderer->draw();
		application.run();
	}
}