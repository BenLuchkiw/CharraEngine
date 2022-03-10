#pragma once

#include "vulkan/vulkan.h"

#include "Device.hpp"

namespace Charra
{
	class Semaphore
	{
	public:
		Semaphore(Device* deviceRef);
		~Semaphore();

		// Move constructors so that vectors work, TODO research what this actually does
		Semaphore(Semaphore&&) noexcept {}
		Semaphore& operator=(Semaphore&&) noexcept {}

		inline VkSemaphore getSemaphore() { return m_semaphore; }
		
	private: // Methods

	private: // Members
		Device* m_deviceRef;
		VkSemaphore m_semaphore;
	};

	class Fence
	{
	public:
		Fence(Device* deviceRef);
		~Fence();

		// Move constructors so that vectors work, TODO research what this actually does
		Fence(Fence&&) noexcept {}
		Fence& operator=(Fence&&) noexcept {}

		inline VkFence getFence() { return m_fence; }
		inline void reset() { vkResetFences(m_deviceRef->getDevice(), 1, &m_fence); }

	private: // Methods

	private: // Members
		Device* m_deviceRef;
		VkFence m_fence;
	};
}