#include "DeviceAllocator.hpp"

#include "Core/Logging.hpp"

namespace Charra
{
	DeviceAllocator::DeviceAllocator(Device& deviceRef, uint32_t deviceTypeIndex, uint32_t amd256TypeIndex)
	: m_deviceRef(deviceRef), m_deviceTypeIndex(deviceTypeIndex), m_amd256TypeIndex(amd256TypeIndex)
	{

	}

	DeviceAllocator::~DeviceAllocator()
	{
		CHARRA_LOG_ERROR(m_amd256Pages.size(), "Not all amd256 data was freed");
		CHARRA_LOG_ERROR(m_devicePages.size(), "Not all device memory was freed");
	}
	
	void DeviceAllocator::alloc(VkDeviceSize size, VkDeviceSize* offsetIntoBuffer, VkBuffer* buffer)
	{
		// This assumes worst case alignment, will get updated later
		VkDeviceSize alignedSize = ((size + (256 - 1)) & ~(256 - 1));
		int freeSpaceIndex = 0;
		DevicePage* selectedPage = nullptr;

		for(auto& page : m_devicePages)
		{
			if(page.freeBytes >= alignedSize)
			{
				for(int i = 0; i < page.m_freeSpaces.size(); i++)
				{
					if(page.m_freeSpaces[i].size >= alignedSize)
					{
						selectedPage = &page;
						freeSpaceIndex = i;
						break;
					}
				}
			}
		}

		if(!selectedPage)
		{
			addPage(alignedSize, false);
			selectedPage = &m_devicePages.back();
			freeSpaceIndex = 0;
		}

		if(selectedPage->alignment != 256)
		{
			alignedSize = ((size + (selectedPage->alignment - 1)) & ~(selectedPage->alignment - 1));
		}
		
		iVec2 allocation(selectedPage->m_freeSpaces[freeSpaceIndex].offset, static_cast<uint32_t>(alignedSize));
		selectedPage->m_allocations.push_back(allocation);

		*buffer = selectedPage->buffer; 
		*offsetIntoBuffer = static_cast<VkDeviceSize>(selectedPage->m_freeSpaces[freeSpaceIndex].offset);

		if(selectedPage->m_freeSpaces[freeSpaceIndex].size == alignedSize)
		{
			selectedPage->m_freeSpaces.erase(selectedPage->m_freeSpaces.begin() + freeSpaceIndex);
		}
		else
		{
			selectedPage->m_freeSpaces[freeSpaceIndex].offset += alignedSize;
			selectedPage->m_freeSpaces[freeSpaceIndex].size -= alignedSize;
		}

		selectedPage->freeBytes -= alignedSize;
	}

	void DeviceAllocator::free(VkBuffer buffer, VkDeviceSize offset)
	{
		DevicePage* selectedPage = nullptr;
		int selectedPageIndex;
		for(int i = 0; i < m_devicePages.size(); i++)
		{
			if(m_devicePages[i].buffer == buffer)
			{
				selectedPage = &m_devicePages[i];
				selectedPageIndex = i;
				break;
			}
		}

		CHARRA_LOG_ERROR(!selectedPage, "Attempt to free memory that is not allocated");
		CHARRA_LOG_ERROR(selectedPage->m_allocations.size() == 0, "Attempt to free memory that is not allocated");

		iVec2 allocInfo;
		for(int i = 0; i < selectedPage->m_allocations.size(); i++)
		{
			if(selectedPage->m_allocations[i].offset == offset)
			{
				allocInfo = selectedPage->m_allocations[i];
				selectedPage->m_allocations.erase(selectedPage->m_allocations.begin() + i);
			}
		}

		CHARRA_LOG_ERROR(allocInfo.size == 0, "Allocation free issue...");

		// Join free spaces
		VkDeviceSize start = allocInfo.offset;
		VkDeviceSize end = start + allocInfo.size;
		int merges = 0;
		for(int i = 0; i < selectedPage->m_freeSpaces.size(); i++)
		{
			if(selectedPage->m_freeSpaces[i].offset == end)
			{ // Space merges backwards
				selectedPage->m_freeSpaces[i].offset = start;
				merges++;
			}
			else if((selectedPage->m_freeSpaces[i].offset + selectedPage->m_freeSpaces[i].size) == start)
			{	// Space merges forwards
				selectedPage->m_freeSpaces[i].size += allocInfo.size;
				merges++;
			}

			if(merges == 2)
			{
				break;
			}
		}

		selectedPage->freeBytes += allocInfo.size;

		if(merges == 0)
		{
			selectedPage->m_freeSpaces.push_back(allocInfo);
		}

		if(selectedPage->freeBytes == selectedPage->size)
		{
			freePage(selectedPageIndex, false);
		}
	}

