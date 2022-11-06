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
		Renderer(const std::string& mainWindowName, iVec2 windowSize, iVec2 windowPos, Events* eventHandler);
		~Renderer();

		void draw();

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

		std::vector<Window> m_windows;

		// TODO get this out of this class
		Buffer m_vertexStagingBuffer{};
		Buffer m_vertexDeviceBuffer{};
		Buffer m_indexStagingBuffer{};
		Buffer m_indexDeviceBuffer{};
		bool m_shouldTransfer = false;
		
		GUI_Square m_square;

		VkVertexInputBindingDescription m_vertAttribs{0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX};
		std::vector<VkVertexInputAttributeDescription> m_attribDesc = {
			{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)},
			{1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, colour)}
		};
	};
}