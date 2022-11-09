#pragma once

#include <string>

#include "Material.hpp"
#include "Math/MathTypes.hpp"
#include "Vulkan/Renderpass.hpp"
#include "Vulkan/Swapchain.hpp"
#include "Vulkan/Syncronization.hpp"

namespace Charra
{

	class Renderer;
	class Window;

	class WindowDummy // Platform needs to make window before swapchain is created
	{
	public:
		WindowDummy(iVec2 size, iVec2 position, const std::string& name, Window* window);
	};

	class Window
	{
	public:
		Window(iVec2 size, iVec2 position, const std::string& name, Renderer* rendererRef);
		~Window();

		iVec2&			getWindowSize() { return m_windowSize; }
		std::string& 	getWindowName() { return m_windowName; }
		Swapchain& 		getSwapchain() 	{ return m_swapchain; }
		Renderpass& 	getRenderpass() { return m_renderpass; }
		Semaphore& 		getSemaphore() 	{ return m_imageWaitSemaphore; }
		Material&		getMaterial()	{ return m_material; }
		Mat4X4&			getOrthoMatrix(){ return m_orthographicMatrix; }

		static bool resizeCallback(EventType type, InputCode code, uint64_t data, void* privateData);
	private: // Methods

	private: // Members
	friend class WindowDummy;
		WindowDummy m_dummy;
		Renderer* m_rendererRef;

		iVec2 		m_windowSize;
		std::string m_windowName;
		Swapchain 	m_swapchain;
		Renderpass	m_renderpass;
		Semaphore 	m_imageWaitSemaphore;
		Material 	m_material;

		Mat4X4 m_orthographicMatrix = {};

		uint32_t m_windowID;
	};
}