	void DeviceAllocator::alloc256(VkDeviceSize size, VkDeviceSize* offsetIntoBuffer, VkBuffer* buffer, void** mappedData)
	{
		if(m_amd256TypeIndex == UINT32_MAX)
		{
			CHARRA_LOG_WARNING(true, "No amd256 memory exists");
			return;
		}
		// Worst case alignment this gets updated later
		VkDeviceSize alignedSize = ((size + (128 - 1)) & ~(128 - 1));
		Amd256Page* selectedPage = nullptr;
		int freeSpaceIndex = UINT32_MAX;

		for(auto& page : m_amd256Pages)
		{
			if(page.freeBytes >= alignedSize)
			{
				for(int i = 0; i < page.m_freeSpaces.size(); i++)
				{
					if(page.m_freeSpaces[i].size >= alignedSize)
					{
						selectedPage = &page;
						freeSpaceIndex = i;
						break;
					}
				}
			}
		}

		if(!selectedPage)
		{
			addPage(size, true);
			selectedPage = &m_amd256Pages.back();
			freeSpaceIndex = 0;
		}

		if(selectedPage->alignment != 128)
		{
			alignedSize = ((size + (selectedPage->alignment - 1)) & ~(selectedPage->alignment - 1));
		}

		iVec2 allocation(selectedPage->m_freeSpaces[freeSpaceIndex].offset, static_cast<uint32_t>(size));
		selectedPage->m_allocations.push_back(allocation);

		*buffer = selectedPage->buffer; 
		*offsetIntoBuffer = static_cast<VkDeviceSize>(selectedPage->m_freeSpaces[freeSpaceIndex].offset);

		*mappedData = reinterpret_cast<void*>(reinterpret_cast<std::size_t>(selectedPage->alloc) + selectedPage->m_freeSpaces[freeSpaceIndex].offset);

		if(selectedPage->m_freeSpaces[freeSpaceIndex].size == alignedSize)
		{
			selectedPage->m_freeSpaces.erase(selectedPage->m_freeSpaces.begin() + freeSpaceIndex);
		}
		else
		{
			selectedPage->m_freeSpaces[freeSpaceIndex].offset += alignedSize;
		}

		selectedPage->freeBytes -= alignedSize;
	}

	void DeviceAllocator::free256(VkBuffer buffer, VkDeviceSize offset)
	{
		Amd256Page* selectedPage = nullptr;
		int selectedPageIndex;
		for(int i = 0; i < m_amd256Pages.size(); i++)
		{
			if(m_amd256Pages[i].buffer == buffer)
			{
				selectedPage = &m_amd256Pages[i];
				selectedPageIndex = i;
				break;
			}
		}

		CHARRA_LOG_ERROR(!selectedPage, "Attempt to free memory that is not allocated");
		CHARRA_LOG_ERROR(selectedPage->m_allocations.size() == 0, "Attempt to free memory that is not allocated");

		iVec2 allocInfo;
		for(int i = 0; i < selectedPage->m_allocations.size(); i++)
		{
			if(selectedPage->m_allocations[i].offset == offset)
			{
				allocInfo = selectedPage->m_allocations[i];
				selectedPage->m_allocations.erase(selectedPage->m_allocations.begin() + i);
			}
		}

		CHARRA_LOG_ERROR(allocInfo.size == 0, "Allocation free issue...");

		// Join free spaces
		VkDeviceSize start = allocInfo.offset;
		VkDeviceSize end = start + allocInfo.size;
		int merges = 0;
		for(int i = 0; i < selectedPage->m_freeSpaces.size(); i++)
		{
			if(selectedPage->m_freeSpaces[i].offset == end)
			{ // Space merges backwards
				selectedPage->m_freeSpaces[i].offset = start;
				merges++;
			}
			else if((selectedPage->m_freeSpaces[i].offset + selectedPage->m_freeSpaces[i].size) == start)
			{	// Space merges forwards
				selectedPage->m_freeSpaces[i].size += allocInfo.size;
				merges++;
			}

			if(merges == 2)
			{
				break;
			}
		}

		selectedPage->freeBytes += allocInfo.size;

		if(merges == 0)
		{
			selectedPage->m_freeSpaces.push_back(allocInfo);
		}

		if(selectedPage->freeBytes == selectedPage->size)
		{
			freePage(selectedPageIndex, false);
		}
	}

