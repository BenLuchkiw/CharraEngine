#include "Syncronization.hpp"

namespace Charra
{
	Semaphore::Semaphore(Device& deviceRef)
		: m_deviceRef(deviceRef)
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.pNext;
		semaphoreCreateInfo.flags;

		vkCreateSemaphore(m_deviceRef.getDevice(), &semaphoreCreateInfo, NULL, &m_semaphore);
	}

	Semaphore::~Semaphore()
	{
		vkDestroySemaphore(m_deviceRef.getDevice(), m_semaphore, NULL);
	}
	


	Fence::Fence(Device& deviceRef)
		: m_deviceRef(deviceRef)
	{
		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.pNext;
		fenceCreateInfo.flags;

		vkCreateFence(m_deviceRef.getDevice(), &fenceCreateInfo, NULL, &m_fence);
	}

	Fence::~Fence()
	{
		vkDestroyFence(m_deviceRef.getDevice(), m_fence, NULL);
	}
}