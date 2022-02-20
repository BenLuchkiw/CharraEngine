#include "Images.hpp"

#include "Core/Logging.hpp"

namespace Charra
{
	Images::Images(Device* deviceRef, Swapchain* swapchainRef, Renderpass* renderpassRef)
		: m_deviceRef(deviceRef),
		m_swapchainRef(swapchainRef),
		m_renderpassRef(renderpassRef),
		m_imageCount(0),
		m_images(NULL),
		m_imageViews(NULL),
		m_framebuffers(NULL)
	{
		createImages();
		createFramebuffers();
	}

	Images::~Images()
	{
		destroy();
	}

	void Images::recreate()
	{
		destroy();
		createImages();
		createFramebuffers();
	}

	void Images::createImages()
	{
		vkGetSwapchainImagesKHR(m_deviceRef->getDevice(), *m_swapchainRef->getSwapchain(), &m_imageCount, NULL);
		CHARRA_LOG_ERROR(m_imageCount == 0, "No swapchain images were available");
		m_images.resize(m_imageCount);
		vkGetSwapchainImagesKHR(m_deviceRef->getDevice(), *m_swapchainRef->getSwapchain(), &m_imageCount, m_images.data());
		
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.pNext;
		createInfo.flags;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = m_swapchainRef->getSurfaceFormat();
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
			CHARRA_LOG_ERROR(vkCreateImageView(m_deviceRef->getDevice(), &createInfo, NULL, &m_imageViews[i]) != VK_SUCCESS, "Vulkan failed to create image view");
		}
	}

	void Images::createFramebuffers()
	{
		m_framebuffers.resize(m_imageCount);

		VkFramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.pNext;
		framebufferCreateInfo.flags;
		framebufferCreateInfo.renderPass = *m_renderpassRef->getRenderPass();
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments;
		framebufferCreateInfo.width = m_swapchainRef->getPixelExtent().width;
		framebufferCreateInfo.height = m_swapchainRef->getPixelExtent().height;
		framebufferCreateInfo.layers = 1;

		for (int i = 0; i < m_imageCount; i++)
		{
			const VkImageView attachment[] = { m_imageViews.at(i) };
			framebufferCreateInfo.pAttachments = attachment;
			CHARRA_LOG_ERROR(VK_SUCCESS != vkCreateFramebuffer(m_deviceRef->getDevice(), &framebufferCreateInfo, NULL, &m_framebuffers[i]), "Vulkan could not make a framebuffer");
		}
	}

	void Images::destroy()
	{
		for (int i = 0; i < m_imageCount; i++)
		{
			vkDestroyFramebuffer(m_deviceRef->getDevice(), m_framebuffers[i], NULL);
			vkDestroyImageView(m_deviceRef->getDevice(), m_imageViews[i], NULL);
		}
	}
}