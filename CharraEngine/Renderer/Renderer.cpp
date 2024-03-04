#include "Renderer.hpp"

#include "Platform/Platform.hpp"
#include "Core/Logging.hpp"
#include "Math/RendererTypes.hpp"
#include "Math/MathFunctions.hpp"

#include "vulkan/vulkan.h"

namespace Charra
{
		Renderer::Renderer(Events* eventHandler, WindowManager* windowManagerRef)
		: m_eventHandlerRef(eventHandler),
		  m_windowManagerRef(windowManagerRef),
		  m_instance(),
		  m_device(m_instance),
		  m_commandBuffers(m_device, 2, CommandBufferType::GRAPHICS),
		  m_bufferManager(m_device),
		  m_transferFinishedSemaphore(m_device),
		  m_renderFinishedFence(m_device)
	{
	}

	Renderer::~Renderer()
	{
		vkDeviceWaitIdle(m_device.getDevice());
	}

	void Renderer::draw()
	{
		// 1/10th of a second
		VkFence fences[] = {m_renderFinishedFence.getFence()};
		vkWaitForFences(m_device.getDevice(), 1, fences, true, 100000000);
		m_renderFinishedFence.reset();

		std::vector<VkSemaphore> renderWaitSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkPipelineStageFlags> waitStages;
		
		
		transferBuffers(&renderWaitSemaphores, &waitStages);

		recordCommandBuffers(&renderWaitSemaphores, &renderFinishedSemaphores, &waitStages);

		submitCommandBuffers();

		presentImages();

		// This flips the index between 0 and 1 without branching
		m_commandBufferIndex = 1 - m_commandBufferIndex;
	}

	void Renderer::transferBuffers(std::vector<VkSemaphore>* semaphores, std::vector<VkPipelineStageFlags>* waitStages)
	{
		// Pass BufferManager to each canvas and let them manage transfers

		m_windowManagerRef->transferBuffers(&m_bufferManager);

		VkCommandBuffer buffer = m_bufferManager.getTransferBuffer();

		if(buffer != VK_NULL_HANDLE)
		{
			semaphores->push_back(m_transferFinishedSemaphore.getSemaphore());
			waitStages->push_back(VK_PIPELINE_STAGE_TRANSFER_BIT);

			VkSubmitInfo transferSubmitInfo{};
			transferSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			transferSubmitInfo.pNext;
			transferSubmitInfo.waitSemaphoreCount;
			transferSubmitInfo.pWaitSemaphores;
			transferSubmitInfo.commandBufferCount = 1;
			transferSubmitInfo.pCommandBuffers = &buffer;
			transferSubmitInfo.signalSemaphoreCount = 1;
			transferSubmitInfo.pSignalSemaphores = &semaphores->back();

			CHARRA_LOG_ERROR(vkQueueSubmit(m_device.getTransferQueue(), 1, &transferSubmitInfo, VK_NULL_HANDLE) != VK_SUCCESS, "Vulkan could not submit transfer command buffer to queue");
		}
	
	}

	void Renderer::recordCommandBuffers(std::vector<VkSemaphore>* renderSemaphore, std::vector<VkSemaphore>* presentSemaphore, std::vector<VkPipelineStageFlags>* waitStages)
	{
		// Record command buffers for each window
		m_commandBuffers.resetCommandBuffer(m_commandBufferIndex);
		m_commandBuffers.beginRecording(m_commandBufferIndex);

		// Record command buffers for each window

		m_windowManagerRef->recordCommandBuffers(renderSemaphore, presentSemaphore, waitStages, m_commandBufferIndex);

		// End recording
		m_commandBuffers.endRecording(m_commandBufferIndex);
	}

	void Renderer::submitCommandBuffers()
	{
		// Submit command buffers
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext;
		submitInfo.waitSemaphoreCount;
		submitInfo.pWaitSemaphores;
		submitInfo.pWaitDstStageMask;
		submitInfo.commandBufferCount = 1;
		VkCommandBuffer commandBuffers[] = {m_commandBuffers.getCommandBuffer(m_commandBufferIndex)};
		submitInfo.pCommandBuffers = commandBuffers;
		submitInfo.signalSemaphoreCount;
		submitInfo.pSignalSemaphores;

		CHARRA_LOG_ERROR(vkQueueSubmit(m_device.getGraphicsQueue(), 1, &submitInfo, m_renderFinishedFence.getFence()) != VK_SUCCESS, "Vulkan could not submit command buffer to queue");
	}

	void Renderer::presentImages()
	{
		// Present images
		m_windowManagerRef->presentImages();
	}
}