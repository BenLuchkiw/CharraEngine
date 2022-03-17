#include "Renderer.hpp"

#include "Platform/Platform.hpp"
#include "Core/Logging.hpp"
#include "Math/RendererTypes.hpp"
#include "Math/MathFunctions.hpp"

// For RendererImplData
#include "Vulkan/Allocator/Allocator.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/Instance.hpp"
#include "Vulkan/Syncronization.hpp"

// For WindowData
#include "Vulkan/Swapchain.hpp"
#include "Vulkan/Renderpass.hpp"

// For MaterialData
#include "Vulkan/Pipeline.hpp"

#include "vulkan/vulkan.h"

#include <array>

namespace Charra
{
	struct MaterialData
	{
		GraphicsPipeline pipeline;

		MaterialData(Device* deviceRef, Renderpass* renderpassRef,
		 VkVertexInputBindingDescription vertexBindingAttributes,
		 std::vector<VkVertexInputAttributeDescription> attributeDescription)
		:pipeline(deviceRef, renderpassRef, "SimpleVertex.spv", "SimpleFragment.spv",
		 vertexBindingAttributes, attributeDescription)
		{}
	};

	struct WindowData
	{
		iVec2 windowSize;
		std::string windowName;
		Swapchain swapchain;
		Renderpass renderpass;
		Semaphore imageWaitSemaphore;

