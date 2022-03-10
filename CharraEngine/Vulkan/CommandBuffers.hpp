#pragma once

#include "vulkan/vulkan.h"

#include "Device.hpp"

namespace Charra
{
	enum class CommandBufferType
	{
		GRAPHICS = 1,
		TRANSFER
	};

	class CommandBuffers
	{
	public:
		CommandBuffers(Device* deviceRef, uint32_t bufferCount, CommandBufferType type);
		~CommandBuffers();

		inline uint32_t getBufferCount() { return m_bufferCount; }
		inline VkCommandBuffer getCommandBuffer(uint32_t index) { return m_commandBuffers[index]; }

		void beginRecording(uint32_t bufferIndex);
		void endRecording(uint32_t bufferIndex);
		void resetCommandBuffer(uint32_t bufferIndex);

		void beginRenderpass(VkRenderPass renderpass, VkFramebuffer framebuffer, VkPipeline pipeline, VkExtent2D extent, uint32_t bufferIndex);
		void endRenderpass(uint32_t bufferIndex);

	private: // Methods
		void createCommandPool(CommandBufferType type);
		void createCommandBuffers();
	private: // Members
		Device* m_deviceRef;
		VkCommandPool m_commandPool;
		std::vector<VkCommandBuffer> m_commandBuffers;

		uint32_t m_bufferCount;
	};
}