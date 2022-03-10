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
		VkBuffer buffer;
		// This is the offset into the buffer
		VkDeviceSize offset;
		// Size of data in bytes
		VkDeviceSize size;

		// If this is not nullptr then it is mapped to a buffer
		void* data = nullptr;

	private:
		// These are flags for interal allocator usage
		bool deviceBuffer; 
		bool amd256;

		friend class Allocator;
	};
}