#include "Instance.hpp"

#include "ValidationLayers.hpp"

#include "Core/Logging.hpp"

namespace Charra
{
	Instance::Instance()
		: m_instance(NULL)
	{
		createInstance();
		initializeDebugMessenger(m_instance, &m_debugMessenger);
	}

	Instance::~Instance()
	{
		terminateDebugMessenger(m_instance, m_debugMessenger, NULL);
		vkDestroyInstance(m_instance, NULL);
	}

	void Instance::createInstance()
	{
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "First Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "First Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.pApplicationInfo = &appInfo;
		auto extensions = getRequiredExtensions(); // gets all glfw extensions
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
		createInfo.enabledLayerCount = 0;


		const char* layerNames = "VK_LAYER_KHRONOS_validation";
		if (initValidationLayers())
		{
			createInfo.enabledLayerCount = 1;
			createInfo.ppEnabledLayerNames = &layerNames;
		}

		CHARRA_LOG_ERROR(vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS, "Failed to create Vulkan instance");
	}
}