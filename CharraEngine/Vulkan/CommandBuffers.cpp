#include "CommandBuffers.hpp"

#include "Pipeline.hpp"
#include "Renderpass.hpp"

#include "Core/Logging.hpp"

namespace Charra
{
	CommandBuffers::CommandBuffers(Device* deviceRef, uint32_t bufferCount)
		: m_deviceRef(deviceRef),
		m_commandPool(VK_NULL_HANDLE),
		m_commandBuffers(bufferCount),
		m_bufferCount(bufferCount)
	{
		createCommandPool();
		createCommandBuffers();
	}

	CommandBuffers::~CommandBuffers()
	{
		vkDestroyCommandPool(m_deviceRef->getDevice(), m_commandPool, NULL);
	}

	void CommandBuffers::beginRecording(uint32_t bufferIndex)
	{
		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		commandBufferBeginInfo.pInheritanceInfo;

		CHARRA_LOG_ERROR(vkBeginCommandBuffer(m_commandBuffers[bufferIndex], &commandBufferBeginInfo) != VK_SUCCESS, "Vulkan could not begin command buffer recording");
	}

	void CommandBuffers::endRecording(uint32_t bufferIndex)
	{
		vkEndCommandBuffer(m_commandBuffers[bufferIndex]);
	}

	void CommandBuffers::resetCommandBuffer(uint32_t bufferIndex)
	{
		vkResetCommandBuffer(m_commandBuffers[bufferIndex], NULL);
	}

	void CommandBuffers::beginRenderpass(VkRenderPass* renderpass, VkFramebuffer* framebuffer, VkPipeline* pipeline, VkExtent2D extent, uint32_t bufferIndex)
	{
		VkClearValue clearValue[1] = { 1.0f, 1.0f, 0.0f, 1.0f };
		VkRenderPassBeginInfo renderpassBeginInfo{};
		renderpassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderpassBeginInfo.pNext;
		renderpassBeginInfo.renderPass = *renderpass;
		renderpassBeginInfo.framebuffer = *framebuffer;
		renderpassBeginInfo.renderArea.extent = extent;
		renderpassBeginInfo.renderArea.offset = { 0,0 };
		renderpassBeginInfo.clearValueCount = 1;
		renderpassBeginInfo.pClearValues = clearValue;
		vkCmdBeginRenderPass(m_commandBuffers[bufferIndex], &renderpassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkRect2D scissor{};
		scissor.extent = extent;
		scissor.offset = { 0,0 };
		vkCmdSetScissor(m_commandBuffers[bufferIndex], 0, 1, &scissor);

		VkViewport viewport{};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = extent.width;
		viewport.height = extent.height;
		viewport.minDepth = 1.0f;
		viewport.maxDepth = 0.0f;
		vkCmdSetViewport(m_commandBuffers[bufferIndex], 0, 1, &viewport);
		
		vkCmdBindPipeline(m_commandBuffers[bufferIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline);
	}

	void CommandBuffers::endRenderpass(uint32_t bufferIndex)
	{
		vkCmdEndRenderPass(m_commandBuffers[bufferIndex]);
	}

	void CommandBuffers::draw(uint32_t bufferIndex)
	{
		vkCmdDraw(m_commandBuffers[bufferIndex], 3, 1, 0, 0);
	}

	void CommandBuffers::createCommandPool()
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo{};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.pNext;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = m_deviceRef->getGraphicsQueueIndex();

		CHARRA_LOG_ERROR(vkCreateCommandPool(m_deviceRef->getDevice(), &commandPoolCreateInfo, NULL, &m_commandPool) != VK_SUCCESS, "Vulkan was unable to create command pool");
	}

	void CommandBuffers::createCommandBuffers()
	{
		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.pNext;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 3;
		commandBufferAllocateInfo.commandPool = m_commandPool;

		CHARRA_LOG_ERROR(vkAllocateCommandBuffers(m_deviceRef->getDevice(), &commandBufferAllocateInfo, m_commandBuffers.data()) != VK_SUCCESS, "Vulkan could not allocate command buffers");
	}
}