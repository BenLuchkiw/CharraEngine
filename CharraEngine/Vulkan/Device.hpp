#pragma once

#include "vulkan/vulkan.h"

#include "Instance.hpp"

namespace Charra
{
	class Device
	{
	public:
		Device(Instance* instanceRef);
		~Device();


		VkDevice getDevice() { return m_device; }
		VkPhysicalDevice getPhysicalDevice() { return m_physicalDevice; }
		VkPhysicalDeviceProperties* getPhysicalDeviceProperties() { return &m_physicalDeviceProperties; }


		// If the index equals uint32_MAX not queue is available
		uint32_t getGraphicsQueueIndex() { return m_graphicsQueueIndex; }
		uint32_t getTransferQueueIndex() { return m_transferQueueIndex; }
		uint32_t getComputeQueueIndex()  { return m_computeQueueIndex; }

		VkQueue getGraphicsQueue() { return m_graphicsQueue; }
		VkQueue getTransferQueue() { return m_transferQueue; }
		VkQueue getComputeQueue() { return m_computeQueue; }

		std::vector<VkQueueFamilyProperties>* getQueueFamilyProperties() { return &m_queueFamilyProperties; }

		bool computeSupported() { return m_computeQueueIndex != UINT32_MAX; }

	private: // Methods
		void selectPhysicalDevice();
		void createLogicalDevice();
		void initQueues();
		void setQueues();

	private: // Members
		Instance* m_instanceRef;

		VkDevice m_device;
		VkPhysicalDevice m_physicalDevice;
		VkPhysicalDeviceProperties m_physicalDeviceProperties;

		uint32_t m_graphicsQueueIndex;
		uint32_t m_transferQueueIndex;

		bool m_computeSupported = false;
		uint32_t m_computeQueueIndex;

		VkQueue m_graphicsQueue;
		VkQueue m_transferQueue;
		VkQueue m_computeQueue;

		std::vector<VkQueueFamilyProperties> m_queueFamilyProperties;
	};
}