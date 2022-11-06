#include "Renderer.hpp"

#include "Platform/Platform.hpp"
#include "Core/Logging.hpp"
#include "Math/RendererTypes.hpp"
#include "Math/MathFunctions.hpp"

#include "vulkan/vulkan.h"

namespace Charra
{
		Renderer::Renderer(const std::string& mainWindowName, iVec2 windowSize, iVec2 windowPos, Events* eventHandler)
		: m_eventHandlerRef(eventHandler),
		  m_instance(),
		  m_device(m_instance),
		  m_commandBuffers(m_device, 2, CommandBufferType::GRAPHICS),
		  m_allocator(m_device),
		  m_transferFinishedSemaphore(m_device),
		  m_renderFinishedSemaphore(m_device),
		  m_renderFinishedFence(m_device)
	{
		// TODO max size/fix error
		m_windows.reserve(20);

		Platform::createWindow(mainWindowName, windowSize, windowPos);
		m_windows.emplace_back(windowSize, mainWindowName, m_device, m_instance, m_vertAttribs, m_attribDesc);
		m_eventHandlerRef->registerEventCallback(0, EventType::WINDOW_RESIZE, InputCode::NO_EVENT,
												m_windows[0].resizeCallback, 
												&m_windows[0]);


		VkExtent2D extent = m_windows[0].getSwapchain().getPixelExtent();

		m_square.updateVertices({0.0f, 0.0f}, {200, 200}, {1.0f, 0.0f, 0.0f, 1.0f}, m_windows[0].getOrthoMatrix());
		// 4 vertices in a quad
		uint32_t verticesSize = sizeof(Vertex) * 4;
		// 6 indices in a quad
		uint32_t indicesSize = sizeof(uint32_t) * 6;

		m_vertexStagingBuffer = m_allocator.allocateBuffer(verticesSize, BufferType::CPU);
		m_indexStagingBuffer = m_allocator.allocateBuffer(indicesSize, BufferType::CPU);

		m_allocator.submitData(m_vertexStagingBuffer, m_square.getVertices().data(), verticesSize, 0);
		m_allocator.submitData(m_indexStagingBuffer, m_square.getIndices(0).data(), indicesSize, 0);

		BufferTypeFlags flags = m_allocator.getBufferTypes();
		if(flags & BufferType::GPU)
		{
			m_shouldTransfer = true;
			m_vertexDeviceBuffer = m_allocator.allocateBuffer(verticesSize, BufferType::GPU);
			m_allocator.applyForTransfer(&m_vertexStagingBuffer, &m_vertexDeviceBuffer);

			m_indexDeviceBuffer = m_allocator.allocateBuffer(indicesSize, BufferType::GPU);
			m_allocator.applyForTransfer(&m_indexStagingBuffer, &m_indexDeviceBuffer);
		}
	}

	Renderer::~Renderer()
	{
		vkDeviceWaitIdle(m_device.getDevice());
		m_allocator.deallocateBuffer(&m_vertexStagingBuffer);
		m_allocator.deallocateBuffer(&m_vertexDeviceBuffer);

		m_allocator.deallocateBuffer(&m_indexStagingBuffer);
		m_allocator.deallocateBuffer(&m_indexDeviceBuffer);
	}