		// TODO this should not be here
		VkVertexInputBindingDescription vertexBindingAttributes{0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX};
		std::vector<VkVertexInputAttributeDescription> attributeDescription = {
			{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)},
			{1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, colour)}
		};

		MaterialData material;

		WindowData(iVec2 size, const std::string& name, Device* deviceRef, Instance* instanceRef)
		:windowSize(size), windowName(name), swapchain(deviceRef, instanceRef), 
		 renderpass(deviceRef, swapchain.getSurfaceFormat()),
		 imageWaitSemaphore(deviceRef),
		 material(deviceRef, &renderpass, vertexBindingAttributes, attributeDescription)
		{
			swapchain.passRenderpass(&renderpass);
		}
	
		WindowData(const WindowData&) = default;
  		WindowData& operator=(const WindowData&) = default;
  		WindowData(WindowData&&) = default;
  		WindowData& operator=(WindowData&&) = default;
	};

	struct RendererImplData
	{
		Events* eventHandlerRef;

		Instance instance;
		Device device;

		Semaphore transferFinishedSemaphore;
		Semaphore renderFinishSempahore;
		Fence renderFinishedFence;

		CommandBuffers commandBuffers;
		uint32_t commandBufferIndex = 0;

		Allocator allocator;

		std::vector<WindowData> windows;

		std::vector<Vertex> vertices;
		Buffer vertexStagingBuffer{};
		Buffer vertexDeviceBuffer{};
		bool shouldTransfer = false;

		RendererImplData(iVec2 windowSize, const std::string& windowName, Events* eventHandlerRef)
		:instance(),
		 device(&instance),
		 transferFinishedSemaphore(&device),
		 renderFinishSempahore(&device),
		 renderFinishedFence(&device),
		 commandBuffers(&device, 2, CommandBufferType::GRAPHICS),
		 eventHandlerRef(eventHandlerRef),
		 allocator(&device)
		{
			Platform::createWindow(windowName, windowSize);

			windows.emplace_back(windowSize, windowName, &device, &instance);

			eventHandlerRef->registerEventCallback(0, EventType::WINDOW_RESIZE, InputCode::NO_EVENT,
			 										windows[0].swapchain.resizeCallback, &windows[0].swapchain);
		}

		~RendererImplData()
		{
			windows.clear();
		}
	};

	Renderer::Renderer(const std::string& mainWindowName, Events* eventHandler)
	{
		m_pImpl  = std::make_unique<RendererImplData>(iVec2(400, 400), mainWindowName, eventHandler);

		VkExtent2D extent = m_pImpl->windows[0].swapchain.getPixelExtent();
		Mat4X4 mat = getOrthographicMatrix(100, 0, 0, extent.width, 0, extent.height);

		Vertex point;
		point.position.x = 100.0f;
		point.position.y = 0.0f;
		point.position.z = 50.0f;
		point.colour.r = 1.0f;
		point.colour.g = 0.0f;
		point.colour.b = 0.0f;
		point.colour.a = 1.0f;
		m_pImpl->vertices.push_back(point);

		point.position.x = 200.0f;
		point.position.y = 200.0f;
		point.colour.r = 0.0f;
		point.colour.g = 1.0f;
		m_pImpl->vertices.push_back(point);

		point.position.x = 0.0f;
		point.colour.g = 0.0f;
		point.colour.b = 1.0f;
		m_pImpl->vertices.push_back(point);

		m_pImpl->vertices[0].position = mulMatrix(mat, m_pImpl->vertices[0].position);
		m_pImpl->vertices[1].position = mulMatrix(mat, m_pImpl->vertices[1].position);
		m_pImpl->vertices[2].position = mulMatrix(mat, m_pImpl->vertices[2].position);

		uint32_t size = sizeof(Vertex) * m_pImpl->vertices.size();

		m_pImpl->vertexStagingBuffer = m_pImpl->allocator.allocateBuffer(size, BufferType::CPU);

		m_pImpl->allocator.submitData(&m_pImpl->vertexStagingBuffer, m_pImpl->vertices.data(), size, 0);

		BufferTypeFlags flags = m_pImpl->allocator.getBufferTypes();
		if(flags & BufferType::GPU)
		{
			m_pImpl->shouldTransfer = true;
			m_pImpl->vertexDeviceBuffer = m_pImpl->allocator.allocateBuffer(size, BufferType::GPU);
			m_pImpl->allocator.applyForTransfer(&m_pImpl->vertexStagingBuffer, &m_pImpl->vertexDeviceBuffer);
		}
	}

	Renderer::~Renderer()
	{
		vkDeviceWaitIdle(m_pImpl->device.getDevice());
		m_pImpl->allocator.deallocateBuffer(&m_pImpl->vertexDeviceBuffer);
		m_pImpl->allocator.deallocateBuffer(&m_pImpl->vertexStagingBuffer);

		m_pImpl.reset();
	}

	void Renderer::draw()
	{
		std::vector<VkSemaphore> renderWaitSemaphores;
		std::vector<VkPipelineStageFlags> waitStages;

		// Transfer vertices if required
		if(m_pImpl->shouldTransfer)
		{
			m_pImpl->shouldTransfer = false;

			renderWaitSemaphores.push_back(m_pImpl->transferFinishedSemaphore.getSemaphore());
			waitStages.push_back(VK_PIPELINE_STAGE_TRANSFER_BIT);

			VkSubmitInfo transferSubmitInfo{};
			transferSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			transferSubmitInfo.pNext;
			transferSubmitInfo.waitSemaphoreCount;
			transferSubmitInfo.pWaitSemaphores;
			transferSubmitInfo.commandBufferCount = 1;
			VkCommandBuffer command[] = {m_pImpl->allocator.getTransferBuffer()};
			transferSubmitInfo.pCommandBuffers = command;
			transferSubmitInfo.signalSemaphoreCount = 1;
			transferSubmitInfo.pSignalSemaphores = &renderWaitSemaphores.back();

			CHARRA_LOG_ERROR(vkQueueSubmit(m_pImpl->device.getTransferQueue(), 1, &transferSubmitInfo, VK_NULL_HANDLE) != VK_SUCCESS, "Vulkan could not submit transfer command buffer to queue");
		}
		
		// For next frame
		WindowData& window = m_pImpl->windows[0];
		m_pImpl->windows[0].swapchain.prepareNextImage(&window.imageWaitSemaphore);

		m_pImpl->commandBuffers.resetCommandBuffer(m_pImpl->commandBufferIndex);
		m_pImpl->commandBuffers.beginRecording(m_pImpl->commandBufferIndex);

		// I will record the main window now
		m_pImpl->commandBuffers.beginRenderpass(window.renderpass.getRenderPass(),
												window.swapchain.getFramebuffer(), 
												window.material.pipeline.getPipeline(),
												window.swapchain.getPixelExtent(),
												m_pImpl->commandBufferIndex);


		VkRect2D scissor;
		scissor.extent = window.swapchain.getPixelExtent();
		scissor.offset = {0,0};

		VkViewport viewportCreateInfo{};
		viewportCreateInfo.x = 0;
		viewportCreateInfo.y = 0;
		viewportCreateInfo.width = scissor.extent.width;
		viewportCreateInfo.height = scissor.extent.height;
		viewportCreateInfo.minDepth = 0.0f;
		viewportCreateInfo.maxDepth = 1.0f;

		vkCmdSetViewport(m_pImpl->commandBuffers.getCommandBuffer(m_pImpl->commandBufferIndex), 0, 1, &viewportCreateInfo);
		vkCmdSetScissor(m_pImpl->commandBuffers.getCommandBuffer(m_pImpl->commandBufferIndex), 0, 1, &scissor);

		if(m_pImpl->vertexDeviceBuffer.size != 0)
		{
			vkCmdBindVertexBuffers(m_pImpl->commandBuffers.getCommandBuffer(m_pImpl->commandBufferIndex), 0, 1, &m_pImpl->vertexDeviceBuffer.buffer, &m_pImpl->vertexDeviceBuffer.offset);
		}
		else 
		{
			vkCmdBindVertexBuffers(m_pImpl->commandBuffers.getCommandBuffer(m_pImpl->commandBufferIndex), 0, 1, &m_pImpl->vertexStagingBuffer.buffer, &m_pImpl->vertexStagingBuffer.offset);
		}

		uint32_t size = m_pImpl->vertices.size() * sizeof(Vertex);
		vkCmdDraw(m_pImpl->commandBuffers.getCommandBuffer(m_pImpl->commandBufferIndex), static_cast<uint32_t>(m_pImpl->vertices.size()), 1, 0, 0);
		 
		m_pImpl->commandBuffers.endRenderpass(m_pImpl->commandBufferIndex);

		// TODO gui stuff
		// I will record any gui windows now


		m_pImpl->commandBuffers.endRecording(m_pImpl->commandBufferIndex);

		// Submit command buffers
		renderWaitSemaphores.push_back(window.imageWaitSemaphore.getSemaphore());
		waitStages.push_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

		VkSubmitInfo submitInfo{};
		submitInfo.waitSemaphoreCount = renderWaitSemaphores.size();
		submitInfo.pWaitSemaphores = renderWaitSemaphores.data();
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext;
		submitInfo.pWaitDstStageMask = waitStages.data();
		submitInfo.commandBufferCount = 1;
		VkCommandBuffer commandBuffers[] = {m_pImpl->commandBuffers.getCommandBuffer(m_pImpl->commandBufferIndex)};
		submitInfo.pCommandBuffers = commandBuffers;
		submitInfo.signalSemaphoreCount = 1;
		VkSemaphore signalSemaphores[] = {m_pImpl->renderFinishSempahore.getSemaphore()};
		submitInfo.pSignalSemaphores = signalSemaphores;

		// 1/10th of a second
		VkFence fences[] = {m_pImpl->renderFinishedFence.getFence()};
		vkWaitForFences(m_pImpl->device.getDevice(), 1, fences, true, 100000000);
		m_pImpl->renderFinishedFence.reset();

		// This is the minimal info that can be done after waiting on fences

		CHARRA_LOG_ERROR(vkQueueSubmit(m_pImpl->device.getGraphicsQueue(), 1, &submitInfo, m_pImpl->renderFinishedFence.getFence()) != VK_SUCCESS, "Vulkan could not submit command buffer to queue");

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext;
		presentInfo.waitSemaphoreCount = 1;
		VkSemaphore presentSemaphores[] = {m_pImpl->renderFinishSempahore.getSemaphore()};
		presentInfo.pWaitSemaphores = presentSemaphores;
		presentInfo.swapchainCount = 1; // TODO mulitple windows will need this
		VkSwapchainKHR swapchains[1] = {window.swapchain.getSwapchain()};
		presentInfo.pSwapchains = swapchains;
		const uint32_t imageIndices[1] = {window.swapchain.getImageIndex()};
		presentInfo.pImageIndices = imageIndices; // TODO this will need to be more complex for multiple windows
		presentInfo.pResults;

		// Check this result for resizing
		VkResult result = vkQueuePresentKHR(m_pImpl->device.getGraphicsQueue(), &presentInfo);

		// This flips the index between 0 and 1 without branching
		m_pImpl->commandBufferIndex = 1 - m_pImpl->commandBufferIndex;
	}
}