#pragma once

#include "vulkan/vulkan.h"

#include "Math/RendererTypes.hpp"

#include <vector>

namespace Charra
{
	namespace BufferType
	{
		const int CPU    = 1;
		const int GPU    = 2;
		const int AMD256 = 4;
	}
	typedef int BufferTypeFlags;
	
	struct Buffer
	{
	public:

		// This VkBuffer does not contain just the relevant data
		// So ownership is not unique to this data
		VkBuffer buffer = VK_NULL_HANDLE;
		// This is the offset into the buffer
		VkDeviceSize offset = 0;
		// Size of data in bytes
		VkDeviceSize size = 0;

		// Flag for Charra::BufferManager to know if this buffer is in use
		bool inUse = false;

		void zeroBuffer()
		{
			buffer = VK_NULL_HANDLE;
			offset = 0;
			size = 0;
			memory = VK_NULL_HANDLE;
			deviceBuffer = false;
			amd256 = false;
			data = nullptr;
		}


	private:
		// These are flags for interal allocator usage
		VkDeviceMemory memory = VK_NULL_HANDLE;
		bool deviceBuffer = false; 
		bool amd256 = false;

		// If this is not nullptr then it is mapped to a buffer
		void* data = nullptr;
		
		friend class Allocator;
	};
}