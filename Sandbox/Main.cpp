#include "CharraEngine/Core/Application.hpp"
#include "CharraEngine/Renderer/Renderer.hpp"
#include "CharraEngine/Math/MathTypes.hpp"


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


int main()
{
	Charra::Application application({15,15}, {400, 400}, "Charra Test");

	Charra::Renderer* renderer = application.getRendererRef();
	Charra::Events* events = application.getEventHandler();

	Object obj1(Charra::fVec3(100.0f, 150.0f, 20.0f), Charra::fVec2(20.0f, 40.0f), Charra::fVec4(1.0f, 1.0f, 0.0f, 0.9f), renderer);
	Object obj2(Charra::fVec3(100.0f, 100.0f, 20.0f), Charra::fVec2(20.0f, 40.0f), Charra::fVec4(1.0f, 0.0f, 0.0f, 0.9f), renderer);

	while (!application.shouldQuit())
	{
		if(events->isButtonDown(Charra::InputCode::W_KEY))
		{
			obj1.m_pos.y -= 1;
		}
		if(events->isButtonDown(Charra::InputCode::A_KEY))
		{
			obj1.m_pos.x -= 1;
		}
		if(events->isButtonDown(Charra::InputCode::S_KEY))
		{
			obj1.m_pos.y += 1;
		}
		if(events->isButtonDown(Charra::InputCode::D_KEY))
		{
			obj1.m_pos.x +=1;
		}
		obj1.draw();
		obj2.draw();
		
		application.run();
	}
}