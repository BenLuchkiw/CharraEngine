#pragma once

#include "vulkan/vulkan.h"

#include "Vulkan/Device.hpp"

#include "Buffer.hpp"

#include "Math/MathTypes.hpp"

#include <vector>

namespace Charra
{
	class DeviceAllocator
	{
	public:
		DeviceAllocator(Device* deviceRef, uint32_t deviceTypeIndex, uint32_t amd256TypeIndex);
		~DeviceAllocator();

		void alloc(VkDeviceSize size, VkDeviceSize* pOffsetIntoBuffer, VkBuffer* buffer);
		void free(VkBuffer buffer, VkDeviceSize offset);

		void alloc256(VkDeviceSize size, VkDeviceSize* pOffsetIntoBuffer, VkBuffer* buffer, void** pMappedData);
		void free256(VkBuffer buffer, VkDeviceSize offset);

	private: // Method
		void addPage(VkDeviceSize size, bool amd256Page);
		void freePage(uint32_t pageIndex, bool amd256Page);

	private: // Member
		struct Amd256Page
		{
			VkBuffer buffer;
			VkDeviceSize size;
			VkDeviceSize alignment;
			VkDeviceSize freeBytes;
			VkDeviceSize largestSpace;

			VkDeviceMemory memory;
			void* alloc;

			std::vector<iVec2> m_allocations;
			// All freespace are aligned on allocation
			std::vector<iVec2> m_freeSpaces;
		};

		struct DevicePage
		{
			VkBuffer buffer;
			VkDeviceSize size;
			VkDeviceSize alignment;
			VkDeviceSize freeBytes;
			// TODO this could be a possible performance improvements, would require profiling
			//VkDeviceSize largestSpace;

			VkDeviceMemory memory;

			std::vector<iVec2> m_allocations;
			// All freespace are aligned on allocation
			std::vector<iVec2> m_freeSpaces;
		};

		Device* m_deviceRef;
		
		uint32_t m_deviceTypeIndex;
		uint32_t m_amd256TypeIndex;

		VkDeviceSize m_minPageSize = 64 * 1024 * 1024;

		std::vector<Amd256Page> m_amd256Pages;
		std::vector<DevicePage> m_devicePages;
	};
}

/*
*/