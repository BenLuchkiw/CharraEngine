#include "Window.hpp"

#include "Math/MathFunctions.hpp"
#include "Renderer/Renderer.hpp"
#include "Platform/Platform.hpp"
#include "Vulkan/Allocator/BufferManager.hpp"

#include "Canvas.hpp"

#include "Vulkan/vulkan.h"

namespace Charra
{
	Window::Window(iVec2 size, iVec2 position, const std::string& name, Renderer* rendererRef, uint32_t windowID)
	: m_windowSize(size),
	  m_windowName(name),
	  m_rendererRef(rendererRef),
	  m_swapchain(rendererRef->getDevice(), rendererRef->getInstance()),
	  m_renderpass(rendererRef->getDevice(), m_swapchain.getSurfaceFormat()),
	  m_imageWaitSemaphore(rendererRef->getDevice()),
	  m_renderFinishedSemaphore(rendererRef->getDevice()),
	  m_material(rendererRef->getDevice(), m_renderpass, "SimpleVertex", "SimpleFragment"),
	  m_windowID(windowID)
	{
		m_swapchain.passRenderpass(&m_renderpass);
		
		m_orthographicMatrix = getOrthographicMatrix(100, 0, 0, static_cast<float>(size.width),
														  0, static_cast<float>(size.height));

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

	void Window::assignCanvas(Canvas* canvas)
	{
		m_canvasMap.emplace(canvas->getCanvasID(), canvas);
	}

	void Window::disassociateCanvas(Canvas* canvas)
	{
		m_canvasMap.erase(canvas->getCanvasID());
	}

	void Window::recordCommandBuffers(std::vector<VkSemaphore>* renderSemaphore, std::vector<VkSemaphore>* presentSemaphore, std::vector<VkPipelineStageFlags>* waitStages, uint32_t commandBufIndex)
	{
		m_swapchain.prepareNextImage(&m_imageWaitSemaphore);

		m_rendererRef->getCommandBuffers().beginRenderpass(m_renderpass.getRenderpass(),
															m_swapchain.getFramebuffer(), 
															m_material.getPipeline().getPipeline(),
															m_swapchain.getPixelExtent(),
															commandBufIndex);
	
		VkRect2D scissor;
			scissor.extent = m_swapchain.getPixelExtent();
			scissor.offset = {0,0};

		VkViewport viewportCreateInfo{};
			viewportCreateInfo.x = 0;
			viewportCreateInfo.y = 0;
			viewportCreateInfo.width = static_cast<float>(scissor.extent.width);
			viewportCreateInfo.height = static_cast<float>(scissor.extent.height);
			viewportCreateInfo.minDepth = 0.0f;
			viewportCreateInfo.maxDepth = 1.0f;

		vkCmdSetViewport(m_rendererRef->getCommandBuffers().getCommandBuffer(commandBufIndex), 0, 1, &viewportCreateInfo);
		vkCmdSetScissor(m_rendererRef->getCommandBuffers().getCommandBuffer(commandBufIndex), 0, 1, &scissor);

		for(auto& canvas : m_canvasMap)
		{
			// If there is a device buffer bind those addresses, otherwise bind the staging buffer addresses
			canvas.second->bindBuffers(m_rendererRef->getBufferManager(), m_rendererRef->getCommandBuffers().getCommandBuffer(commandBufIndex));
		}
		m_rendererRef->getCommandBuffers().endRenderpass(commandBufIndex);
		renderSemaphore->push_back(m_imageWaitSemaphore.getSemaphore());
		presentSemaphore->push_back(m_renderFinishedSemaphore.getSemaphore());
			
		waitStages->push_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
	}


	// Window Manager

	WindowManager::WindowManager(Renderer* rendererRef)
	: m_rendererRef(rendererRef)
	{
	}

	WindowManager::~WindowManager()
	{
	}

	WindowID WindowManager::createWindow(iVec2 size, iVec2 position, const std::string& name)
	{
		// Create a window
		WindowID windowID = Platform::createWindow(name, size, position); 
		// Add the window to the unordered map
		m_windows.emplace(std::piecewise_construct, std::forward_as_tuple(windowID), std::forward_as_tuple(size, position, name, m_rendererRef, windowID));		m_rendererRef->getEventHandler()->registerEventCallback(windowID, EventType::WINDOW_RESIZE, InputCode::NO_EVENT, &Window::resizeCallback, &m_windows.at(windowID));

		return windowID;
	}

	void WindowManager::destroyWindow(WindowID ID)
	{
		// Destroy a window
		Platform::destroyWindow(ID);
		// Remove the window from the unordoer map
		m_windows.erase(ID);

	}

	Canvas* WindowManager::createCanvas()
	{
		// Create a canvas, and increment the canvas ID
		CanvasID canvasID = m_canvasIDGen++;
		m_canvases.emplace(canvasID, Canvas());
		m_canvases.at(canvasID).m_ID = canvasID;
		return &m_canvases[canvasID];
	}

	void WindowManager::destroyCanvas(Canvas& canvas)
	{
		// Destroy a canvas
		CanvasID ID = canvas.getCanvasID();
		m_canvases.erase(ID);
	}

	void WindowManager::assignCanvasToWindow(Canvas& canvas, WindowID windowID)
	{
		// Assign a canvas to a window
		m_windows.at(windowID).assignCanvas(&m_canvases.at(canvas.getCanvasID()));
	}

	void WindowManager::disassociateCanvasFromWindow(Canvas& canvas, WindowID windowID)
	{
		// Disassociate a canvas from a window
		m_windows.at(windowID).disassociateCanvas(&m_canvases.at(canvas.getCanvasID()));
	}

	void WindowManager::transferBuffers(BufferManager* bufferManager)
	{
		// Transfer buffers
		for(auto& canvas : m_canvases)
		{
			canvas.second.transferBuffers(bufferManager);
		}
	}

	void WindowManager::recordCommandBuffers(std::vector<VkSemaphore>* renderSemaphore, std::vector<VkSemaphore>* presentSemaphore, std::vector<VkPipelineStageFlags>* waitStages, uint32_t commandBufIndex)
	{
		// Record command buffers
		for(auto& window : m_windows)
		{
			window.second.recordCommandBuffers(renderSemaphore, presentSemaphore, waitStages, commandBufIndex);
		}
	}

	void WindowManager::presentImages()
	{
		// Present images

		VkPresentInfoKHR presentInfo{};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.pNext;
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.swapchainCount = 1;
			presentInfo.pResults;

		for(auto& window : m_windows)
		{
			VkSemaphore semaphore = window.second.getRenderFinishedSemaphore().getSemaphore();
			presentInfo.pWaitSemaphores = &semaphore;
			VkSwapchainKHR swapchains[1] = {window.second.getSwapchain().getSwapchain()};
			presentInfo.pSwapchains = swapchains;
			const uint32_t imageIndices[1] = {window.second.getSwapchain().getImageIndex()};
			presentInfo.pImageIndices = imageIndices;
			vkQueuePresentKHR(m_rendererRef->getDevice().getGraphicsQueue(), &presentInfo);
		}
	}

}