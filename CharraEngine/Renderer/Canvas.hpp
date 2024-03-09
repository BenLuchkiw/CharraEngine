#pragma once

#include <vector>
#include <array>

#include "Math/RendererTypes.hpp"
#include "Vulkan/Allocator/BufferManager.hpp"
#include "Math/RendererTypes.hpp"

namespace Charra
{
	/*
		This class should manage all drawing operations
		
		Users of this class will be users and window manager

		Needs abiliity to record to command buffer, with expectation that
		recording began before calling function and will close after,
		this is because renderer::draw will iterate and record each canvas to the same buffer

		Does not need to know anything about window other than the dimensions,
		the best way to do this would probably be a simple change dimension function,
		if other parts of the program need this information they will need to ask canvas
		for dimensions, callbacks are unnecessary at this time

		Needs to make, store, and manage all vertex and index information in vertex buffers, 
		needs to be able to submit buffers on request for transfers

		Might want the ability to optimize buffers, future consideration, 
		would be a big task


	*/

	typedef uint32_t CanvasID;
	class Canvas
	{
	public:
		Canvas();
		~Canvas();

		void drawQuad(std::array<Vertex, 4>& vertices);
		CanvasID getCanvasID() { return m_ID; }

	protected: // Methods
		friend class WindowManager;
		// This will queue any required transfers
		void transferBuffers(BufferManager* bufferManager);

	protected:
		friend class Window;
		// This will record the draw commands to the command buffer
		void bindBuffers(BufferManager& bufferManager, VkCommandBuffer commandBuffer);

	private: // Methods

	private: // Members
		friend class Window;

		CanvasID m_ID;
		bool m_GPU = false;

		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_indices;

		BufferID m_vertexStagingBuffer = -1;
		BufferID m_vertexDeviceBuffer = -1;
		BufferID m_indexStagingBuffer = -1;
		BufferID m_indexDeviceBuffer = -1;
	};

}

