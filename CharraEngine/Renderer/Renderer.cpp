#include "Renderer.hpp"

#include "Platform/Platform.hpp"
#include "Core/Logging.hpp"
#include "Math/RendererTypes.hpp"
#include "Math/MathFunctions.hpp"

#include "vulkan/vulkan.h"

namespace Charra
{
		Renderer::Renderer(Events* eventHandler)
		: m_eventHandlerRef(eventHandler),
		  m_instance(),
		  m_device(m_instance),
		  m_commandBuffers(m_device, 2, CommandBufferType::GRAPHICS),
		  m_allocator(m_device),
		  m_transferFinishedSemaphore(m_device),
		  m_renderFinishedSemaphore(m_device),
		  m_renderFinishedFence(m_device)
	{
		
	}

	Renderer::~Renderer()
	{
		vkDeviceWaitIdle(m_device.getDevice());
		m_allocator.deallocateBuffer(&m_vertexStagingBuffer);
		m_allocator.deallocateBuffer(&m_vertexDeviceBuffer);

		m_allocator.deallocateBuffer(&m_indexStagingBuffer);
		m_allocator.deallocateBuffer(&m_indexDeviceBuffer);
	}

	void Renderer::draw(std::vector<Window>& windows)
	{
		// 1/10th of a second
		VkFence fences[] = {m_renderFinishedFence.getFence()};
		vkWaitForFences(m_device.getDevice(), 1, fences, true, 100000000);
		m_renderFinishedFence.reset();


		// Update quad list and set up transfers

		if(m_vertexStagingBuffer.buffer != VK_NULL_HANDLE)
		{
			m_allocator.deallocateBuffer(&m_vertexStagingBuffer);
		}
		if(m_vertexDeviceBuffer.buffer != VK_NULL_HANDLE)
		{
			m_allocator.deallocateBuffer(&m_vertexDeviceBuffer);
		}
		if(m_indexStagingBuffer.buffer != VK_NULL_HANDLE)
		{
			m_allocator.deallocateBuffer(&m_indexStagingBuffer);
		}
		if(m_indexDeviceBuffer.buffer != VK_NULL_HANDLE)
		{
			m_allocator.deallocateBuffer(&m_indexDeviceBuffer);
		}

		if(m_quads.size() == 0 || windows.size() == 0)
		{
			return;
		}

		for(int i = 0; i < m_quads.size(); i++)
		{
			m_quads[i].updateVertices(windows[m_quads[i].getWindowID()].getOrthoMatrix());
		}

		static uint32_t verticesSize = sizeof(Vertex) * 4;
		static uint32_t indicesSize = sizeof(uint32_t) * 6;

		m_vertexStagingBuffer = m_allocator.allocateBuffer(verticesSize * m_quads.size(), BufferType::CPU);
		m_indexStagingBuffer = m_allocator.allocateBuffer(indicesSize * m_quads.size(), BufferType::CPU);

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		for(int i = 0; i < m_quads.size(); i++)
		{
			vertices.insert(vertices.end(), m_quads[i].getVertices().begin(), m_quads[i].getVertices().end());

			std::array<uint32_t, 6> indicesTemp = m_quads[i].getIndices(i * 4);
			indices.insert(indices.end(), indicesTemp.begin(), indicesTemp.end());
		}

		m_allocator.submitData(m_vertexStagingBuffer, vertices.data(), verticesSize * m_quads.size(), 0);
		m_allocator.submitData(m_indexStagingBuffer, indices.data(), indicesSize * m_quads.size(), 0);

		BufferTypeFlags flags = m_allocator.getBufferTypes();
		if(flags & BufferType::GPU)
		{
			m_shouldTransfer = true;
			m_vertexDeviceBuffer = m_allocator.allocateBuffer(verticesSize * m_quads.size(), BufferType::GPU);
			m_allocator.applyForTransfer(&m_vertexStagingBuffer, &m_vertexDeviceBuffer);

			m_indexDeviceBuffer = m_allocator.allocateBuffer(indicesSize * m_quads.size(), BufferType::GPU);
			m_allocator.applyForTransfer(&m_indexStagingBuffer, &m_indexDeviceBuffer);
		}
		
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
		
		Window& window = windows[0];
		windows[0].getSwapchain().prepareNextImage(&window.getSemaphore());

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

		// TODO this should not be hardcoded
	
		vkCmdDrawIndexed(m_commandBuffers.getCommandBuffer(m_commandBufferIndex), sizeof(uint32_t) * 6 * m_quads.size(), 1, 0, 0, 0);
		 
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

		m_quads.clear();
	}

	void Renderer::drawQuad(fVec3 pos, fVec2 size, fVec4 colour, uint32_t windowID)
	{
		m_quads.emplace_back(pos, size, colour);
		m_quads.back().assignWindow(windowID);
	}
}