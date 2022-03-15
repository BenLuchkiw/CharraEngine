#pragma once

#include "CommandBuffers.hpp"
#include "Device.hpp"
#include "Syncronization.hpp"

#include "vulkan/vulkan.h"

#include <vector>

namespace Charra
{
	class Memory
	{
	public:

	private: // Members

	private: // Methods
		struct FreeBlock
		{
			VkDeviceSize size;
			VkDeviceSize offset;
		};

		struct AllocatedBlock
		{
			VkDeviceSize allocationSize;
			VkDeviceSize blockSize;
			VkDeviceSize offset;

			VkBuffer buffer;
		};

		struct Page
		{
			VkDeviceSize size;
			VkDeviceSize freeBytes;
			VkDeviceSize largestSpace;

			std::vector<AllocatedBlock> allocations;
			std::vector<FreeBlock> freeBlocks;
		};

		std::vector<Page> m_hostPages;
		std::vector<Page> m_devicePage;

		VkDeviceSize m_minPageSize = 64 * 1028 * 1028; // 64MiB
	};

	class MemoryManager
	{
	public:
		MemoryManager(Device* deviceRef);
		~MemoryManager();


		// Gives values to the m_gpuBuffer and m_cpuBuffer
		// depending on memory types/configurations,
		// and if queues a transfer operation if it is needed
		void allocateBuffer(Buffer* buffer);
		void deAllocateBuffer(Buffer* buffer);


		// This need to be submitted before the gpu buffers can be bound
		// if the returned semaphore is not VK_NULL_HANDLE
		// it should be waited on by whatever draw call uses the buffers
		VkSemaphore submitTransfers();
	private: // Methods
		// If min size is greater than page size then it will get a unique allocation

		void addGPUPage(VkDeviceSize minSize);
		void addCPUPage(VkDeviceSize minSize);

	private: // Members
		struct Transfers
		{
			VkBuffer src = VK_NULL_HANDLE;
			VkBuffer dst = VK_NULL_HANDLE;
			VkMemoryRequirements memRequirements;
			VkDeviceSize srcOffset;

			Transfers(VkBuffer Src, VkBuffer Dst, VkMemoryRequirements MemRequirements, VkDeviceSize SrcOffset)
			: src(Src), dst(Dst), memRequirements(MemRequirements), srcOffset(SrcOffset) {}
		};

		struct Allocation
		{
			// What page it belongs to
			uint32_t allocationId;
			VkDeviceSize startOffset;
			VkDeviceSize size;

			Allocation(uint32_t id, VkDeviceSize Size) 
			: allocationId(id), startOffset(0), size(Size) {}
		};

		enum class PageType
		{
			DEVICE,
			HOST
		};

		struct MemoryPage
		{
			// This is the reinterpret_cast<uint32_t>(memory)
			uint32_t id;
			PageType type;

			VkDeviceSize size;
			VkDeviceMemory memory;
			// This is to help decide whether defragmentation is needed
			VkDeviceSize freeBytes;
			VkDeviceSize largestSpace;

			std::vector<Allocation> allocations;
			std::vector<Vec2> freeSpaces;

			MemoryPage(uint32_t Id, PageType Type, VkDeviceSize Size, VkDeviceMemory Memory)
			: id(Id), type(Type), size(Size), memory(Memory), freeBytes(Size), largestSpace(Size), allocations(), freeSpaces() {}
		};

		Device* m_deviceRef;
		Fence m_transferFinishedFence;
		Semaphore m_semaphore;
		std::vector<Transfers> m_transfers;
		std::vector<VkBuffer> m_buffersToDestroy;

		std::vector<MemoryPage> m_pages;

		CommandBuffers m_commandBuffer;

		uint32_t m_GPUHeapIndex = UINT32_MAX;
		uint32_t m_CPUHeapIndex = UINT32_MAX;
		uint32_t m_gpuTypeIndex = UINT32_MAX;
		uint32_t m_cpuTypeIndex = UINT32_MAX;

		// TODO make this better
		// 10 MiB
		VkDeviceSize pageSize = 20 * 1024 * 1024;

		uint32_t m_internalId = 0; 
	};
}