#pragma once

#include <array>
#include <memory>
#include <string>

#include "Core/Events.hpp"
#include "GUI/GUI.hpp"
#include "GUI/Square.hpp"
#include "Math/MathTypes.hpp"
#include "Renderer/Window.hpp"
#include "Vulkan/Allocator/Allocator.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/Instance.hpp"
#include "Vulkan/Syncronization.hpp"

namespace Charra
{
	struct RendererImplData;
	class Renderer
	{
	public: 
		Renderer(Events* eventHandler);
		~Renderer();

		// All render commands will be tied to a window id
		void draw(std::vector<Window>& windows);

		void drawQuad(fVec3 pos, fVec2 size, fVec4 colour);

	protected:
		friend class Charra::Window;
		Device& getDevice() { return m_device; }
		Instance& getInstance() { return m_instance; }
		Events* getEventHandler() { return m_eventHandlerRef; }

	private: // Methods

	private: // Members
		Events* m_eventHandlerRef;
		
		// Vulkan backend
		Instance m_instance;
		Device m_device;
		CommandBuffers m_commandBuffers;
		uint32_t m_commandBufferIndex = 0;
		Allocator m_allocator;

		Semaphore m_transferFinishedSemaphore;
		Semaphore m_renderFinishedSemaphore;
		Fence m_renderFinishedFence;

		// TODO get this out of this class
		Buffer m_vertexStagingBuffer{};
		Buffer m_vertexDeviceBuffer{};
		Buffer m_indexStagingBuffer{};
		Buffer m_indexDeviceBuffer{};
		bool m_shouldTransfer = false;

		std::vector<GUI_Square> m_squares = {};
	};
}