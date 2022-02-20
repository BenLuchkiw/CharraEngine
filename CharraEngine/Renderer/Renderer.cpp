#include "Renderer.hpp"

#include "Vulkan/CommandBuffers.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/Instance.hpp"
#include "Vulkan/Shader.hpp"
#include "Vulkan/ValidationLayers.hpp"
#include "Vulkan/Pipeline.hpp"
#include "Vulkan/Swapchain.hpp"
#include "Vulkan/Syncronization.hpp"
#include "Vulkan/Renderpass.hpp"
#include "Vulkan/Images.hpp"

#include "Platform/Platform.hpp"
#include "Core/Logging.hpp"

#include "vulkan/vulkan.h"

#include <array>

namespace Charra
{
	struct RendererImplData // To avoid pimpl, maybe faster
	{
		Instance instance;
		Device device;

		Vec2 windowSize;
		std::string windowName;
		Swapchain mainWindowSwapchain;
		Renderpass renderpass;
		Images mainWindowImages;
		Semaphore mainWindowImageWaitSemaphore;

		Semaphore renderFinishSempahore;
		Fence renderFinishedFence;

		CommandBuffers commandBuffers;

		GraphicsPipeline pipeline;

		uint32_t commandBufferIndex = 0;

		Events* eventHandlerRef;

		RendererImplData(Vec2 windowSize, const std::string& windowName, Events* eventHandlerRef)
		: instance(),
		device(&instance),
		windowSize(windowSize), 
		windowName(windowName),
		mainWindowSwapchain(&device, &instance),
		renderpass(&device, mainWindowSwapchain.getSurfaceFormat()),
		mainWindowImages(&device, &mainWindowSwapchain, &renderpass),
		mainWindowImageWaitSemaphore(&device),
		renderFinishSempahore(&device),
		renderFinishedFence(&device),
		commandBuffers(&device, 2),
		pipeline(&device, &renderpass, "SimpleVertex.spv", "SimpleFragment.spv"),
		eventHandlerRef(eventHandlerRef)
		{

		}
	};

	Renderer::Renderer(const std::string& mainWindowName, Events* eventHandler)
	{
		Vec2 windowSize = {400,400};
		Platform::createWindow(mainWindowName, windowSize);
		m_pImpl  = std::make_unique<RendererImplData>(windowSize, mainWindowName, eventHandler);

		// TODO mulitple windows will need this to be better

		m_pImpl->eventHandlerRef->registerEventCallback(0, EventType::WINDOW_RESIZE, InputCode::NO_EVENT, m_pImpl->mainWindowSwapchain.resizeCallback, &m_pImpl->mainWindowSwapchain);
	}

	Renderer::~Renderer()
	{

	}

	void Renderer::draw()
	{
		if(m_pImpl->mainWindowSwapchain.shouldResize())
		{
			m_pImpl->mainWindowImages.recreate();
		}
		m_pImpl->commandBuffers.resetCommandBuffer(m_pImpl->commandBufferIndex);
		m_pImpl->commandBuffers.beginRecording(m_pImpl->commandBufferIndex);

		// I will record the main window now
		m_pImpl->mainWindowSwapchain.prepareNextImage(&m_pImpl->mainWindowImageWaitSemaphore);
		m_pImpl->commandBuffers.beginRenderpass(m_pImpl->renderpass.getRenderPass(),
												&m_pImpl->mainWindowImages.getFramebuffers()->at(m_pImpl->mainWindowSwapchain.getImageIndex()), 
												m_pImpl->pipeline.getPipeline(),
												m_pImpl->mainWindowSwapchain.getPixelExtent(),
												m_pImpl->commandBufferIndex);


		VkRect2D scissor;
		scissor.extent = m_pImpl->mainWindowSwapchain.getPixelExtent();
		scissor.offset = {0,0};

		VkViewport viewportCreateInfo{};
		viewportCreateInfo.x = 0;
		viewportCreateInfo.y = 0;
		viewportCreateInfo.width = scissor.extent.width;
		viewportCreateInfo.height = scissor.extent.height;
		viewportCreateInfo.minDepth = 0.0f;
		viewportCreateInfo.maxDepth = 1.0f;

		vkCmdSetViewport(*m_pImpl->commandBuffers.getCommandBuffer(m_pImpl->commandBufferIndex), 0, 1, &viewportCreateInfo);
		vkCmdSetScissor(*m_pImpl->commandBuffers.getCommandBuffer(m_pImpl->commandBufferIndex), 0, 1, &scissor);

		m_pImpl->commandBuffers.draw(m_pImpl->commandBufferIndex);
		
		m_pImpl->commandBuffers.endRenderpass(m_pImpl->commandBufferIndex);

		// TODO gui stuff
		// I will record any gui windows now



		m_pImpl->commandBuffers.endRecording(m_pImpl->commandBufferIndex);

		// Submit command buffers
		// TODO multiple windows will need a long array
		std::array<VkPipelineStageFlags, 1> waitStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext;
		submitInfo.waitSemaphoreCount = 1; // TODO multiple windows will need multiple semaphores
		submitInfo.pWaitSemaphores = m_pImpl->mainWindowImageWaitSemaphore.getSemaphore();
		submitInfo.pWaitDstStageMask = waitStages.data();
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = m_pImpl->commandBuffers.getCommandBuffer(m_pImpl->commandBufferIndex);
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = m_pImpl->renderFinishSempahore.getSemaphore();

		// 1/10th of a second
		vkWaitForFences(m_pImpl->device.getDevice(), 1, m_pImpl->renderFinishedFence.getFence(), true, 100000000);
		m_pImpl->renderFinishedFence.reset();
		
		CHARRA_LOG_ERROR(vkQueueSubmit(m_pImpl->device.getGraphicsQueue(), 1, &submitInfo, *m_pImpl->renderFinishedFence.getFence()) != VK_SUCCESS, "Vulkan could not submit command buffer to queue");

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = m_pImpl->renderFinishSempahore.getSemaphore();
		presentInfo.swapchainCount = 1; // TODO mulitple windows will need this
		presentInfo.pSwapchains = m_pImpl->mainWindowSwapchain.getSwapchain();
		const uint32_t imageIndices[1] = {m_pImpl->mainWindowSwapchain.getImageIndex()};
		presentInfo.pImageIndices = imageIndices; // TODO this will need to be more complex for multiple windows
		presentInfo.pResults;


		// Check this result for resizing
		VkResult result = vkQueuePresentKHR(m_pImpl->device.getGraphicsQueue(), &presentInfo);

		// This flips the index between 0 and 1 without branching
		m_pImpl->commandBufferIndex = 1 - m_pImpl->commandBufferIndex;
	}
}