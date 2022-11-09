#include "Allocator.hpp"

#include "Core/Logging.hpp"

namespace Charra
{
	Allocator::Allocator(Device& deviceRef)
	: m_deviceRef(deviceRef), m_commandBuffers(deviceRef, 1, CommandBufferType::TRANSFER), m_hostAllocator(nullptr), m_deviceAllocator(nullptr)
	{
		if(m_deviceRef.getPhysicalDeviceProperties()->deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
		{
			VkMemoryPropertyFlags flags;

			uint32_t result;
			// Ideal storage
			flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT |
					VK_MEMORY_PROPERTY_HOST_COHERENT_BIT|
					VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
			result = findType(flags);

			if(result != UINT32_MAX)
			{
				m_hostAllocator = std::unique_ptr<HostAllocator>(std::make_unique<HostAllocator>(deviceRef, result));
				m_hostCoherant = true;
			}
			else
			{
				// Cached but needs invalidation/flushing
				flags ^= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
				result = findType(flags);

				if(result != UINT32_MAX)
				{
					m_hostAllocator = std::unique_ptr<HostAllocator>(std::make_unique<HostAllocator>(deviceRef, result));
					m_hostCoherant = false;
				}
				else
				{
					// coherent but not cached
					flags ^= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
							VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
					result = findType(flags);

					if(result != UINT32_MAX)
					{
						m_hostAllocator = std::unique_ptr<HostAllocator>(std::make_unique<HostAllocator>(deviceRef, result));
						m_hostCoherant = true;
					}
					else
					{
						CHARRA_LOG_ERROR(true, "Vulkan could not find a suitable memory type");
					}
				}
			}
		}
		else // Dedicated card
		{
			// Find best host type
			VkMemoryPropertyFlags flags;
			uint32_t result;

			// Ideal
			flags = VK_MEMORY_PROPERTY_HOST_CACHED_BIT   |
					VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

			result = findType(flags);
			if(result != UINT32_MAX)
			{
				m_hostAllocator = std::unique_ptr<HostAllocator>(std::make_unique<HostAllocator>(deviceRef, result));
			}
			else
			{
				// Cached but needs invalidation/flushing
				flags ^= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
				result = findType(flags);

				if(result != UINT32_MAX)
				{
					m_hostAllocator = std::unique_ptr<HostAllocator>(std::make_unique<HostAllocator>(deviceRef, result));
				}
				else
				{
					// coherent but not cached
					flags ^= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
							VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
					result = findType(flags);

					if(result != UINT32_MAX)
					{
						m_hostAllocator = std::unique_ptr<HostAllocator>(std::make_unique<HostAllocator>(deviceRef, result));
					}
					else
					{
						CHARRA_LOG_ERROR(true, "Vulkan could not find a suitable memory type");
					}
				}
			}

			// Best device mem type
			result = findType(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			CHARRA_LOG_ERROR(result == UINT32_MAX, "Vulkan could not find suitable device heap");

			// amd256
			uint32_t amd256TypeIndex = findType(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

			m_deviceAllocator = std::unique_ptr<DeviceAllocator>(std::make_unique<DeviceAllocator>(deviceRef, result, amd256TypeIndex));

			m_availableTypes |= BufferType::GPU;
			m_availableTypes |= (amd256TypeIndex == UINT32_MAX) ? 0 : BufferType::AMD256;
		}
	}

	Allocator::~Allocator()
	{

	}

	void Allocator::submitData(Buffer& buffer, void* data, size_t bytes, size_t offset)
	{
		char* ptr = static_cast<char*>(data);
		*ptr += offset;

		memcpy(buffer.data, ptr, bytes);

		if(!m_hostCoherant)
		{
			VkMappedMemoryRange range;
			range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			range.pNext;
			range.size = bytes;
			range.offset = buffer.offset;
			range.memory = buffer.memory;

			vkFlushMappedMemoryRanges(m_deviceRef.getDevice(), 1, &range);
		}
	}

	Buffer Allocator::allocateBuffer(uint64_t size, BufferTypeFlags type)
	{
		Buffer buffer{};

		if(type == BufferType::CPU)
		{
			m_hostAllocator->alloc(&buffer.memory, size, &buffer.buffer, &buffer.offset, &buffer.data);
			buffer.deviceBuffer = false;
			buffer.amd256 = false;
		}
		else if(type == BufferType::GPU)
		{
			m_deviceAllocator->alloc(size, &buffer.offset, &buffer.buffer);
			buffer.deviceBuffer = true;
			buffer.amd256 = false;
		}
		else if(type == BufferType::AMD256)
		{
			m_deviceAllocator->alloc256(size, &buffer.offset, &buffer.buffer, &buffer.data);
			buffer.deviceBuffer = true;
			buffer.amd256 = true;
		}
		else
		{
			CHARRA_LOG_ERROR(true, "Buffer can only have one type");
			return buffer;
		}

		buffer.size = size;
		return buffer;
	}

	void Allocator::deallocateBuffer(Buffer* buffer)
	{
		if(buffer->deviceBuffer)
		{
			if(buffer->amd256)
			{
				m_deviceAllocator->free256(buffer->buffer, buffer->offset);
			}
			else
			{
				m_deviceAllocator->free(buffer->buffer, buffer->offset);
			}
		}
		else
		{
			m_hostAllocator->free(buffer->buffer, buffer->offset);
		}
	}

	void Allocator::applyForTransfer(Buffer* src, Buffer* dst)
	{
		VkBufferCopy copyData;
		copyData.srcOffset = src->offset;
		copyData.dstOffset = dst->offset;
		copyData.size = (src->size > dst->size) ? src->size : dst->size;

		for(auto& transfer : m_transfers)
		{
			if(transfer.dst == dst->buffer && transfer.src == src->buffer)
			{
				transfer.m_copies.push_back(copyData);
				return;
			}
		}

		Transfer transferData;
		transferData.src = src->buffer;
		transferData.dst = dst->buffer;
		transferData.m_copies.push_back(copyData);

		m_transfers.push_back(transferData);
	}

	VkCommandBuffer Allocator::getTransferBuffer()
	{
		m_commandBuffers.beginRecording(0);

		for(int i = 0; i < m_transfers.size(); i++)
		{
			vkCmdCopyBuffer(m_commandBuffers.getCommandBuffer(0), m_transfers[i].src, m_transfers[i].dst,
							m_transfers[i].m_copies.size(), m_transfers[i].m_copies.data());
		}

		m_transfers.clear();

		m_commandBuffers.endRecording(0);

		return m_commandBuffers.getCommandBuffer(0);
	}

	uint32_t Allocator::findType(VkMemoryPropertyFlags flags)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(m_deviceRef.getPhysicalDevice(), &memProperties);
		
		uint32_t bestIndex = UINT32_MAX;
		VkDeviceSize largestHeap = 0;

		for(int i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if(memProperties.memoryTypes[i].propertyFlags & flags)
			{
				if(memProperties.memoryHeaps[memProperties.memoryTypes[i].heapIndex].size > largestHeap)
				{
					bestIndex = i;
					largestHeap = memProperties.memoryHeaps[memProperties.memoryTypes[i].heapIndex].size;					
				}
			}
		}

		return bestIndex;
	}
}

