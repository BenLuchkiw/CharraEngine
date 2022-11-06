#pragma once

#include "vulkan/vulkan.h"

#include "Device.hpp"
#include "Instance.hpp"
#include "Renderpass.hpp"
#include "Syncronization.hpp"

#include "Core/Events.hpp"

namespace Charra
{
	class Renderpass;
	class Swapchain
	{
	public:
		Swapchain(Device& deviceRef, Instance& instanceRef);
		~Swapchain();

		void passRenderpass(Renderpass* renderpassRef) { m_renderpassRef = renderpassRef; }

		void recreateSwapchain();

		// Pixel dimensions
		inline VkExtent2D getPixelExtent() { return m_pixelExtent; }

		// For use by the owning window class only
		inline void setPixelExtent(int width, int height) { m_pixelExtent = VkExtent2D(width, height); };

		inline VkFormat getSurfaceFormat() { return m_surfaceFormat.format; }

		inline VkSwapchainKHR getSwapchain() { return m_swapchain; }
		void prepareNextImage(Semaphore* waitSemaphore);
		inline uint32_t getImageIndex() { return m_imageIndex; }

		inline void invalidateSwapchain() { m_resized = true; }

		inline VkFramebuffer getFramebuffer() { 
			return m_framebuffers[m_imageIndex]; }
	private: // Methods

		void onInit();
		void createSwapchain();
		void destroyImages();
		void createImages();

	private: // Members
		Device& m_deviceRef;
		Instance& m_instanceRef;
		Renderpass* m_renderpassRef = nullptr;

		VkSurfaceKHR m_surface;
		VkSurfaceCapabilitiesKHR m_surfaceCapabilites;
		VkSurfaceFormatKHR m_surfaceFormat;
		VkExtent2D m_pixelExtent; // Pixel dimensions

		VkPresentModeKHR m_presentMode;
		VkSwapchainKHR m_swapchain;

		uint32_t m_imageCount = 0;
		uint32_t m_imageIndex = 0;

		bool m_resized = false;
		bool m_framebufferInvalid = true;

		std::vector<VkImage> m_images;
		std::vector<VkImageView> m_imageViews;
		std::vector<VkFramebuffer> m_framebuffers;

		// Stuff that needs to be destroyed in the next frame
		std::vector<VkImage> m_defferedImages;
		std::vector<VkImageView> m_defferedImageViews;
		std::vector<VkFramebuffer> m_defferedFramebuffers;
	};
}