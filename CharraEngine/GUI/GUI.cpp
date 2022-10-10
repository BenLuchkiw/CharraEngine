#include "GUI.hpp"

namespace Charra
{
	GUI::GUI()
	: m_containers()
	{
		
	}

	GUI::~GUI()
	{

	}

	GUI_Container& GUI::createContainer()
	{
		m_BaseID++;
		return m_containers.emplace(std::make_pair(m_BaseID, GUI_Container(m_BaseID))).first->second;
	}
}