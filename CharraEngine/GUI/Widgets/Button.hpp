#pragma once

#include "Math/MathTypes.hpp"

#include "AbstractWidget.hpp"

#include <string> 

namespace Charra {
namespace GUI
{
    // data is either an unknown user defined pointer or it will be nullptr
	typedef void (*PFN_buttonCallback)(void* data);

	class ButtonWidget : public AbstractWidget
	{
	public:
        
        virtual bool mouseOver(iVec2 cursorPos) override;
	private:
		bool highlightOnFocus;
		bool round;
		fVec4 borderColour;
		fVec4 backgroundColour;

		std::string title;

		PFN_buttonCallback callback;
		void* callbackData;
	};
} // GUI
} // Charra