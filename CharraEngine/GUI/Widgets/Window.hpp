#pragma once

#include "AbstractWidget.hpp"

#include "Math/MathTypes.hpp"

namespace Charra {
namespace GUI
{
    class Window
	{
	public:

	private:
		iVec2 position;
		iVec2 dimensions;

		std::vector<std::unique_ptr<AbstractWidget>> widgets;
	};
} // GUI
} // Charra