#include "BufferManager.hpp"

namespace Charra
{

	BufferManager::BufferManager(Charra::Device &device)
	: m_allocator(device)
	{
		
	}

	BufferManager::~BufferManager()
	{
	}

	int BufferManager::createBuffer()
	{
		// Change buffer for in use flag then check if any of those already exist
		// before pushing back any more
		Charra::Buffer buffer;

		for(auto& b : m_buffers)
		{
			if(!b.inUse)
			{
				buffer = b;
				b.inUse = true;
				return &b - &m_buffers[0];
			}
		}

		buffer.inUse = true;
		m_buffers.push_back(buffer);
		return m_buffers.size() - 1;
	}

	void BufferManager::deleteBuffer(int buffer)
	{
		// Zero out the buffer and set inUse to false
		m_buffers[buffer].zeroBuffer();
		m_buffers[buffer].inUse = false;
	}

	int BufferManager::allocateBuffer(int buffer, uint64_t size, Charra::BufferTypeFlags type)
	{
		// TODO: check for failure to allocate and return nonzero
		m_buffers[buffer] = m_allocator.allocateBuffer(size, type);
		m_buffers[buffer].inUse = true;
		return 0; 
	}

	void BufferManager::deallocateBuffer(int buffer)
	{
		// If buffer has not been allocated this function will do nothing
		if(m_buffers[buffer].buffer != VK_NULL_HANDLE)
		{
			m_allocator.deallocateBuffer(&m_buffers[buffer]);
		}
	}

	void BufferManager::submitData(int buffer, void* data, size_t bytes, size_t offset)
	{
		m_allocator.submitData(m_buffers[buffer], data, bytes, offset);
	}

	void BufferManager::queueTransfer(int src, int dst)
	{
		m_allocator.applyForTransfer(&m_buffers[src], &m_buffers[dst]);
	}
}