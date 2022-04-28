#pragma once

#include "Math/MathTypes.hpp"

#include <vector>
#include <memory>

namespace Charra {
namespace GUI
{
	class AbstractWidget
	{
	public:
		AbstractWidget();
		~AbstractWidget();

		// This will be called to check if mouse is over this widget,
		// Due to the order this will be checked in function implementations can assume focus if this is true
		virtual bool mouseOver(iVec2 cursorPos);

		

		bool inFocus() { return hasFocus; }

	private: // Members
		fVec2 topLeft; // x, y
		fVec2 dimensions; // width, height

		// Coordinates are either relative (0-1),
		// or pixel based with 0,0 being the top left corner of the window.
		bool relativeCoords;

		bool hasFocus;

		// If this widget is destroyed all children need to be destroyed first,
		// then you need to inform the parent that this class is destroyed
		AbstractWidget* parent;
		std::vector<std::unique_ptr<AbstractWidget>> children;

		uint64_t widgetId;
	};
	
} // GUI
} // Charra