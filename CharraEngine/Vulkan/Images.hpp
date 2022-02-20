#pragma once

#include "vulkan/vulkan.h"

#include "Device.hpp"
#include "Swapchain.hpp"
#include "Renderpass.hpp"

namespace Charra
{
	class Images
	{
	public:
		Images(Device* deviceRef, Swapchain* swapchainRef, Renderpass* renderpassRef);
		~Images();

		inline uint32_t getImageCount() { return m_imageCount; }
		inline std::vector<VkImage>* getImages() { return &m_images; }
		inline std::vector<VkImageView>* getImageViews() { return &m_imageViews; }
		inline std::vector<VkFramebuffer>* getFramebuffers() { return &m_framebuffers; }

		void recreate();

	private: // Methods
		void createImages();
		void createFramebuffers();
		void destroy();

	private: // Members
		Device* m_deviceRef;
		Swapchain* m_swapchainRef;
		Renderpass* m_renderpassRef;

		uint32_t m_imageCount;

		std::vector<VkImage> m_images;
		std::vector<VkImageView> m_imageViews;
		std::vector<VkFramebuffer> m_framebuffers;
	};
}