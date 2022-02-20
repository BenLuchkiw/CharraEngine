#pragma once

#include "Core/Events.hpp"

namespace Charra
{
	class Application
	{
	public:
		// platform is passed by value so that this class can be platform agnostic
		Application();
		~Application();

		Events* getEventHandler() { return &m_eventHandler; }

		void run();
	private: // Methods
	
	private: // Members
		Events m_eventHandler;
	};
};