	void Renderer::draw()
	{
		// Check for transfers

		// Prepare images for all windows

		// Do drawing for each window

		// Submit command buffers

		// Switch frame value
 
		std::vector<VkSemaphore> renderWaitSemaphores;
		std::vector<VkPipelineStageFlags> waitStages;

		// Transfer vertices if required
		if(m_shouldTransfer)
		{
			m_shouldTransfer = false;

			renderWaitSemaphores.push_back(m_transferFinishedSemaphore.getSemaphore());
			waitStages.push_back(VK_PIPELINE_STAGE_TRANSFER_BIT);

			VkSubmitInfo transferSubmitInfo{};
			transferSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			transferSubmitInfo.pNext;
			transferSubmitInfo.waitSemaphoreCount;
			transferSubmitInfo.pWaitSemaphores;
			transferSubmitInfo.commandBufferCount = 1;
			VkCommandBuffer command[] = {m_allocator.getTransferBuffer()};
			transferSubmitInfo.pCommandBuffers = command;
			transferSubmitInfo.signalSemaphoreCount = 1;
			transferSubmitInfo.pSignalSemaphores = &renderWaitSemaphores.back();

			CHARRA_LOG_ERROR(vkQueueSubmit(m_device.getTransferQueue(), 1, &transferSubmitInfo, VK_NULL_HANDLE) != VK_SUCCESS, "Vulkan could not submit transfer command buffer to queue");
		}
		
		Window& window = m_windows[0];
		m_windows[0].getSwapchain().prepareNextImage(&window.getSemaphore());

		m_commandBuffers.resetCommandBuffer(m_commandBufferIndex);
		m_commandBuffers.beginRecording(m_commandBufferIndex);

		// I will record the main window now
		m_commandBuffers.beginRenderpass(window.getRenderpass().getRenderpass(),
												window.getSwapchain().getFramebuffer(), 
												window.getMaterial().getPipeline().getPipeline(),
												window.getSwapchain().getPixelExtent(),
												m_commandBufferIndex);


		VkRect2D scissor;
		scissor.extent = window.getSwapchain().getPixelExtent();
		scissor.offset = {0,0};

		VkViewport viewportCreateInfo{};
		viewportCreateInfo.x = 0;
		viewportCreateInfo.y = 0;
		viewportCreateInfo.width = static_cast<float>(scissor.extent.width);
		viewportCreateInfo.height = static_cast<float>(scissor.extent.height);
		viewportCreateInfo.minDepth = 0.0f;
		viewportCreateInfo.maxDepth = 1.0f;

		vkCmdSetViewport(m_commandBuffers.getCommandBuffer(m_commandBufferIndex), 0, 1, &viewportCreateInfo);
		vkCmdSetScissor(m_commandBuffers.getCommandBuffer(m_commandBufferIndex), 0, 1, &scissor);

		if(m_vertexDeviceBuffer.size != 0)
		{
			vkCmdBindVertexBuffers(m_commandBuffers.getCommandBuffer(m_commandBufferIndex), 0, 1, &m_vertexDeviceBuffer.buffer,
								   &m_vertexDeviceBuffer.offset);

			vkCmdBindIndexBuffer(m_commandBuffers.getCommandBuffer(m_commandBufferIndex),
								 m_indexDeviceBuffer.buffer, m_indexDeviceBuffer.offset, VK_INDEX_TYPE_UINT32);
		}
		else 
		{
			vkCmdBindVertexBuffers(m_commandBuffers.getCommandBuffer(m_commandBufferIndex), 0, 1, &m_vertexStagingBuffer.buffer,
								   &m_vertexStagingBuffer.offset);

			vkCmdBindIndexBuffer(m_commandBuffers.getCommandBuffer(m_commandBufferIndex), m_indexStagingBuffer.buffer, 
								 m_indexStagingBuffer.offset, VK_INDEX_TYPE_UINT32);
		}

		//vkCmdDraw(m_pImpl->commandBuffers.getCommandBuffer(m_pImpl->commandBufferIndex), static_cast<uint32_t>(m_pImpl->vertices.size()), 1, 0, 0);

		// TODO this shoudl not be hardcoded
	
		vkCmdDrawIndexed(m_commandBuffers.getCommandBuffer(m_commandBufferIndex), sizeof(uint32_t) * 6, 1, 0, 0, 0);
		 
		m_commandBuffers.endRenderpass(m_commandBufferIndex);

		// TODO gui stuff
		// I will record any gui windows now


		m_commandBuffers.endRecording(m_commandBufferIndex);

		// Submit command buffers
		renderWaitSemaphores.push_back(window.getSemaphore().getSemaphore());
		waitStages.push_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

		VkSubmitInfo submitInfo{};
		submitInfo.waitSemaphoreCount = renderWaitSemaphores.size();
		submitInfo.pWaitSemaphores = renderWaitSemaphores.data();
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext;
		submitInfo.pWaitDstStageMask = waitStages.data();
		submitInfo.commandBufferCount = 1;
		VkCommandBuffer commandBuffers[] = {m_commandBuffers.getCommandBuffer(m_commandBufferIndex)};
		submitInfo.pCommandBuffers = commandBuffers;
		submitInfo.signalSemaphoreCount = 1;
		VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphore.getSemaphore()};
		submitInfo.pSignalSemaphores = signalSemaphores;

		// 1/10th of a second
		VkFence fences[] = {m_renderFinishedFence.getFence()};
		vkWaitForFences(m_device.getDevice(), 1, fences, true, 100000000);
		m_renderFinishedFence.reset();

		// This is the minimal info that can be done after waiting on fences

		CHARRA_LOG_ERROR(vkQueueSubmit(m_device.getGraphicsQueue(), 1, &submitInfo, m_renderFinishedFence.getFence()) != VK_SUCCESS, "Vulkan could not submit command buffer to queue");

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext;
		presentInfo.waitSemaphoreCount = 1;
		VkSemaphore presentSemaphores[] = {m_renderFinishedSemaphore.getSemaphore()};
		presentInfo.pWaitSemaphores = presentSemaphores;
		presentInfo.swapchainCount = 1; // TODO mulitple windows will need this
		VkSwapchainKHR swapchains[1] = {window.getSwapchain().getSwapchain()};
		presentInfo.pSwapchains = swapchains;
		const uint32_t imageIndices[1] = {window.getSwapchain().getImageIndex()};
		presentInfo.pImageIndices = imageIndices; // TODO this will need to be more complex for multiple windows
		presentInfo.pResults;

		// TODO: Check this result for resizing
		vkQueuePresentKHR(m_device.getGraphicsQueue(), &presentInfo);

		// This flips the index between 0 and 1 without branching
		m_commandBufferIndex = 1 - m_commandBufferIndex;
	}
}