#include "Containers.hpp"

namespace Charra
{
	GUI_Container::GUI_Container(uint32_t id)
	: m_container_ID(id),
	  m_position(),
	  m_colour()
	{
		
	}

	GUI_Container::~GUI_Container()
	{

	}

	GUI_Container& GUI_Container::hide()
	{
		m_hidden = true;
		return *this;
	}

	GUI_Container& GUI_Container::show()
	{
		m_hidden = false;
		return *this;
	}

	GUI_Container& GUI_Container::setPosition(fVec3 position)
	{
		m_position = position;
		return *this;
	}

	GUI_Container& GUI_Container::setColour(fVec4 colour)
	{
		m_colour = colour;
		return *this;
	}
}