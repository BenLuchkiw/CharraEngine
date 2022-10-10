#pragma once

#include "Math/MathTypes.hpp"

namespace Charra
{
	class GUI_Container
	{
	public:
		GUI_Container(uint32_t id);
		~GUI_Container();

		GUI_Container& hide();
		GUI_Container& show();
		GUI_Container& setPosition(fVec3 position);
		GUI_Container& setColour(fVec4 colour);

		bool isHidden() 	 { return m_hidden; }
		fVec3& getPosition() { return m_position; }
		fVec4& getColour()   { return m_colour; }
		uint32_t getID()	 { return m_container_ID; }

	private: // Methods

	private: // Members
		uint32_t m_container_ID;

		fVec3 m_position;
		fVec4 m_colour;
		bool m_hidden = false;
	};
}