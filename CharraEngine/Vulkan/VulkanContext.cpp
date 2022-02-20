#include "VulkanContext.hpp"

#include "Core/Logging.hpp"

#include "Platform/Platform.hpp"

#include <vector>
#include <string>
#include <bitset>

namespace Charra
{
	VulkanContext::VulkanContext()
	{
		createInstance();
		initializeDebugMessenger(m_sharedData.instance, &m_sharedData.debugMessenger);
		createDevice();

		m_sharedData.mainWindow = std::make_unique<VulkanWindow>();
		createRenderPass();
		m_sharedData.mainWindow->createFramebuffers();
	}

	VulkanContext::~VulkanContext()
	{
		vkDestroyDevice(m_sharedData.device, NULL);
		terminateDebugMessenger(m_sharedData.instance, m_sharedData.debugMessenger, NULL);
		vkDestroyInstance(m_sharedData.instance, NULL);
	}

	void VulkanContext::createWindow()
	{
		m_sharedData.secondaryWindows.emplace_back();
		m_sharedData.secondaryWindows.back().createFramebuffers();
	}

	void VulkanContext::createInstance()
	{
		VkApplicationInfo applicationInfo{};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pNext;
		applicationInfo.pApplicationName = "Charra Engine";
		applicationInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
		applicationInfo.pEngineName = "Charra Engine";
		applicationInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
		applicationInfo.apiVersion = VK_API_VERSION_1_2;

		uint32_t instanceLayerCount;
		vkEnumerateInstanceLayerProperties(&instanceLayerCount, NULL);
		std::vector<VkLayerProperties> layers(instanceLayerCount);
		vkEnumerateInstanceLayerProperties(&instanceLayerCount, layers.data());

		const int numOfLayers = 1;
		const char* requiredLayers[numOfLayers] = {"VK_LAYER_KHRONOS_validation"};

		bool found = false;
		auto layerIt = layers.begin();
		for(layerIt; layerIt < layers.end(); layerIt++)
		{
			for(int i = 0; i < numOfLayers; i++)
			{
				if(strcmp(layerIt->layerName, requiredLayers[i]));
				{
					found = true;
					break;
				}
			}
		}
		CHARRA_LOG_ERROR(!found, "Vulkan extensions not found")

		std::vector<const char*> extensions = Platform::getExtensions();

		VkInstanceCreateInfo instanceCreateInfo{};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pNext;
		instanceCreateInfo.flags;
		instanceCreateInfo.pApplicationInfo = &applicationInfo;
		instanceCreateInfo.enabledLayerCount = numOfLayers;
		instanceCreateInfo.ppEnabledLayerNames = requiredLayers;
		instanceCreateInfo.enabledExtensionCount = extensions.size();
		instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

		vkCreateInstance(&instanceCreateInfo, NULL, &m_sharedData.instance);
	}

