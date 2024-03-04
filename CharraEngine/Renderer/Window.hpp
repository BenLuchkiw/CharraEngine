#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "Material.hpp"
#include "Math/MathTypes.hpp"
#include "Vulkan/Renderpass.hpp"
#include "Vulkan/Swapchain.hpp"
#include "Vulkan/Syncronization.hpp"

/*

	User code for this class will utilize the window manager to create and destroy and manage windows
	Window objects will be encapsulated by the window manager

*/

namespace Charra
{

	class Renderer;
	class Canvas;
	
	
	class Window
	{
	public:
		Window(iVec2 size, iVec2 position, const std::string& name, Renderer* rendererRef, uint32_t windowID);
		~Window();

		iVec2&			getWindowSize() 		{ return m_windowSize; }
		std::string& 	getWindowName() 		{ return m_windowName; }
		Swapchain& 		getSwapchain() 			{ return m_swapchain; }
		Renderpass& 	getRenderpass() 		{ return m_renderpass; }
		Semaphore& 		getImageSemaphore() 	{ return m_imageWaitSemaphore; } // VkAcquireNextImage
		Semaphore& 		getRenderSemaphore() 	{ return m_renderFinishedSemaphore; } // rendering is finished
		Material&		getMaterial()			{ return m_material; }
		Mat4X4&			getOrthoMatrix()		{ return m_orthographicMatrix; }
		uint32_t 		getWindowID()   		{ return m_windowID; }

		static bool resizeCallback(EventType type, InputCode code, uint64_t data, void* privateData);

		void assignCanvas(Canvas* canvas);
		void disassociateCanvas(Canvas* canvas);
	protected: // Methods
		friend class WindowManager;
		void recordCommandBuffers(std::vector<VkSemaphore>* renderSemaphore, std::vector<VkSemaphore>* presentSemaphore, std::vector<VkPipelineStageFlags>* waitStages, uint32_t commandBufIndex);
		Semaphore& getRenderFinishedSemaphore() { return m_renderFinishedSemaphore; }


	private: // Members
		Renderer* 	m_rendererRef;
		iVec2 		m_windowSize;
		std::string m_windowName;
		Swapchain 	m_swapchain;
		Renderpass	m_renderpass;
		Semaphore 	m_imageWaitSemaphore;
		Semaphore   m_renderFinishedSemaphore;
		Material 	m_material;

		Mat4X4 m_orthographicMatrix = {};
		std::unordered_map<uint32_t, Canvas*> m_canvasMap;

		uint32_t m_windowID;
	};



/*
	This class will manage the relationship between windows and canvases

	The user needs to manage 
*/

	// typdef a canvas ID and window ID for managing classes

	typedef uint32_t CanvasID;
	typedef uint32_t WindowID;

	class BufferManager;

	class WindowManager
	{
	public:
		WindowManager(Renderer* rendererRef);
		~WindowManager();

		// Windows are managed via integer ID
		WindowID createWindow(iVec2 size, iVec2 position, const std::string& name);
		void destroyWindow(WindowID ID);

		// Returns canvas ID
		Canvas* createCanvas();
		void destroyCanvas(Canvas& canvas);

		void assignCanvasToWindow(Canvas& canvas, WindowID windowID);
		void disassociateCanvasFromWindow(Canvas& canvas, WindowID windowID);


		// TODO: Implement these
		// void resizeWindow();
		// void resizeCanvas();
	private: // Methods
	friend class Renderer;
		void transferBuffers(BufferManager* bufferManager);
		// This will call each window to draw record its canvasses
		// This will be called mid recording of the command buffer
		void recordCommandBuffers(std::vector<VkSemaphore>* renderSemaphore, std::vector<VkSemaphore>* presentSemaphore, std::vector<VkPipelineStageFlags>* waitStages, uint32_t commandBufIndex);
		void presentImages();
	private: // Members

		Renderer* m_rendererRef;

		std::unordered_map<WindowID, Window> m_windows;
		std::unordered_map<CanvasID, Canvas> m_canvases;

		uint32_t m_canvasIDGen = 0;

	};
}