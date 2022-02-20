#include "Swapchain.hpp"

#include "Core/Logging.hpp"
#include "Platform/Platform.hpp"

namespace Charra
{
	Swapchain::Swapchain(Device* deviceRef, Instance* instanceRef)
		: m_deviceRef(deviceRef),
		m_instanceRef(instanceRef),
		m_surfaceCapabilites(),
		m_surfaceFormat(),
		m_pixelExtent({ 0, 0 }),
		m_presentMode(),
		m_swapchain(VK_NULL_HANDLE)
	{
		onInit();
		createSwapchain();
	}

	Swapchain::~Swapchain()
	{
		vkDestroySwapchainKHR(m_deviceRef->getDevice(), m_swapchain, NULL);
		vkDestroySurfaceKHR(m_instanceRef->getInstance(), m_surface, NULL);
	}

	void Swapchain::recreateSwapchain(VkExtent2D extent)
	{
		m_pixelExtent = extent;
		m_resized = true;
		createSwapchain();
	}

	void Swapchain::onInit()
	{
		m_surface = Platform::getSurface(m_instanceRef->getInstance());
		// Find out if the physical device supports vulkan swapchains
		VkBool32 supported;
		vkGetPhysicalDeviceSurfaceSupportKHR(m_deviceRef->getPhysicalDevice(), m_deviceRef->getGraphicsQueueIndex(), m_surface, &supported);
		CHARRA_LOG_ERROR(!supported, "The selected graphics device does not have surface support");

		// Find the surface capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_deviceRef->getPhysicalDevice(), m_surface, &m_surfaceCapabilites);

		// Decide image count
		if (m_surfaceCapabilites.maxImageCount > 0 && ((m_surfaceCapabilites.minImageCount + 1) <= m_surfaceCapabilites.maxImageCount))
		{
			m_imageCount = m_surfaceCapabilites.minImageCount + 1;
		}
		else
		{
			m_imageCount = m_surfaceCapabilites.maxImageCount;
			CHARRA_LOG_WARNING(1, "The selected GPU may not be adequate, it only supports minimal images");
		}

		// Select the image format

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_deviceRef->getPhysicalDevice(), m_surface, &formatCount, NULL);
		std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_deviceRef->getPhysicalDevice(), m_surface, &formatCount, surfaceFormats.data());

		m_surfaceFormat = surfaceFormats[0]; // Add a default surface format
		for (const auto& format : surfaceFormats)
		{
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				m_surfaceFormat = format;
			}
		}

		// Select my extent, TODO make this fancy apple monitor compatible
		m_pixelExtent = m_surfaceCapabilites.currentExtent;
		


		// Select the present mode

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_deviceRef->getPhysicalDevice(), m_surface, &presentModeCount, NULL);
		std::vector<VkPresentModeKHR> presentModes(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_deviceRef->getPhysicalDevice(), m_surface, &presentModeCount, presentModes.data());

		m_presentMode = VK_PRESENT_MODE_FIFO_KHR;
		for (const auto& presentMode : presentModes)
		{
			if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				m_presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			}
		}
	}

	void Swapchain::createSwapchain()
	{

		// Create the swapchain
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.pNext;
		createInfo.flags;
		createInfo.surface = m_surface;
		createInfo.minImageCount = m_imageCount;
		createInfo.imageFormat = m_surfaceFormat.format;
		createInfo.imageColorSpace = m_surfaceFormat.colorSpace;
		createInfo.imageExtent = m_pixelExtent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = NULL;
		createInfo.preTransform = m_surfaceCapabilites.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = m_presentMode;
		createInfo.clipped = VK_TRUE; // #TODO this needs more research, and may end up needing to be changed for simulations
		createInfo.oldSwapchain = (m_swapchain == VK_NULL_HANDLE) ? NULL : m_swapchain; // #Resizing will need this

		vkCreateSwapchainKHR(m_deviceRef->getDevice(), &createInfo, NULL, &m_swapchain);
	}

	bool Swapchain::resizeCallback(EventType type, InputCode code, uint64_t data, void* privateData)
	{
		Vec2 size;
		size.width = UPPER_UINT64(data);
		size.height = LOWER_UINT64(data);

		Swapchain* swapchainRef = static_cast<Swapchain*>(privateData);

		VkExtent2D currentExtent = swapchainRef->getPixelExtent();
		if(currentExtent.width != size.width || currentExtent.height != size.height)
		{
			swapchainRef->recreateSwapchain(VkExtent2D(size.width, size.height));
		}
		
		return true;
	}
};