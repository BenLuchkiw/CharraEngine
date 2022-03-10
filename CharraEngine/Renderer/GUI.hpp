#pragma once

#include "Vulkan/Swapchain.hpp"
#include "Vulkan/Syncronization.hpp"
#include "Vulkan/Renderpass.hpp"
#include "Vulkan/Images.hpp"

#include <vector>

namespace Charra
{
	class GUI
	{
	public:
		GUI(Instance* instanceRef, Device* deviceRef, Renderpass* renderpass);
		~GUI();

		void draw();

	private: // Mehthods

	private: // Members
		Renderpass* m_renderpassRef;
		
		Swapchain swapchain;
		Images images;
		Semaphore imageAvailable;
	};
}