	void VulkanContext::createDevice()
	{
		// Selecting physical device
		uint32_t deviceCount;
		vkEnumeratePhysicalDevices(m_sharedData.instance, &deviceCount, NULL);
		CHARRA_LOG_ERROR(deviceCount == 0, "No suitable gpu was found");
		std::vector<VkPhysicalDevice> properties(deviceCount);
		vkEnumeratePhysicalDevices(m_sharedData.instance, &deviceCount, properties.data());

		auto physicalDevice = properties.begin();
		uint32_t bestPoints = 0;
		VkPhysicalDevice bestDevice = VK_NULL_HANDLE;
		for(physicalDevice; physicalDevice < properties.end(); physicalDevice++)
		{
			uint32_t points = 0;
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(*physicalDevice, &deviceProperties);

			switch (deviceProperties.deviceType)
			{
				case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
					points += 100;
				break;
				case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
					points += 1000;
				break;
				default:
					continue;
			}
			if(points > bestPoints)
			{
				bestPoints = points;
				bestDevice = *physicalDevice;
			}
		}

		CHARRA_LOG_ERROR(!bestPoints, "No suitable vulkan device was found");
		CHARRA_LOG_WARNING(bestPoints < 1000, "Possibly unsuitable gpu was selected");
		m_sharedData.physicalDevice = bestDevice;


		// Queue selection adn creation
		uint32_t propertyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(m_sharedData.physicalDevice, &propertyCount, NULL);
		std::vector<VkQueueFamilyProperties> queueProperties(propertyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_sharedData.physicalDevice, &propertyCount, queueProperties.data());
		CHARRA_LOG_ERROR(propertyCount == 0, "Vulkan could not get valid queues");

		uint32_t bestGraphicsQueueFamily = UINT32_MAX;
		uint32_t bestComputeQueuefamily = UINT32_MAX;
		uint32_t bestTransferQueueFamily = UINT32_MAX;
		uint32_t bestTransferBitCount = UINT32_MAX;
		for(int i = 0; i < queueProperties.size(); i++)
		{
			// I want graphics to be on the lowest queue family, 
			// Transfer to be on an dedicated queue family,
			// Compute to be separate if possible
			if((queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && (bestGraphicsQueueFamily == UINT32_MAX))
			{
				bestGraphicsQueueFamily = i;
				if(queueProperties[i].queueCount == 1)
				{
					continue;
				}
			}

			if((queueProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) && (std::bitset<sizeof(uint32_t)>(queueProperties[i].queueFlags).count() < bestTransferBitCount))
			{
				bestTransferBitCount = std::bitset<sizeof(uint32_t)>(queueProperties[i].queueFlags).count();
				bestTransferQueueFamily = i;
			}

			if(queueProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				bestComputeQueuefamily = i;
				m_sharedData.computeSupported = true;
			}
		}

		CHARRA_LOG_ERROR(bestGraphicsQueueFamily == UINT32_MAX, "Vulkan could not find a suitable graphics queue");
		CHARRA_LOG_ERROR(bestTransferBitCount == UINT32_MAX, "Vulkan could not find a suitable transfer queue");
		CHARRA_LOG_WARNING(bestComputeQueuefamily == UINT32_MAX, "Vulkan could not find a suitable compute queue");


		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.pNext;
		queueCreateInfo.flags;
		queueCreateInfo.queueCount = 1;
		const float priority = 1.0f;
		queueCreateInfo.pQueuePriorities = &priority;

		// The queues only differ by one parameter
		queueCreateInfo.queueFamilyIndex = bestGraphicsQueueFamily;
		queueCreateInfos.push_back(queueCreateInfo);
		queueCreateInfo.queueFamilyIndex = bestTransferQueueFamily;
		queueCreateInfos.push_back(queueCreateInfo);
		if(m_sharedData.computeSupported)
		{
			queueCreateInfo.queueFamilyIndex = bestComputeQueuefamily;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pNext;
		deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.enabledLayerCount;
		deviceCreateInfo.ppEnabledLayerNames;
		std::vector<const char*> extensions = { "VK_KHR_swapchain" };
		deviceCreateInfo.enabledExtensionCount = extensions.size();
		deviceCreateInfo.ppEnabledExtensionNames = extensions.data();
		deviceCreateInfo.pEnabledFeatures;

		vkCreateDevice(m_sharedData.physicalDevice, &deviceCreateInfo, NULL, &m_sharedData.device);

		vkGetDeviceQueue(m_sharedData.device, bestGraphicsQueueFamily, 0, &m_sharedData.graphicsQueue);
		m_sharedData.graphicsQueueFamily = bestGraphicsQueueFamily;
		vkGetDeviceQueue(m_sharedData.device, bestTransferQueueFamily, (bestGraphicsQueueFamily == bestTransferQueueFamily) ? 1 : 0, &m_sharedData.transferQueue);
		m_sharedData.transferQueueFamily = bestTransferQueueFamily;
		if(m_sharedData.computeSupported)
		{
			uint32_t index= 0;
			if(bestGraphicsQueueFamily == bestComputeQueuefamily)
			{
				index++;
			}
			if(bestGraphicsQueueFamily == bestTransferQueueFamily)
			{
				index++;
			}

			vkGetDeviceQueue(m_sharedData.device, bestComputeQueuefamily, index, &m_sharedData.computeQueue);
			m_sharedData.computeQueueFamily = bestComputeQueuefamily;
		}
	}

	void VulkanContext::createRenderPass()
	{
		VkAttachmentDescription attachmentDescription{};
		attachmentDescription.flags;
		attachmentDescription.format = m_sharedData.format;
		attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT ;
		attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // TODO research this
		attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference attachmentReference{};
		attachmentReference.attachment = 0;
		attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription{};
		subpassDescription.flags;
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.inputAttachmentCount;
		subpassDescription.pInputAttachments;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &attachmentReference;
		subpassDescription.pResolveAttachments;
		subpassDescription.pDepthStencilAttachment;
		subpassDescription.preserveAttachmentCount;
		subpassDescription.pPreserveAttachments;

		VkRenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.pNext;
		renderPassCreateInfo.flags;
		renderPassCreateInfo.attachmentCount = 1;
		renderPassCreateInfo.pAttachments = &attachmentDescription;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpassDescription;
		renderPassCreateInfo.dependencyCount;
		renderPassCreateInfo.pDependencies;

		vkCreateRenderPass(m_sharedData.device, &renderPassCreateInfo, NULL, &m_sharedData.renderPass);
	}
}