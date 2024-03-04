#include "Canvas.hpp"

namespace Charra
{
	Canvas::Canvas()
	{
	}

	Canvas::~Canvas()
	{
	}

	void Canvas::drawQuad(fVec3 position, fVec2 size, fVec4 colour)
	{
		Vertex v1 = {{position.x, position.y, position.z}, {colour.x, colour.y, colour.z, colour.w}};
		Vertex v2 = {{position.x, position.y + size.y, position.z}, {colour.x, colour.y, colour.z, colour.w}};
		Vertex v3 = {{position.x + size.x, position.y + size.y, position.z}, {colour.x, colour.y, colour.z, colour.w}};
		Vertex v4 = {{position.x + size.x, position.y, position.z}, {colour.x, colour.y, colour.z, colour.w}};

		m_vertices.push_back(v1);
		m_vertices.push_back(v2);
		m_vertices.push_back(v3);
		m_vertices.push_back(v4);

		uint32_t indexOffset = m_indices.size();
		m_indices.push_back(indexOffset);
		m_indices.push_back(indexOffset + 1);
		m_indices.push_back(indexOffset + 2);
		m_indices.push_back(indexOffset + 2);
		m_indices.push_back(indexOffset + 3);
		m_indices.push_back(indexOffset);
	}

	void Canvas::transferBuffers(BufferManager* bufferManager)
	{
		if(m_vertices.size() == 0 || m_indices.size() == 0)
		{
			return;
		}

		if(m_vertexStagingBuffer == -1)
		{
			m_vertexStagingBuffer = bufferManager->createBuffer();
		}
		if(m_vertexDeviceBuffer == -1)
		{
			m_vertexDeviceBuffer = bufferManager->createBuffer();
		}
		if(m_indexStagingBuffer == -1)
		{
			m_indexStagingBuffer = bufferManager->createBuffer();
		}
		if(m_indexDeviceBuffer == -1)
		{
			m_indexDeviceBuffer = bufferManager->createBuffer();
		}

		// Deallocate all data
		bufferManager->deallocateBuffer(m_vertexStagingBuffer);
		bufferManager->deallocateBuffer(m_vertexDeviceBuffer);
		bufferManager->deallocateBuffer(m_indexStagingBuffer);
		bufferManager->deallocateBuffer(m_indexDeviceBuffer);

		bufferManager->allocateBuffer(m_vertexStagingBuffer, m_vertices.size() * sizeof(Vertex), BufferType::CPU);
		bufferManager->allocateBuffer(m_indexStagingBuffer, m_indices.size() * sizeof(uint32_t), BufferType::CPU);

		bufferManager->submitData(m_vertexStagingBuffer, m_vertices.data(), m_vertices.size() * sizeof(Vertex), 0);
		bufferManager->submitData(m_indexStagingBuffer, m_indices.data(), m_indices.size() * sizeof(uint32_t), 0);

		// Check for gpu, then transfer if needed
		auto flags = bufferManager->getBufferTypes();
		if(flags & BufferType::GPU)
		{
			m_GPU = true;

			bufferManager->allocateBuffer(m_vertexDeviceBuffer, m_vertices.size() * sizeof(Vertex), BufferType::GPU);
			bufferManager->queueTransfer(m_vertexStagingBuffer, m_vertexDeviceBuffer);

			bufferManager->allocateBuffer(m_indexDeviceBuffer, m_indices.size() * sizeof(uint32_t), BufferType::GPU);
			bufferManager->queueTransfer(m_indexStagingBuffer, m_indexDeviceBuffer);
		}


		// TODO only temporary
		m_vertices.clear();
		m_indices.clear();
	}

	void Canvas::bindBuffers(BufferManager& bufferManager, VkCommandBuffer commandBuffer)
	{
		if(m_GPU)
		{
			Buffer* buffer = bufferManager.getBuffer(m_vertexDeviceBuffer);
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, &buffer->buffer, &buffer->offset);

			buffer = bufferManager.getBuffer(m_indexDeviceBuffer);
			vkCmdBindIndexBuffer(commandBuffer, buffer->buffer, buffer->offset, VK_INDEX_TYPE_UINT32);
		}
		else if(m_indexDeviceBuffer != -1 && m_vertexDeviceBuffer != -1)
		{
			Buffer* buffer = bufferManager.getBuffer(m_vertexStagingBuffer);
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, &buffer->buffer, &buffer->offset);

			buffer = bufferManager.getBuffer(m_indexStagingBuffer);
			vkCmdBindIndexBuffer(commandBuffer, buffer->buffer, buffer->offset, VK_INDEX_TYPE_UINT32);
		}

		vkCmdDrawIndexed(commandBuffer, sizeof(uint32_t) * m_indices.size(), 1, 0, 0, 0);
	}
}