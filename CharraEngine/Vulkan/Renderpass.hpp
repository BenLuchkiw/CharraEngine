#pragma once

#include "vulkan/vulkan.h"

#include "Device.hpp"
#include "Swapchain.hpp"

namespace Charra
{
	class Renderpass
	{
	public:
		Renderpass(Device* deviceRef, VkFormat format);
		~Renderpass();

		void createRenderpass();
		
		VkRenderPass getRenderPass() { return m_renderPass; }

	private: // Methods

	private: // Members
		Device* m_deviceRef;

		VkRenderPass m_renderPass;
		VkFormat m_format;
	};
}