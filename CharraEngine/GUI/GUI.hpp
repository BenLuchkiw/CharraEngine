#pragma once

#include "Math/MathTypes.hpp"

#include "Widgets/Window.hpp"

#include <array>

namespace Charra
{
namespace GUI
{
	class GUIApplication
	{
	public:
		GUIApplication();
		~GUIApplication();

		void addWindow();
		void destroyWindow();

	private:
		// Having more than 20 windows is absurd for this project
		// This also relieves the concern of copying or moving stuff
		std::vector<std::unique_ptr<GUI::Window>> windows;
	};

} // GUI
} // Charra