#pragma once

#include "Vulkan/Allocator/Buffer.hpp"
#include "Vulkan/Allocator/Allocator.hpp"

#include <vector>


namespace Charra
{

    typedef uint32_t BufferID;

    class Device;
    class BufferManager
    {
    public:
        BufferManager(Charra::Device &deviceRef);
        ~BufferManager();

        // Creates a zeroed out buffer, needs to be allocated before use, returns buffer id
        BufferID createBuffer();
        // Deletes buffer
        void deleteBuffer(BufferID buffer);


        // Allocates data and initializes the buffer
        // Returns non-zero on error
        void allocateBuffer(BufferID buffer, uint64_t size, Charra::BufferTypeFlags type);

        // Deallocates the buffer
        void deallocateBuffer(BufferID buffer);

        // Submits data to the buffer
        void submitData(BufferID buffer, void* data, size_t bytes, size_t offset);

        // Queues the buffer for transfer
        void queueTransfer(BufferID src, BufferID dst);

        // Passes buffer types down the line from allocator
        inline Charra::BufferTypeFlags getBufferTypes() { return m_allocator.getBufferTypes(); }
        // Returns the transfer buffer from the allocator
        inline VkCommandBuffer getTransferBuffer() { return m_allocator.getTransferBuffer(); }

        // Passes pointer to buffer  
        Charra::Buffer* getBuffer(BufferID buffer) { return &m_buffers[buffer]; }
    private:

        Charra::Allocator m_allocator;

        // The following vector will store buffers, since the buffer is a simple struct it can 
        // simply be zeroed for deletion, and new buffers can search for zeroed buffers,
        // search is not a concern since the user of the class will pass indexes
        std::vector<Charra::Buffer> m_buffers;
    };


} // namespace CharraEngine