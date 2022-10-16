#include "Window.hpp"

namespace Charra
{
	Window::Window(iVec2 size, const std::string& name, Device& deviceRef, Instance& instanceRef, VkVertexInputBindingDescription vertAttribs, std::vector<VkVertexInputAttributeDescription> attribDesc)
	: m_windowSize(size),
	  m_windowName(name),
	  m_swapchain(deviceRef, instanceRef),
	  m_renderpass(deviceRef, m_swapchain.getSurfaceFormat()),
	  m_imageWaitSemaphore(deviceRef),
	  m_material(deviceRef, m_renderpass, vertAttribs, attribDesc, "SimpleVertex", "SimpleFragment")
	{
		m_swapchain.passRenderpass(&m_renderpass);
	}	
}