#include "Window.hpp"

#include "Math/MathFunctions.hpp"

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
		
		m_orthographicMatrix = getOrthographicMatrix(100, 0, 0, static_cast<float>(size.width),
														  0, static_cast<float>(size.height));
	}	

	bool Window::resizeCallback(EventType type, InputCode code, uint64_t data, void* privateData)
		{
			VkExtent2D size;
			size.width = UPPER_UINT64(data);
			size.height = LOWER_UINT64(data);

			Window* windowRef = static_cast<Window*>(privateData);

			windowRef->getSwapchain().setPixelExtent(size.width, size.height);
			windowRef->getSwapchain().invalidateSwapchain();

			windowRef->getOrthoMatrix() = getOrthographicMatrix(100, 0, 0, static_cast<float>(size.width),
														  0, static_cast<float>(size.height));
			windowRef->getWindowSize() = {size.width, size.height};
		
			return true;
		}
}