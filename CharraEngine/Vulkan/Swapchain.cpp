#include "Swapchain.hpp"

#include "Core/Logging.hpp"
#include "Platform/Platform.hpp"

namespace Charra
{
	Swapchain::Swapchain(Device& deviceRef, Instance& instanceRef)
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
		destroyImages();
		for (int i = 0; i < m_framebuffers.size(); i++)
		{
			vkDestroyFramebuffer(m_deviceRef.getDevice(), m_framebuffers[i], NULL);
			vkDestroyImageView(m_deviceRef.getDevice(), m_imageViews[i], NULL);
		}

		vkDestroySwapchainKHR(m_deviceRef.getDevice(), m_swapchain, NULL);
		vkDestroySurfaceKHR(m_instanceRef.getInstance(), m_surface, NULL);
	}

	void Swapchain::onInit()
	{
		m_surface = Platform::getSurface(m_instanceRef.getInstance());
		// Find out if the physical device supports vulkan swapchains
		VkBool32 supported;
		vkGetPhysicalDeviceSurfaceSupportKHR(m_deviceRef.getPhysicalDevice(), m_deviceRef.getGraphicsQueueIndex(), m_surface, &supported);
		CHARRA_LOG_ERROR(!supported, "The selected graphics device does not have surface support");

		// Find the surface capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_deviceRef.getPhysicalDevice(), m_surface, &m_surfaceCapabilites);

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
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_deviceRef.getPhysicalDevice(), m_surface, &formatCount, NULL);
		std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_deviceRef.getPhysicalDevice(), m_surface, &formatCount, surfaceFormats.data());

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
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_deviceRef.getPhysicalDevice(), m_surface, &presentModeCount, NULL);
		std::vector<VkPresentModeKHR> presentModes(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_deviceRef.getPhysicalDevice(), m_surface, &presentModeCount, presentModes.data());

		m_presentMode = VK_PRESENT_MODE_FIFO_KHR;
		//for (const auto& presentMode : presentModes)
		//{
		//	if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		//	{
		//		m_presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		//	}
		//}
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
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = m_swapchain; // #Resizing will need this

		auto oldSwapchain = m_swapchain;

		vkCreateSwapchainKHR(m_deviceRef.getDevice(), &createInfo, NULL, &m_swapchain);

		if(oldSwapchain != VK_NULL_HANDLE)
		{	
			vkDestroySwapchainKHR(m_deviceRef.getDevice(), oldSwapchain, NULL);
		}
	}

	void Swapchain::prepareNextImage(Semaphore* waitSemaphore)
	{
		if(m_resized)
		{
			// Destroys the deffered framebuffers that are not in use
			destroyImages();

			m_defferedImages.clear();
			m_defferedImageViews.clear();
			m_defferedFramebuffers.clear();

			m_defferedImages.swap(m_images);
			m_defferedImageViews.swap(m_imageViews);
			m_defferedFramebuffers.swap(m_framebuffers);
			
			createSwapchain();
			createImages();
			m_resized = false;			
		}
		if(m_framebufferInvalid)
		{
			m_framebufferInvalid = false;
			createImages();
		}
		vkAcquireNextImageKHR(m_deviceRef.getDevice(), m_swapchain, UINT32_MAX, waitSemaphore->getSemaphore(), VK_NULL_HANDLE, &m_imageIndex);
	}

	void Swapchain::createImages()
	{
		vkGetSwapchainImagesKHR(m_deviceRef.getDevice(), m_swapchain, &m_imageCount, NULL);
		CHARRA_LOG_ERROR(m_imageCount == 0, "No swapchain images were available");
		m_images.resize(m_imageCount);
		vkGetSwapchainImagesKHR(m_deviceRef.getDevice(), m_swapchain, &m_imageCount, m_images.data());
		
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.pNext;
		createInfo.flags;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = m_surfaceFormat.format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.layerCount = 1;
		createInfo.subresourceRange.levelCount = 1;

		m_imageViews.resize(m_imageCount);
		for (int i = 0; i < m_imageCount; i++)
		{
			createInfo.image = m_images[i];
			CHARRA_LOG_ERROR(vkCreateImageView(m_deviceRef.getDevice(), &createInfo, NULL, &m_imageViews[i]) != VK_SUCCESS, "Vulkan failed to create image view");
		}

		m_framebuffers.resize(m_imageCount);

		VkFramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.pNext;
		framebufferCreateInfo.flags;
		framebufferCreateInfo.renderPass = m_renderpassRef->getRenderpass();
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments;
		framebufferCreateInfo.width = m_pixelExtent.width;
		framebufferCreateInfo.height = m_pixelExtent.height;
		framebufferCreateInfo.layers = 1;

		for (int i = 0; i < m_imageCount; i++)
		{
			const VkImageView attachment[] = { m_imageViews.at(i) };
			framebufferCreateInfo.pAttachments = attachment;
			CHARRA_LOG_ERROR(VK_SUCCESS != vkCreateFramebuffer(m_deviceRef.getDevice(), &framebufferCreateInfo, NULL, &m_framebuffers[i]), "Vulkan could not make a framebuffer");
		}

	}

	void Swapchain::destroyImages()
	{
		for (int i = 0; i < m_defferedFramebuffers.size(); i++)
		{
			vkDestroyFramebuffer(m_deviceRef.getDevice(), m_defferedFramebuffers[i], NULL);
			vkDestroyImageView(m_deviceRef.getDevice(), m_defferedImageViews[i], NULL);
		}
	}
};