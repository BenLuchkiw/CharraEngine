#include "MemoryManager.hpp"

#include "Core/Logging.hpp"

namespace Charra
{
	MemoryManager::MemoryManager(Device* deviceRef)
	: m_deviceRef(deviceRef), m_transferFinishedFence(deviceRef), m_semaphore(deviceRef), m_commandBuffer(deviceRef, 1, CommandBufferType::TRANSFER)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(m_deviceRef->getPhysicalDevice(), &memProperties);

		// The host visible memory must be host coherent, and cached in preferable
		// If device local is separate then pick the largest heap

		// If the device is integrated this is easier
		if(deviceRef->getPhysicalDeviceProperties()->deviceType & VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
		{
			uint32_t largestHeapSize = 0;
			uint32_t bestIndex = UINT32_MAX;
			for(int i = 0; i < memProperties.memoryHeapCount; i++)
			{
				if(memProperties.memoryHeaps[i].size > largestHeapSize)
				{
					largestHeapSize = memProperties.memoryHeaps[i].size;
					bestIndex = memProperties.memoryTypes[i].heapIndex;
					m_cpuTypeIndex = i;
				}
			}

			CHARRA_LOG_ERROR(bestIndex == UINT32_MAX, "Vulkan could not select a good heap");
			m_CPUHeapIndex = bestIndex;
		}
		else
		{
			uint32_t bestGPUHeapIndex = UINT32_MAX;
			uint32_t bestGPUHeapSize = 0;

			uint32_t bestCPUHeapIndex = UINT32_MAX;
			uint32_t bestCPUHeapSize = 0;
			bool cached = false;

			for(int i = 0; i < memProperties.memoryTypeCount; i++)
			{
				auto& flags = memProperties.memoryTypes[i].propertyFlags;
	
				if(flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
				{
					bestGPUHeapIndex = memProperties.memoryTypes[i].heapIndex;
					bestGPUHeapSize = memProperties.memoryHeaps[memProperties.memoryTypes[i].heapIndex].size;
					m_gpuTypeIndex = i;
				}

				if(flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
				{
					if(flags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)
					{
						if(cached)
						{
							if(memProperties.memoryHeaps[memProperties.memoryTypes[i].heapIndex].size > bestCPUHeapSize)
							{
								bestCPUHeapSize = memProperties.memoryHeaps[memProperties.memoryTypes[i].heapIndex].size;
								bestCPUHeapIndex = memProperties.memoryTypes[i].heapIndex;
								m_cpuTypeIndex = i;
							}
						}
						else
						{
							bestCPUHeapSize = memProperties.memoryHeaps[memProperties.memoryTypes[i].heapIndex].size;
							bestCPUHeapIndex = memProperties.memoryTypes[i].heapIndex;
							m_cpuTypeIndex = i;
						}
					}
				}
			}

			CHARRA_LOG_ERROR(bestCPUHeapIndex == UINT32_MAX, "Vulkan could not find any host memory");
			CHARRA_LOG_WARNING(bestGPUHeapIndex == UINT32_MAX, "Vulkan could not find any device local memory for a dedicated GPU");
		}

	}

	MemoryManager::~MemoryManager()
	{

	}

	void MemoryManager::allocateBuffer(Buffer* buffer)
	{
		VkMemoryRequirements memRequirements;
		VkBuffer hostBuffer = VK_NULL_HANDLE;

		if(m_GPUHeapIndex != UINT32_MAX)
		{
			VkBufferCreateInfo hostBufferInfo{};
			hostBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			hostBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			hostBufferInfo.size = buffer->dataSize;
			hostBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

			VkBufferCreateInfo deviceBufferInfo{};
			deviceBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			deviceBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			deviceBufferInfo.size = buffer->dataSize;
			deviceBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;

			switch(buffer->type)
			{
			case BufferType::VERTEX:
				hostBufferInfo.usage   |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
				deviceBufferInfo.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
				break;

			case BufferType::INDEX:
				hostBufferInfo.usage   |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
				deviceBufferInfo.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
				break;

			case BufferType::UNIFORM:
				hostBufferInfo.usage   |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
				deviceBufferInfo.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
				break;
			}

			CHARRA_LOG_ERROR(vkCreateBuffer(m_deviceRef->getDevice(), &hostBufferInfo, NULL, &hostBuffer) != VK_SUCCESS, "Vulkan could not create buffer");
			CHARRA_LOG_ERROR(vkCreateBuffer(m_deviceRef->getDevice(), &deviceBufferInfo, NULL, &buffer->buffer) != VK_SUCCESS, "Vulkan could  not create buffer");

			VkMemoryRequirements tempMemRequirements;
			vkGetBufferMemoryRequirements(m_deviceRef->getDevice(), hostBuffer, &tempMemRequirements);
			vkGetBufferMemoryRequirements(m_deviceRef->getDevice(), buffer->buffer, &memRequirements);

			if(tempMemRequirements.size > memRequirements.size)
			{
				memRequirements.size = memRequirements.size;
			}
		}
		else 
		{
			VkBufferCreateInfo hostBuffer{};
			hostBuffer.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			hostBuffer.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			hostBuffer.size = buffer->dataSize;

			switch(buffer->type)
			{
			case BufferType::VERTEX:
				hostBuffer.usage   |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
				break;

			case BufferType::INDEX:
				hostBuffer.usage   |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
				break;

			case BufferType::UNIFORM:
				hostBuffer.usage   |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
				break;
			}

			CHARRA_LOG_ERROR(vkCreateBuffer(m_deviceRef->getDevice(), &hostBuffer, NULL, &buffer->buffer) != VK_SUCCESS, "Vulkan could not create buffer");
		}

		// Select page(s)
		MemoryPage* hostPage = nullptr;
		MemoryPage* devicePage = nullptr;
		for(int i = 0; i < m_pages.size(); i++)
		{
			if((hostBuffer != VK_NULL_HANDLE) && (m_pages[i].type == PageType::DEVICE))
			{
				if(m_pages[i].largestSpace < memRequirements.size)
				{
					devicePage = &m_pages[i];
				}
			}
			else
			{
				if(m_pages[i].largestSpace < memRequirements.size)
				{
					hostPage = &m_pages[i];
				}
			}

		}

		if(!hostPage)
		{
			addCPUPage(memRequirements.size);
			hostPage = &m_pages.back();
		}
		if(!devicePage && (hostBuffer != VK_NULL_HANDLE))
		{
			addGPUPage(memRequirements.size);
			devicePage = &m_pages.back();
		}

		// Map data into cpu buffer
		void* data;
		uint32_t offset;
		for(Vec2& freeSpace : hostPage->freeSpaces)
		{
			if(freeSpace.size >= memRequirements.size)
			{
				offset = freeSpace.offset;
				vkMapMemory(m_deviceRef->getDevice(), hostPage->memory, offset, memRequirements.size, 0, &data);
				freeSpace.offset += memRequirements.size;
				freeSpace.size -= memRequirements.size;
				hostPage->freeBytes -= memRequirements.size;
			}
		}
		memcpy(data, buffer->data, memRequirements.size);

		hostPage->allocations.emplace_back(++m_internalId, memRequirements.size);
	
		buffer->allocationId = hostPage->allocations.back().allocationId;
		buffer->pageID = hostPage->id;

		if(hostBuffer != VK_NULL_HANDLE)
		{
			m_transfers.emplace_back(hostBuffer, buffer->buffer, memRequirements, offset);
		}
	}

	void MemoryManager::deAllocateBuffer(Buffer* buffer)
	{
		for(int i = 0; i < m_pages.size(); i++)
		{
			if(m_pages[i].id == buffer->pageID)
			{
				for(int j = 0; j < m_pages[i].allocations.size(); j++)
				{
					if(m_pages[i].allocations[j].allocationId == buffer->allocationId)
					{
						vkDestroyBuffer(m_deviceRef->getDevice(), buffer->buffer, NULL);

						Vec2 space = { m_pages[i].allocations[j].startOffset, m_pages[i].allocations[j].size};
						m_pages[i].freeSpaces.push_back(space);
						m_pages[i].freeBytes += m_pages[i].allocations[j].size;

						m_pages[i].allocations.erase(m_pages[i].allocations.begin() + j);

						if(m_pages[i].type == PageType::HOST)
						{
							vkUnmapMemory(m_deviceRef->getDevice(), m_pages[i].memory);
						}
					}
					break;
				}
			}
			break;
		}
	}

	void defragment(uint32_t numPages)
	{

	}

	VkSemaphore MemoryManager::submitTransfers()
	{
		// This needs to check that the device pages have enough space

		m_commandBuffer.beginRecording(0);
		uint32_t offset;

		for(const auto& transfer : m_transfers)
		{
			bool pageFound = false;
			for(auto& page : m_pages)
			{
				if(page.type == PageType::DEVICE)
				{	
					if(page.freeBytes < transfer.memRequirements.size)
					{
						continue;
					}

					for(int i = 0; i < page.freeSpaces.size(); i++)
					{
						if(page.freeSpaces[i].size >= transfer.memRequirements.size)
						{
							vkBindBufferMemory(m_deviceRef->getDevice(), transfer.dst, page.memory, page.freeSpaces[i].offset);

							offset = page.freeSpaces[i].offset;
							page.freeSpaces[i].offset += transfer.memRequirements.size;
							page.freeSpaces[i].size -= transfer.memRequirements.size;
							page.freeBytes -= transfer.memRequirements.size;
						}
					}
				}
			}
			if(!pageFound)
			{
				addGPUPage(transfer.memRequirements.size);
				vkBindBufferMemory(m_deviceRef->getDevice(), transfer.dst, m_pages.back().memory, 0);

				offset = m_pages.back().freeSpaces[0].offset;
				m_pages.back().freeSpaces[0].offset += transfer.memRequirements.size;
				m_pages.back().freeSpaces[0].size -= transfer.memRequirements.size;
				m_pages.back().freeBytes -= transfer.memRequirements.size;
			}

			VkBufferCopy copyData;
			copyData.srcOffset = transfer.srcOffset;
			copyData.dstOffset = offset;
			copyData.size = transfer.memRequirements.size;
			vkCmdCopyBuffer(m_commandBuffer.getCommandBuffer(0), transfer.src, transfer.dst, 1, &copyData);
		}

		m_commandBuffer.endRecording(0);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext;
		submitInfo.waitSemaphoreCount;
		submitInfo.pWaitSemaphores;
		submitInfo.pWaitDstStageMask;
		submitInfo.commandBufferCount = 1;
		VkCommandBuffer cb[1] = {m_commandBuffer.getCommandBuffer(0)};
		submitInfo.pCommandBuffers = cb;
		submitInfo.signalSemaphoreCount = 1;
		VkSemaphore semaphores[1] = {m_semaphore.getSemaphore()};
		submitInfo.pSignalSemaphores = semaphores;

		VkFence fence[] = {m_transferFinishedFence.getFence()};
		vkWaitForFences(m_deviceRef->getDevice(), 1, fence, true, UINT32_MAX);

		vkQueueSubmit(m_deviceRef->getTransferQueue(), 1, &submitInfo, m_transferFinishedFence.getFence());
	
		return m_semaphore.getSemaphore();
	}	

	void MemoryManager::addGPUPage(VkDeviceSize minSize)
	{
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.pNext;
		allocInfo.allocationSize = (minSize > pageSize) ? minSize : pageSize;
		allocInfo.memoryTypeIndex = m_gpuTypeIndex;

		VkDeviceMemory memory;
		vkAllocateMemory(m_deviceRef->getDevice(), &allocInfo, NULL, &memory);
		m_pages.emplace_back(MemoryPage(++m_internalId, PageType::DEVICE, allocInfo.allocationSize, memory));
	}

	void MemoryManager::addCPUPage(VkDeviceSize minSize)
	{
		VkDeviceMemory memory;
		vkMapMemory(m_deviceRef->getDevice(), 
		vkAllocateMemory(m_deviceRef->getDevice(), &allocInfo, NULL, &memory);
		m_pages.emplace_back(MemoryPage(++m_internalId, PageType::HOST, allocInfo.allocationSize, memory));
	}
}