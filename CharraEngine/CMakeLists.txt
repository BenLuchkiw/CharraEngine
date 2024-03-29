set(CHARRA_SOURCE

${CMAKE_CURRENT_LIST_DIR}/Core/Application.hpp
${CMAKE_CURRENT_LIST_DIR}/Core/Application.cpp
${CMAKE_CURRENT_LIST_DIR}/Core/Events.hpp
${CMAKE_CURRENT_LIST_DIR}/Core/Events.cpp
${CMAKE_CURRENT_LIST_DIR}/Core/Logging.hpp
${CMAKE_CURRENT_LIST_DIR}/Core/Logging.cpp

${CMAKE_CURRENT_LIST_DIR}/Draw2D/Quad.hpp
${CMAKE_CURRENT_LIST_DIR}/Draw2D/Quad.cpp

#${CMAKE_CURRENT_LIST_DIR}/GUI/Containers.hpp
#${CMAKE_CURRENT_LIST_DIR}/GUI/Containers.cpp
#${CMAKE_CURRENT_LIST_DIR}/GUI/GUI.hpp
#${CMAKE_CURRENT_LIST_DIR}/GUI/GUI.cpp
#${CMAKE_CURRENT_LIST_DIR}/GUI/Square.hpp
#${CMAKE_CURRENT_LIST_DIR}/GUI/Square.cpp

${CMAKE_CURRENT_LIST_DIR}/Renderer/Canvas.cpp
${CMAKE_CURRENT_LIST_DIR}/Renderer/Canvas.hpp
${CMAKE_CURRENT_LIST_DIR}/Renderer/Material.hpp
${CMAKE_CURRENT_LIST_DIR}/Renderer/Material.cpp
${CMAKE_CURRENT_LIST_DIR}/Renderer/Renderer.hpp
${CMAKE_CURRENT_LIST_DIR}/Renderer/Renderer.cpp
${CMAKE_CURRENT_LIST_DIR}/Renderer/Window.hpp
${CMAKE_CURRENT_LIST_DIR}/Renderer/Window.cpp

${CMAKE_CURRENT_LIST_DIR}/Vulkan/Allocator/Buffer.hpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/Allocator/Allocator.hpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/Allocator/Allocator.cpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/Allocator/DeviceAllocator.hpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/Allocator/DeviceAllocator.cpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/Allocator/HostAllocator.hpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/Allocator/HostAllocator.cpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/Allocator/BufferManager.hpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/Allocator/BufferManager.cpp

${CMAKE_CURRENT_LIST_DIR}/Vulkan/CommandBuffers.hpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/CommandBuffers.cpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/Device.hpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/Device.cpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/Instance.hpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/Instance.cpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/Pipeline.hpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/Pipeline.cpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/Renderpass.hpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/Renderpass.cpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/Shader.hpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/Shader.cpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/Swapchain.hpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/Swapchain.cpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/Syncronization.hpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/Syncronization.cpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/ValidationLayers.hpp
${CMAKE_CURRENT_LIST_DIR}/Vulkan/ValidationLayers.cpp

${CMAKE_CURRENT_LIST_DIR}/Math/MathTypes.hpp
${CMAKE_CURRENT_LIST_DIR}/Math/MathFunctions.hpp
${CMAKE_CURRENT_LIST_DIR}/Math/MathFunctions.cpp
${CMAKE_CURRENT_LIST_DIR}/Math/RendererTypes.hpp

PARENT_SCOPE)

set(WINDOWS_PLATFORM_SOURCES

${CMAKE_CURRENT_LIST_DIR}/Platform/Platform.hpp
${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/Platform.cpp

${CMAKE_CURRENT_LIST_DIR}/Platform/Timer.hpp
${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/Timer.cpp

PARENT_SCOPE) 