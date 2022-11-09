#include "CharraEngine/Core/Application.hpp"
#include "CharraEngine/Renderer/Renderer.hpp"
#include "CharraEngine/Math/MathTypes.hpp"
#include "CharraEngine/Renderer/Window.hpp"

#include <vector>

class Object
{
	public:

	Object(Charra::fVec3 pos, Charra::fVec2 size, Charra::fVec4 colour, Charra::Renderer* renderer)
	: m_renderer(renderer),
	  m_pos(pos),
	  m_size(size),
	  m_colour(colour)
	{

	}

	void draw()
	{
		m_renderer->drawQuad(m_pos, m_size, m_colour);
	}

		Charra::Renderer* m_renderer;
		Charra::fVec3 m_pos;
		Charra::fVec2 m_size;
		Charra::fVec4 m_colour;
};

bool resize(Charra::EventType type, Charra::InputCode code, uint64_t data, void* privateData)
{
	//Object objRef = static_cast<Object*>(privateData);



	return true;
}

int main()
{
	Charra::Application application("Charra Test");

	Charra::Renderer* renderer = application.getRendererRef();
	Charra::Events* events = application.getEventHandler();

	std::vector<Charra::Window> windows;
	windows.emplace_back(Charra::iVec2(500, 500), Charra::iVec2(0,0), "Charra Test", renderer);

	Object obj1(Charra::fVec3(400.0f, 400.0f, 20.0f), Charra::fVec2(100.0f, 100.0f), Charra::fVec4(1.0f, 1.0f, 0.0f, 0.9f), renderer);

	events->registerEventCallback(0, Charra::EventType::WINDOW_RESIZE, Charra::InputCode::NO_EVENT, &resize, &obj1);

	while (!application.shouldQuit())
	{
		
		

		obj1.draw();
		
		renderer->draw(windows);
		application.run();
	}
}