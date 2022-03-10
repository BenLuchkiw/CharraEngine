#pragma once

#include "Math/MathTypes.hpp"

#include "vulkan/vulkan.h"

#include "Vulkan/Device.hpp"

#include "Buffer.hpp"

#include <vector>
namespace Charra
{
	class HostAllocator
	{
	public:
		HostAllocator(Device* deviceRef, uint32_t memTypeIndex);
		~HostAllocator();

		void alloc(VkDeviceSize size, VkBuffer* buffer, VkDeviceSize* offsetIntoBuffer, void** pMappedData);
		void free(VkBuffer buffer, VkDeviceSize offset);
	private: // Methdos
		void createPage(VkDeviceSize size);

	private: // Members
		struct Page
		{
			VkBuffer buffer;
			VkDeviceSize size;
			VkDeviceSize alignment;
			VkDeviceSize freeBytes;
			// TODO this could improve performance, would need profiling
			//VkDeviceSize largestSpace;

			VkDeviceMemory memory;
			void* alloc;

			std::vector<Vec2> m_allocations;
			// All freespace are aligned on allocation
			std::vector<Vec2> m_freeSpaces;
		};

		Device* m_deviceRef;

		uint32_t m_minPageSize = 64 * 1024 * 1024; // 64MiB

		std::vector<Page> m_pages;

		uint32_t m_memTypeIndex;
	};
}

/*

	Allocator:
		AllocatorBufferFlags:
			CPU_ONLY,
			CPU_TO_GPU,
			AMD256_BUFFER


		AllocatorBufferInfo:
			uint64_t size;
			bool cpuToGpu;
			bool cpuPersist;


		Buffer allocateBuffer(uint64_t size);
		void deallocateBuffer(Buffer* buffer);
		void applyForTransfer(Buffer* buffer);

		VkCommandBuffer getTransferBuffer();
	
	HostAllocator:
		void alloc(HostAllocInfo* info, VkBuffer* buffer, void** pMappedData);
		void free(VkBuffer buffer, void* data);

	DeviceAllocator:
		DeviceAllocInfo:
			VkDeviceSize size;
			VkDeviceSize alignment;
			// This is for DeviceAllocator use only
			VkDeviceSize offsetIntoBuffer;

		void alloc(DeviceAllocInfo* info, VkBuffer* buffer);
		void free(VkBuffer buffer, VkDeviceSize offset);

		void alloc256(HostAllocInfo* info, VkBuffer* buffer, void** pMappedData);
		void free256(VkBuffer buffer, void* data);

*/