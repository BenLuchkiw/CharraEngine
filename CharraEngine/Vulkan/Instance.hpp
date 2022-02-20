#pragma once

#include "vulkan/vulkan.h"

#include "ValidationLayers.hpp"

namespace Charra
{
	class Instance
	{
	public:
		Instance();
		~Instance();

		VkInstance getInstance() { return m_instance; }

	private: // Methods
		void createInstance();
	private: // Members
		VkInstance m_instance;
		VkDebugUtilsMessengerEXT m_debugMessenger;
	};
}