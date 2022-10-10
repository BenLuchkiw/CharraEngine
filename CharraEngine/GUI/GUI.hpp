#pragma once

#include "Containers.hpp"

#include <map>

namespace Charra
{
	class GUI
	{
	public:
		GUI();
		~GUI();

		GUI_Container& createContainer();
		GUI_Container& getContainer(uint32_t containerID) { return m_containers.find(containerID)->second; }
		void deleteContainer(uint32_t containerID) { m_containers.erase(m_containers.find(containerID)); }

	private: // Methods

	private: // Members
		std::map<uint32_t, GUI_Container> m_containers;

		// TODO add redundancy for if m_BaseID = UINT32_MAX even though it is very unlikely
		uint32_t m_BaseID = 0;
	};
}