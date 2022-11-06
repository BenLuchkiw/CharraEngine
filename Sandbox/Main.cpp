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

bool move(Charra::EventType type, Charra::InputCode code, uint64_t data, void* privateData)
{
	Object* obj = static_cast<Object*>(privateData);

	switch(code)
	{
	case Charra::InputCode::W_KEY:
		obj->m_pos.y -= 1;
		break;
	case Charra::InputCode::A_KEY:
		obj->m_pos.x -= 1;
		break;
	case Charra::InputCode::S_KEY:
		obj->m_pos.y += 1;
		break;
	case Charra::InputCode::D_KEY:
		obj->m_pos.x += 1;
		break;
	default:
		break;
	}
	return true;
}


int main()
{
	Charra::Application application({15,15}, {400, 400}, "Charra Test");

	Charra::Renderer* renderer = application.getRendererRef();
	Charra::Events* events = application.getEventHandler();

	Object obj1(Charra::fVec3(100.0f, 150.0f, 20.0f), Charra::fVec2(20.0f, 40.0f), Charra::fVec4(1.0f, 1.0f, 0.0f, 0.9f), renderer);
	Object obj2(Charra::fVec3(100.0f, 100.0f, 20.0f), Charra::fVec2(20.0f, 40.0f), Charra::fVec4(1.0f, 0.0f, 0.0f, 0.9f), renderer);

	events->registerEventCallback(0, Charra::EventType::KEY_DOWN, Charra::InputCode::W_KEY, &move, &obj1);
	events->registerEventCallback(0, Charra::EventType::KEY_DOWN, Charra::InputCode::A_KEY, &move, &obj1);
	events->registerEventCallback(0, Charra::EventType::KEY_DOWN, Charra::InputCode::S_KEY, &move, &obj1);
	events->registerEventCallback(0, Charra::EventType::KEY_DOWN, Charra::InputCode::D_KEY, &move, &obj1);

	while (!application.shouldQuit())
	{
		obj1.draw();
		obj2.draw();
		
		application.run();
	}
}