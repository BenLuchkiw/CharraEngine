cmake_minimum_required(VERSION 3.21)

project(CharraEngine
VERSION 1.0
LANGUAGES CXX) 

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(CharraEngine)
add_subdirectory(libs)

set(ALL_SOURCES
${CHARRA_SOURCE}
${VULKAN_SOURCES}
${WINDOWS_PLATFORM_SOURCES}
)

add_executable(CharraEngine WIN32 ${ALL_SOURCES})

# For finding possible errors in the sea of warnings
#set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} 
#/W4
#/wd4201
#/wd4267
#")

target_link_libraries(CharraEngine "${CMAKE_CURRENT_LIST_DIR}\\Libs\\vulkan\\Lib\\vulkan-1.lib")
include_directories("Libs\\vulkan\\Include")
include_directories("CharraEngine")
# TODO platform stuff

add_definitions(-DVULKAN_VALIDATION_LAYERS)
add_definitions(-DROOT_DIR="${CMAKE_CURRENT_LIST_DIR}")