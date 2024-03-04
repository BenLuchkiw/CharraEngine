#pragma once

#include <array>
#include <memory>
#include <string>

#include "Core/Events.hpp"
#include "Draw2D/Quad.hpp"
#include "Math/MathTypes.hpp"
#include "Renderer/Window.hpp"
#include "Vulkan/Allocator/BufferManager.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/Instance.hpp"
#include "Vulkan/Syncronization.hpp"

namespace Charra
{
	struct RendererImplData;
	class Renderer
	{
	public: 
		Renderer(Events* eventHandler, WindowManager* windowManagerRef);
		~Renderer();

		// All render commands will be tied to a window id
		void draw();

	protected:
		friend class Charra::Window;
		friend class Charra::WindowManager;

		Device& getDevice() { return m_device; }
		Instance& getInstance() { return m_instance; }
		Events* getEventHandler() { return m_eventHandlerRef; }
		CommandBuffers& getCommandBuffers() { return m_commandBuffers; }
		BufferManager& getBufferManager() { return m_bufferManager; }

	private: // Methods

		void transferBuffers(std::vector<VkSemaphore>* semaphore, std::vector<VkPipelineStageFlags>* waitStages);
		void recordCommandBuffers(std::vector<VkSemaphore>* renderSemaphore, std::vector<VkSemaphore>* presentSemaphore, std::vector<VkPipelineStageFlags>* waitStages);
		void submitCommandBuffers();
		void presentImages();


	private: // Members
		Events* m_eventHandlerRef;
		WindowManager* m_windowManagerRef;

		// Vulkan backend
		Instance m_instance;
		Device m_device;
		CommandBuffers m_commandBuffers;
		uint32_t m_commandBufferIndex = 0;
		BufferManager m_bufferManager;

		Semaphore m_transferFinishedSemaphore;
		Fence m_renderFinishedFence;
	};
}