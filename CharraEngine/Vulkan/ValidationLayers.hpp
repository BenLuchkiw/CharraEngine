#pragma once

#include "vulkan/vulkan.h"

#include <vector>

namespace Charra
{
	// If validation layers are not requested in cmake this function will not do anything
	// Uses VK_LAYER_KHRONOS_validation by default, if others are desired the function will need to be changed
	bool initValidationLayers();

	// From vulkan-tutorial, this function includes glfw extensions already
	std::vector<const char*> getRequiredExtensions();

	// From vulkan-tutorial
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	// If validation layers have not been requested in cmkae this function will not do anything
	void initializeDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT* debugMessenger);

	// From vulkan-tutorial
	void terminateDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
}