#pragma once

#include "vulkan/vulkan.h"

#include "Buffer.hpp"
#include "DeviceAllocator.hpp"
#include "HostAllocator.hpp"

#include "Vulkan/CommandBuffers.hpp"
#include "Vulkan/Device.hpp"

#include <memory>

namespace Charra
{
	class Allocator
	{
	public:
		Allocator(Device* deviceRef);
		~Allocator();

		void submitData(Buffer* buffer, void* data, size_t bytes, size_t offset);
		void getData();

		Buffer allocateBuffer(uint64_t size, BufferTypeFlags type);
		void deallocateBuffer(Buffer* buffer);
		void applyForTransfer(Buffer* src, Buffer* dst);

		VkCommandBuffer getTransferBuffer();

		inline BufferTypeFlags getBufferTypes() { return m_availableTypes; }
	private: // Methods
		uint32_t findType(VkMemoryPropertyFlags flags);

	private: // Members
		struct Transfer
		{
			VkBuffer src;
			VkBuffer dst;

			std::vector<VkBufferCopy> m_copies;
		};

		Device* m_deviceRef;

		CommandBuffers m_commandBuffers;

		std::unique_ptr<HostAllocator> m_hostAllocator;
		std::unique_ptr<DeviceAllocator> m_deviceAllocator;

		std::vector<Transfer> m_transfers;

		uint64_t m_internalId = 0;

		bool m_hostCoherant;

		BufferTypeFlags m_availableTypes = BufferType::CPU;
	};
}