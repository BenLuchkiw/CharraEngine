#pragma once

#include "vulkan/vulkan.h"

#include "Device.hpp"
#include "Instance.hpp"
#include "Syncronization.hpp"

#include "Core/Events.hpp"

namespace Charra
{
	class Swapchain
	{
	public:
		Swapchain(Device* deviceRef, Instance* instanceRef);
		~Swapchain();

		void recreateSwapchain(VkExtent2D extent);

		// Pixel dimensions
		inline VkExtent2D getPixelExtent() { return m_pixelExtent; }

		// For use by the owning window class only
		inline void setPixelExtent(int width, int height) { m_pixelExtent = VkExtent2D(width, height); };

		inline VkFormat getSurfaceFormat() { return m_surfaceFormat.format; }

		inline VkSwapchainKHR* getSwapchain() { return &m_swapchain; }
		inline void prepareNextImage(Semaphore* waitSemaphore) { vkAcquireNextImageKHR(m_deviceRef->getDevice(), m_swapchain, UINT32_MAX, *waitSemaphore->getSemaphore(), VK_NULL_HANDLE, &m_imageIndex); }
		inline uint32_t getImageIndex() { return m_imageIndex; }

		static bool resizeCallback(EventType type, InputCode code, uint64_t data, void* privateData);

		// This function will automatically change internal state,
		// this means that a result of true must immediately be dealt with
		inline bool shouldResize() { return m_resized; m_resized = false; }
	private: // Methods

		void onInit();
		void createSwapchain();


	private: // Members
		Device* m_deviceRef;
		Instance* m_instanceRef;

		VkSurfaceKHR m_surface;
		VkSurfaceCapabilitiesKHR m_surfaceCapabilites;
		VkSurfaceFormatKHR m_surfaceFormat;
		VkExtent2D m_pixelExtent; // Pixel dimensions

		VkPresentModeKHR m_presentMode;
		VkSwapchainKHR m_swapchain;

		uint32_t m_imageCount = 0;
		uint32_t m_imageIndex = 0;

		bool m_resized = false;
	};
}