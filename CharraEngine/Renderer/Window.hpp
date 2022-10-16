#pragma once

#include <string>

#include "Material.hpp"
#include "Math/MathTypes.hpp"
#include "Vulkan/Renderpass.hpp"
#include "Vulkan/Swapchain.hpp"
#include "Vulkan/Syncronization.hpp"

namespace Charra
{
	class Window
	{
	public:
		Window(iVec2 size, const std::string& name, Device& deviceRef, Instance& instanceRef, VkVertexInputBindingDescription vertAttribs, std::vector<VkVertexInputAttributeDescription> attribDesc);

		iVec2&			getWindowSize() { return m_windowSize; }
		std::string& 	getWindowName() { return m_windowName; }
		Swapchain& 		getSwapchain() 	{ return m_swapchain; }
		Renderpass& 	getRenderpass() { return m_renderpass; }
		Semaphore& 		getSemaphore() 	{ return m_imageWaitSemaphore; }
		Material&		getMaterial()	{ return m_material; }
	private: // Methods

	private: // Members
		iVec2 		m_windowSize;
		std::string m_windowName;
		Swapchain 	m_swapchain;
		Renderpass	m_renderpass;
		Semaphore 	m_imageWaitSemaphore;
		Material 	m_material;
	};
}