#pragma once

#include "Math/MathTypes.hpp"

#include "AbstractWidget.hpp"

namespace Charra {
namespace GUI
{
    class TaskbarWidget : public AbstractWidget
	{
	public:

	private:
		fVec4 borderColour;
		fVec4 backgroundColour;

		std::string title;
	};
} // GUI
} // Charra