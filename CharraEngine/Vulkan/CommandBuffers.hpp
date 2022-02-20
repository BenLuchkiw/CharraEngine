#pragma once

#include "vulkan/vulkan.h"

#include "Device.hpp"

namespace Charra
{
	class CommandBuffers
	{
	public:
		CommandBuffers(Device* deviceRef, uint32_t bufferCount);
		~CommandBuffers();

		inline uint32_t getBufferCount() { return m_bufferCount; }
		inline VkCommandBuffer* getCommandBuffer(uint32_t index) { return &m_commandBuffers[index]; }

		void beginRecording(uint32_t bufferIndex);
		void endRecording(uint32_t bufferIndex);
		void resetCommandBuffer(uint32_t bufferIndex);

		void beginRenderpass(VkRenderPass* renderpass, VkFramebuffer* framebuffer, VkPipeline* pipeline, VkExtent2D extent, uint32_t bufferIndex);
		void endRenderpass(uint32_t bufferIndex);

		void draw(uint32_t bufferIndex); // Todo make input buffers

	private: // Methods
		void createCommandPool();
		void createCommandBuffers();
	private: // Members
		Device* m_deviceRef;
		VkCommandPool m_commandPool;
		std::vector<VkCommandBuffer> m_commandBuffers;

		uint32_t m_bufferCount;
	};
}