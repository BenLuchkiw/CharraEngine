#include "Window.hpp"

#include "Math/MathFunctions.hpp"
#include "Renderer/Renderer.hpp"
#include "Platform/Platform.hpp"

#include "Vulkan/vulkan.h"

namespace Charra
{
	WindowDummy::WindowDummy(iVec2 size, iVec2 position, const std::string& name, Window* window)
	{
		window->m_windowID = Platform::createWindow(name, size, position);
	}

	Window::Window(iVec2 size, iVec2 position, const std::string& name, Renderer* rendererRef)
	: m_dummy(size, position, name, this),
	  m_rendererRef(rendererRef),
	  m_windowSize(size),
	  m_windowName(name),
	  m_swapchain(rendererRef->getDevice(), rendererRef->getInstance()),
	  m_renderpass(rendererRef->getDevice(), m_swapchain.getSurfaceFormat()),
	  m_imageWaitSemaphore(rendererRef->getDevice()),
	  m_material(rendererRef->getDevice(), m_renderpass, "SimpleVertex", "SimpleFragment")
	{
		m_swapchain.passRenderpass(&m_renderpass);
		
		m_orthographicMatrix = getOrthographicMatrix(100, 0, 0, static_cast<float>(size.width),
														  0, static_cast<float>(size.height));

		rendererRef->getEventHandler()->registerEventCallback(m_windowID, EventType::WINDOW_RESIZE, InputCode::NO_EVENT, &Window::resizeCallback, this);
	}

	Window::~Window()
	{
		vkDeviceWaitIdle(m_rendererRef->getDevice().getDevice());
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