	void DeviceAllocator::addPage(VkDeviceSize size, bool amd256Page)
	{
		VkBufferCreateInfo bufferCreateInfo{};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.pNext;
		bufferCreateInfo.flags;
		bufferCreateInfo.size = (m_minPageSize >= size) ? m_minPageSize : size;
		// TODO research if this is bad
		bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT   | 
								 VK_BUFFER_USAGE_INDEX_BUFFER_BIT    |   
								 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT  |
								 VK_BUFFER_USAGE_TRANSFER_SRC_BIT    |
								 VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.queueFamilyIndexCount;
		bufferCreateInfo.pQueueFamilyIndices;

		if(amd256Page)
		{
			Amd256Page page{};
		
			vkCreateBuffer(m_deviceRef.getDevice(), &bufferCreateInfo, NULL, &page.buffer);
	
			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(m_deviceRef.getDevice(), page.buffer, &memRequirements);
	
			page.size = bufferCreateInfo.size;
			page.freeBytes = page.size;
			page.largestSpace = page.size;
			page.alignment = memRequirements.alignment;
	
			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.pNext;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = m_amd256TypeIndex;
	
			vkAllocateMemory(m_deviceRef.getDevice(), &allocInfo, NULL, &page.memory);
			vkBindBufferMemory(m_deviceRef.getDevice(), page.buffer, page.memory, 0);

			vkMapMemory(m_deviceRef.getDevice(), page.memory, 0, page.size, 0, &page.alloc);
	
			m_amd256Pages.push_back(page);
			iVec2 space(0, static_cast<uint32_t>(allocInfo.allocationSize));
			m_amd256Pages.back().m_freeSpaces.push_back(space);
		}
		else
		{
			DevicePage page{};
		
			vkCreateBuffer(m_deviceRef.getDevice(), &bufferCreateInfo, NULL, &page.buffer);
	
			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(m_deviceRef.getDevice(), page.buffer, &memRequirements);
	
			page.size = bufferCreateInfo.size;
			page.freeBytes = page.size;
			page.alignment = memRequirements.alignment;
	
			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.pNext;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = m_deviceTypeIndex;
	
			vkAllocateMemory(m_deviceRef.getDevice(), &allocInfo, NULL, &page.memory);
			vkBindBufferMemory(m_deviceRef.getDevice(), page.buffer, page.memory, 0);
	
			m_devicePages.push_back(page);
			iVec2 space(0, static_cast<uint32_t>(allocInfo.allocationSize));
			m_devicePages.back().m_freeSpaces.push_back(space);
		}
	}

	void DeviceAllocator::freePage(uint32_t pageIndex, bool amd256Page)
	{
		if(amd256Page)
		{
			auto& page = m_amd256Pages[pageIndex];
			
			vkDestroyBuffer(m_deviceRef.getDevice(), page.buffer, NULL);
			vkFreeMemory(m_deviceRef.getDevice(), page.memory, NULL);

			m_amd256Pages.erase(m_amd256Pages.begin() + pageIndex);
		}
		else
		{
			auto& page = m_devicePages[pageIndex];

			vkDestroyBuffer(m_deviceRef.getDevice(), page.buffer, NULL);
			vkFreeMemory(m_deviceRef.getDevice(), page.memory, NULL);

			m_devicePages.erase(m_devicePages.begin() + pageIndex);
		}
	}
}