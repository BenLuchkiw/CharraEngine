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
		Renderer(Events* eventHandler);
		~Renderer();

		// All render commands will be tied to a window id
		void draw(std::vector<Window*>& windows);

		void drawQuad(fVec3 pos, fVec2 size, fVec4 colour, uint32_t windowID);

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
		BufferManager m_bufferManager;

		Semaphore m_transferFinishedSemaphore;
		Fence m_renderFinishedFence;


		int m_vertexStagingBuffer = -1;
		int m_vertexDeviceBuffer = -1;
		int m_indexStagingBuffer = -1;
		int m_indexDeviceBuffer = -1;


		bool m_shouldTransfer = false;

		std::vector<Quad> m_quads = {};
	};
}