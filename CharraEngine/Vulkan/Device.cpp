#include "Device.hpp"
#include "Renderer/Renderer.hpp"

#include "Core/Logging.hpp"

#include <bitset>

namespace Charra
{
	Device::Device(Instance* instanceRef)
		: m_instanceRef(instanceRef),
		m_device(VK_NULL_HANDLE),
		m_physicalDevice(VK_NULL_HANDLE),
		m_physicalDeviceProperties(),
		m_graphicsQueueIndex(UINT32_MAX),
		m_computeQueueIndex(UINT32_MAX),
		m_graphicsQueue(VK_NULL_HANDLE),
		m_computeQueue(VK_NULL_HANDLE),
		m_queueFamilyProperties(0)
	{
		selectPhysicalDevice();
		initQueues();
		createLogicalDevice();
		setQueues();
	}

	Device::~Device()
	{
		vkDestroyDevice(m_device, NULL);
	}

	void Device::selectPhysicalDevice()
	{
		// Finding a list of physical devices
		uint32_t deviceCount;
		vkEnumeratePhysicalDevices(m_instanceRef->getInstance(), &deviceCount, NULL);
		CHARRA_LOG_ERROR(deviceCount == 0, "Vulkan failed to detect a GPU");
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_instanceRef->getInstance(), &deviceCount, devices.data());

		// Find which physical device is most suitable
		int winningPoints = 0;
		int currentPoints = 0;
		int winningIndex = 0;
		VkPhysicalDeviceProperties deviceProperties;
		for (int i = 0; i < deviceCount; i++)
		{
			vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);

			// Currently the first discrete device is chosen
			currentPoints += deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 1 : 0;
			
			if (currentPoints > winningPoints)
			{
				winningPoints = currentPoints;
				winningIndex = i;
			}
			currentPoints = 0;
		}

		uint32_t queueCount = 0;
		std::vector<VkQueueFamilyProperties> queueProperties;
		// Graphics = 1, compute = 2, transfer = 4
		m_physicalDevice = devices[winningIndex];
		vkGetPhysicalDeviceProperties(devices[winningIndex], &m_physicalDeviceProperties);
	}

	void Device::createLogicalDevice()
	{
		std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;

		// This has to be declared outside of the inner scope otherwise it may throw errors
		const float priority = 1.0f;

		if (m_computeQueueIndex == m_graphicsQueueIndex)
		{
			if (m_queueFamilyProperties.at(m_graphicsQueueIndex).queueCount == 1)
			{
				VkDeviceQueueCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				createInfo.pNext;
				createInfo.flags;
				createInfo.queueCount = 1;
				createInfo.queueFamilyIndex = m_graphicsQueueIndex;
				createInfo.pQueuePriorities = &priority;
				deviceQueueCreateInfos.emplace_back(createInfo);
			}
			else
			{
				VkDeviceQueueCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				createInfo.pNext;
				createInfo.flags;
				createInfo.queueCount = 2;
				createInfo.queueFamilyIndex = m_graphicsQueueIndex;
				createInfo.pQueuePriorities = &priority;
				deviceQueueCreateInfos.emplace_back(createInfo);
			}
		}
		else 
		{
			VkDeviceQueueCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			createInfo.pNext;
			createInfo.flags;
			createInfo.queueCount = 1;
			createInfo.queueFamilyIndex = m_graphicsQueueIndex;
			createInfo.pQueuePriorities = &priority;

			deviceQueueCreateInfos.emplace_back(createInfo); // Graphics queue

			createInfo.queueFamilyIndex = m_computeQueueIndex;

			deviceQueueCreateInfos.emplace_back(createInfo); // Compute queue
		}

		std::vector<const char*> extensions;
		extensions.push_back("VK_KHR_swapchain");

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pNext;
		deviceCreateInfo.flags;
		deviceCreateInfo.queueCreateInfoCount = deviceQueueCreateInfos.size();
		deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
		deviceCreateInfo.enabledLayerCount;   // Deprecated and ignored
		deviceCreateInfo.ppEnabledLayerNames; // Deprecated and ignored
		deviceCreateInfo.enabledExtensionCount = extensions.size();
		deviceCreateInfo.ppEnabledExtensionNames = extensions.data();
		deviceCreateInfo.pEnabledFeatures;

		CHARRA_LOG_ERROR(vkCreateDevice(m_physicalDevice, &deviceCreateInfo, NULL, &m_device) != VK_SUCCESS, "Vulkan was unable to create logical device");
	}

	void Device::initQueues()
	{
		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, NULL);
		CHARRA_LOG_ERROR(queueFamilyCount == 0, "Vulkan could not find any queue families for givin device");
		m_queueFamilyProperties.resize(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, m_queueFamilyProperties.data());

		uint32_t bestGraphicsQueueFamily = UINT32_MAX;
		uint32_t bestComputeQueuefamily = UINT32_MAX;
		uint32_t bestTransferQueueFamily = UINT32_MAX;
		uint32_t bestTransferBitCount = UINT32_MAX;
		for(int i = 0; i < m_queueFamilyProperties.size(); i++)
		{
			// I want graphics to be on the lowest queue family, 
			// Transfer to be on an dedicated queue family,
			// Compute to be separate if possible
			if((m_queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && (bestGraphicsQueueFamily == UINT32_MAX))
			{
				bestGraphicsQueueFamily = i;
				if(m_queueFamilyProperties[i].queueCount == 1)
				{
					continue;
				}
			}

			if((m_queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) && (std::bitset<sizeof(uint32_t)>(m_queueFamilyProperties[i].queueFlags).count() < bestTransferBitCount))
			{
				bestTransferBitCount = std::bitset<sizeof(uint32_t)>(m_queueFamilyProperties[i].queueFlags).count();
				bestTransferQueueFamily = i;
			}

			if(m_queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				bestComputeQueuefamily = i;
				m_computeSupported = true;
			}
		}

		CHARRA_LOG_ERROR(bestGraphicsQueueFamily == UINT32_MAX, "Vulkan could not find a suitable graphics queue");
		CHARRA_LOG_ERROR(bestTransferBitCount == UINT32_MAX, "Vulkan could not find a suitable transfer queue");
		CHARRA_LOG_WARNING(bestComputeQueuefamily == UINT32_MAX, "Vulkan could not find a suitable compute queue");
		m_graphicsQueueIndex = bestGraphicsQueueFamily;
		m_transferQueueIndex = bestTransferQueueFamily;
		m_computeQueueIndex = bestComputeQueuefamily;
	}

	void Device::setQueues()
	{
		// The graphics queue will be given priority
		vkGetDeviceQueue(m_device, m_graphicsQueueIndex, 0, &m_graphicsQueue);

		// The compute queue will be different from graphics queue if possible
		if (m_graphicsQueueIndex != m_computeQueueIndex) // If compute is a different family
		{
			vkGetDeviceQueue(m_device, m_computeQueueIndex, 0, &m_computeQueue);
		}
		else if (m_queueFamilyProperties[m_computeQueueIndex].queueCount > 1) // If compute is same family but multiple queues are available
		{
			vkGetDeviceQueue(m_device, m_computeQueueIndex, 1, &m_computeQueue);
		}
		else // Compute is on same queue as graphics
		{
			vkGetDeviceQueue(m_device, m_computeQueueIndex, 0, &m_computeQueue);
		}
	}
}