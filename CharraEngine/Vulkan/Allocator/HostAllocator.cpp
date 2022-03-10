#include "HostAllocator.hpp"

#include "Core/Logging.hpp"

namespace Charra
{
	HostAllocator::HostAllocator(Device* deviceRef, uint32_t memTypeIndex)
	: m_deviceRef(deviceRef), m_memTypeIndex(memTypeIndex)
	{

	}

	HostAllocator::~HostAllocator()
	{
		CHARRA_LOG_ERROR(m_pages.size(), "Not all host pages were freed");
	}

	void HostAllocator::alloc(VkDeviceSize size, VkBuffer* buffer, VkDeviceSize* offsetIntoBuffer, void** pMappedData)
	{
		// Worst case alignment this gets updated later
		VkDeviceSize alignedSize = ((size + (128 - 1)) & ~(128 - 1));
		Page* selectedPage = nullptr;
		int freeSpaceIndex = UINT32_MAX;

		for(auto& page : m_pages)
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
			createPage(size);
			selectedPage = &m_pages.back();
			freeSpaceIndex = 0;
		}

		if(selectedPage->alignment != 128)
		{
			alignedSize = ((size + (selectedPage->alignment - 1)) & ~(selectedPage->alignment - 1));
		}

		Vec2 allocation = {selectedPage->m_freeSpaces[freeSpaceIndex].offset, static_cast<uint32_t>(size)};
		selectedPage->m_allocations.push_back(allocation);

		*buffer = selectedPage->buffer; 
		*offsetIntoBuffer = static_cast<VkDeviceSize>(selectedPage->m_freeSpaces[freeSpaceIndex].offset);

		*pMappedData = reinterpret_cast<void*>(reinterpret_cast<std::size_t>(selectedPage->alloc) + selectedPage->m_freeSpaces[freeSpaceIndex].offset);

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

	void HostAllocator::free(VkBuffer buffer, VkDeviceSize offset)
	{
		Page* selectedPage = nullptr;
		int selectedPageIndex;
		for(int i = 0; i < m_pages.size(); i++)
		{
			if(m_pages[i].buffer == buffer)
			{
				selectedPage = &m_pages[i];
				selectedPageIndex = i;
				break;
			}
		}

		CHARRA_LOG_ERROR(!selectedPage, "Attempt to free memory that is not allocated");
		CHARRA_LOG_ERROR(selectedPage->m_allocations.size() == 0, "Attempt to free memory that is not allocated");

		Vec2 allocInfo{};
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
		else
		{
			if(selectedPage->freeBytes == selectedPage->size)
			{
				vkUnmapMemory(m_deviceRef->getDevice(), selectedPage->memory);
				vkFreeMemory(m_deviceRef->getDevice(), selectedPage->memory, NULL);
				vkDestroyBuffer(m_deviceRef->getDevice(), selectedPage->buffer, NULL);
				m_pages.erase(m_pages.begin() + selectedPageIndex);
			}
		}
	}

	void HostAllocator::createPage(VkDeviceSize size)
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

		Page page{};
		vkCreateBuffer(m_deviceRef->getDevice(), &bufferCreateInfo, NULL, &page.buffer);

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(m_deviceRef->getDevice(), page.buffer, &memRequirements);

		page.size = bufferCreateInfo.size;
		page.freeBytes = page.size;
		page.alignment = memRequirements.alignment;

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.pNext;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = m_memTypeIndex;

		vkAllocateMemory(m_deviceRef->getDevice(), &allocInfo, NULL, &page.memory);
		vkBindBufferMemory(m_deviceRef->getDevice(), page.buffer, page.memory, 0);

		vkMapMemory(m_deviceRef->getDevice(), page.memory, 0, page.size, 0, &page.alloc);

		m_pages.push_back(page);
		Vec2 space = {0, static_cast<uint32_t>(allocInfo.allocationSize)};
		m_pages.back().m_freeSpaces.push_back(space);
	}
}